/*
 * This file is part of ClanBomber;
 * you can get it at "http://www.nongnu.org/clanbomber".
 *
 * Copyright (C) 1999-2004, 2007 Andreas Hundt, Denis Oliver Kropp
 * Copyright (C) 2009, 2010 Rene Lopez <rsl@members.fsf.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "ClanBomber.h"
#include "Controller_AI.h"

#include "Bomber.h"
#include "Bomb.h"
#include "Explosion.h"
#include "Extra.h"
#include "Map.h"
#include "MapTile.h"
#include "MapTile_Arrow.h"
#include "Timer.h"
#include "GameConfig.h"

#include <assert.h>
#include <boost/ptr_container/ptr_list.hpp>

/* ---------- Job ----------- */

Job::Job(Controller_AI* _controller) {
	finished = false;
	obsolete = false;
	controller = _controller;
	bomber = controller->bomber;
	app = bomber->app;
}

Job::~Job() {
}

bool Job::is_finished() {
	return finished;
}

bool Job::is_obsolete() {
	return obsolete;
}

void Job::execute() {
}

void Job::init() {
}

/* ---------- Job_Go ----------- */

Job_Go::Job_Go(Controller_AI* _controller, int _dir, int _distance) :
		Job(_controller) {
	distance = _distance;
	dir = _dir;
	init();
}

Job_Go::~Job_Go() {
	controller->dir = DIR_NONE;
}

void Job_Go::execute() {
	controller->dir = dir;
	switch (dir) {
	case DIR_UP:
		if (controller->bomber->get_map_y() <= start - distance
				&& bomber->get_y() % 40 < 15) {
			finished = true;
			controller->dir = DIR_NONE;
		}
		break;
	case DIR_DOWN:
		if (controller->bomber->get_map_y() >= start + distance
				&& bomber->get_y() % 40 > 25) {
			finished = true;
			controller->dir = DIR_NONE;
		}
		break;
	case DIR_LEFT:
		if (controller->bomber->get_map_x() <= start - distance
				&& bomber->get_x() % 40 < 15) {
			finished = true;
			controller->dir = DIR_NONE;
		}
	case DIR_RIGHT:
		if (controller->bomber->get_map_x() >= start + distance
				&& bomber->get_x() % 40 > 25) {
			finished = true;
			controller->dir = DIR_NONE;
		}
		break;
	default:
		obsolete = true;
		controller->dir = DIR_NONE;
		break;
	}
	if (controller->bomber->is_stopped()) {
		obsolete = true;
	}
}

void Job_Go::init() {
	switch (dir) {
	case DIR_UP:
		if (controller->is_death(controller->bomber->get_map_x(),
				controller->bomber->get_map_y() - 1)) {
			obsolete = true;
			controller->dir = DIR_NONE;
		}
		start = controller->bomber->get_map_y();
		break;
	case DIR_DOWN:
		if (controller->is_death(controller->bomber->get_map_x(),
				controller->bomber->get_map_y() + 1)) {
			obsolete = true;
			controller->dir = DIR_NONE;
		}
		start = controller->bomber->get_map_y();
		break;
	case DIR_LEFT:
		if (controller->is_death(controller->bomber->get_map_x() - 1,
				controller->bomber->get_map_y())) {
			obsolete = true;
			controller->dir = DIR_NONE;
		}
		start = controller->bomber->get_map_x();
		break;
	case DIR_RIGHT:
		if (controller->is_death(controller->bomber->get_map_x() + 1,
				controller->bomber->get_map_y())) {
			obsolete = true;
			controller->dir = DIR_NONE;
		}
		start = controller->bomber->get_map_x();
		break;
	default:
		obsolete = true;
		controller->dir = DIR_NONE;
		break;
	}
}

/* ---------- Job_Put_Bomb ----------- */

Job_PutBomb::Job_PutBomb(Controller_AI* _controller) :
		Job(_controller) {
}

Job_PutBomb::~Job_PutBomb() {
	controller->put_bomb = false;
}

void Job_PutBomb::execute() {
	controller->put_bomb = true;
	finished = true;
	if (controller->bomber->app->map->get_maptile(
			(controller->bomber->get_x() + 20) / 40,
			(controller->bomber->get_y() + 20) / 40)->bomb) {
		obsolete = true;
		return;
	}
	if (controller->bomber->get_cur_bombs() == 0) {
		obsolete = true;
		return;
	}
}

/* ---------- Job_Wait ----------- */

Job_Wait::Job_Wait(Controller_AI* _controller, float _duration) :
		Job(_controller) {
	duration = _duration;
}

Job_Wait::~Job_Wait() {
}

void Job_Wait::execute() {
	duration -= Timer::time_elapsed();
	controller->put_bomb = false;
	controller->dir = DIR_NONE;
	if (duration <= 0) {
		finished = true;
	}
	if (controller->is_hotspot(controller->bomber->get_map_x(),
			controller->bomber->get_map_y())) {
		obsolete = true;
	}
}

Controller_AI::Controller_AI() :
		Controller() {
	c_type = AI;
}

void Controller_AI::reset() {
	dir = DIR_NONE;
	put_bomb = false;
	map = bomber->app->map;

	clear_all_jobs();
}

void Controller_AI::clear_all_jobs() {
	for (auto jobs_iter : jobs) {
		delete jobs_iter;
	}
	jobs.clear();
}

void Controller_AI::update() {
	if (active) {
		generate_rating_map();

		if (job_ready()) {
			do_job();
		}
	}
}

bool Controller_AI::job_ready() {
	if (jobs.size() == 0) {
		find_new_jobs();
		return jobs.size() > 0;
	}

	if (jobs[0]->is_obsolete()) {
		for (auto jobs_iter : jobs) {
			delete jobs_iter;
		}
		jobs.clear();
		return job_ready();
	}

	if (jobs[0]->is_finished()) {
		delete jobs[0];
		jobs.erase(jobs.begin());
		if (jobs.size() > 0) {
			jobs[0]->init();
		}
		return job_ready();
	}

	return true;
}

void Controller_AI::find_new_jobs() {
	if (avoid_bombs()) {
		return;
	}

	if (find_way( RATING_EXTRA, RATING_HOT, 10)) {
		return;
	}
	if (bomber->get_cur_bombs() && free_extras(5)) {
		return;
	}
}

void Controller_AI::do_job() {
	assert(jobs.size() > 0);
	jobs[0]->execute();
}

bool Controller_AI::avoid_bombs() {
	if (is_hotspot(bomber->get_map_x(), bomber->get_map_y())) {
		if (!find_way(0, -1)) {
			if (!find_way(0, RATING_HOT)) {
				find_way(0, RATING_X);
			}
		}
		return true;
	}

	return false;
}

void Controller_AI::generate_rating_map() {
	int x, y;
	// neutralize the whole map
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			rmap[x][y] = 0;
		}
	}

	for (auto object_iter : bomber->app->objects) {
		x = (object_iter)->get_map_x();
		y = (object_iter)->get_map_y();

		switch ((object_iter)->get_type()) {
		case GameObject::BOMB:
			apply_bomb_rating(x, y,
					static_cast<Bomb*>(object_iter)->get_power(),
					static_cast<Bomb*>(object_iter)->get_countdown(),
					static_cast<Bomb*>(object_iter)->get_cur_dir());
			break;
		case GameObject::EXPLOSION: {
			int i;
			int power = static_cast<Explosion*>(object_iter)->get_power();

			rmap[x][y] += RATING_X;
			for (i = 1; (i <= power && x + i < MAP_WIDTH); i++) {
				if (map->get_maptile(x + i, y)->blocking
						|| map->get_maptile(x + i, y)->has_extra())
					break;
				rmap[x + i][y] += RATING_X;
			}
			for (i = 1; (i <= power && x - i >= 0); i++) {
				if (map->get_maptile(x - i, y)->blocking
						|| map->get_maptile(x - i, y)->has_extra())
					break;
				rmap[x - i][y] += RATING_X;
			}
			for (i = 1; (i <= power && y + i < MAP_HEIGHT); i++) {
				if (map->get_maptile(x, y + i)->blocking
						|| map->get_maptile(x, y + i)->has_extra())
					break;
				rmap[x][y + i] += RATING_X;
			}
			for (i = 1; (i <= power && y - i >= 0); i++) {
				if (map->get_maptile(x, y - i)->blocking
						|| map->get_maptile(x, y - i)->has_extra())
					break;
				rmap[x][y - i] += RATING_X;
			}
			break;
		}
		case GameObject::EXTRA:
			rmap[x][y] += RATING_EXTRA
					* static_cast<Extra*>(object_iter)->get_ExtraType();
			if (rmap[x][y] < 0) {
				rmap[x][y] = RATING_HOT;
			}
			break;
		default:
			break;
		}
	}

	// apply maptile ratings
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			if (map->get_maptile_fast(x, y)->blocking) {
				rmap[x][y] += RATING_BLOCKING;
			} else if (!map->get_maptile_fast(x, y)->passable) {
				rmap[x][y] += RATING_X;
			}
		}
	}

}

void Controller_AI::apply_bomb_rating(int x, int y, int power, float countdown,
		Direction dir) {
	int rating = RATING_HOT;
	if (countdown > Config::get_bomb_countdown() / 10.0f - 0.1f) {
		rating = -1;
	} else if (countdown < 40.0f / (float) bomber->get_speed()) {
		rating = RATING_X;
	}

	rmap[x][y] =
			bomber->is_able_to_kick()
					&& (map->get_maptile(x, y)->get_type() != MapTile::ARROW) ?
					rating : RATING_BLOCKING;

	int length = 1;
	switch (dir) {
	case DIR_UP:
		while (!map->get_maptile(x, y - length)->blocking && y - length >= 0) {
			length++;
			if (map->get_maptile(x, y - length)->get_type() == MapTile::ARROW
					&& ((MapTile_Arrow*) (map->get_maptile(x, y - length)))->get_direction()
							!= DIR_UP) {
				break;
			}
		}
		break;
	case DIR_DOWN:
		while (!map->get_maptile(x, y + length)->blocking
				&& y + length < MAP_HEIGHT) {
			length++;
			if (map->get_maptile(x, y + length)->get_type() == MapTile::ARROW
					&& ((MapTile_Arrow*) (map->get_maptile(x, y + length)))->get_direction()
							!= DIR_DOWN) {
				break;
			}
		}
		break;
	case DIR_LEFT:
		while (!map->get_maptile(x - length, y)->blocking && x - length >= 0) {
			length++;
			if (map->get_maptile(x - length, y)->get_type() == MapTile::ARROW
					&& ((MapTile_Arrow*) (map->get_maptile(x - length, y)))->get_direction()
							!= DIR_LEFT) {
				break;
			}
		}
		break;
	case DIR_RIGHT:
		while (!map->get_maptile(x + length, y)->blocking
				&& x + length < MAP_WIDTH) {
			length++;
			if (map->get_maptile(x + length, y)->get_type() == MapTile::ARROW
					&& ((MapTile_Arrow*) (map->get_maptile(x + length, y)))->get_direction()
							!= DIR_RIGHT) {
				break;
			}
		}
		break;
	default:
		break;
	}

	while (length) {
		int i;
		for (i = 1; (i <= power && x + i < MAP_WIDTH); i++) {
			rmap[x + i][y] += rating;
			if (map->get_maptile(x + i, y)->blocking
					|| map->get_maptile(x + i, y)->has_extra())
				break;
		}
		for (i = 1; (i <= power && x - i >= 0); i++) {
			rmap[x - i][y] += rating;
			if (map->get_maptile(x - i, y)->blocking
					|| map->get_maptile(x - i, y)->has_extra())
				break;
		}
		for (i = 1; (i <= power && y + i < MAP_HEIGHT); i++) {
			rmap[x][y + i] += rating;
			if (map->get_maptile(x, y + i)->blocking
					|| map->get_maptile(x, y + i)->has_extra())
				break;
		}
		for (i = 1; (i <= power && y - i >= 0); i++) {
			rmap[x][y - i] += rating;
			if (map->get_maptile(x, y - i)->blocking
					|| map->get_maptile(x, y - i)->has_extra())
				break;
		}

		switch (dir) {
		case DIR_UP:
			y--;
			break;
		case DIR_DOWN:
			y++;
			break;
		case DIR_LEFT:
			x--;
			break;
		case DIR_RIGHT:
			x++;
			break;
		default:
			break;
		}

		length--;
	}
}

bool Controller_AI::find_way(int dest_rating, int avoid_rating,
		int max_distance) {
	int x, y;
	int vmap[MAP_WIDTH][MAP_HEIGHT];

	// set unvisited
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			vmap[x][y] = -1;
		}
	}

	boost::ptr_list<CL_Vector> new_queue;
	boost::ptr_list<CL_Vector> working_queue;
	int distance = 0;
	CL_Vector start(bomber->get_map_x(), bomber->get_map_y());
	CL_Vector dest(-1, -1, -1);

	vmap[(int) (start.x)][(int) (start.y)] = 0;
	new_queue.push_back(new CL_Vector(start));

	do {
		working_queue.transfer(working_queue.end(), new_queue.begin(),
				new_queue.end(), new_queue);
		distance++;

		do {
			boost::ptr_list<CL_Vector>::auto_type v = working_queue.release(
					working_queue.begin());

			Direction dirs[4] = { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT };

			for (int per = 0; per < 10; per++) {

				int a = WELLRNG512() % 4;
				int b = WELLRNG512() % 4;
				Direction dummy;
				dummy = dirs[a];
				dirs[a] = dirs[b];
				dirs[b] = dummy;
			}

			for (int dir = 0; dir < 4; dir++) {
				switch (dirs[dir]) {
				case DIR_UP:
					if (v->y == 0)
						break;
					v->y--;
					if (rmap[(int) (v->x)][(int) (v->y)] > avoid_rating
							&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
						new_queue.push_back(new CL_Vector(*v));
						vmap[(int) (v->x)][(int) (v->y)] = distance;
						if (rmap[(int) (v->x)][(int) (v->y)] >= dest_rating) {
							dest = *v;
						}
					}
					v->y++;
					break;

				case DIR_RIGHT:
					if (v->x == MAP_WIDTH - 1)
						break;
					v->x++;
					if (rmap[(int) (v->x)][(int) (v->y)] > avoid_rating
							&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
						new_queue.push_back(new CL_Vector(*v));
						vmap[(int) (v->x)][(int) (v->y)] = distance;
						if (rmap[(int) (v->x)][(int) (v->y)] >= dest_rating) {
							dest = *v;
						}
					}
					v->x--;
					break;

				case DIR_DOWN:
					if (v->y == MAP_HEIGHT - 1)
						break;
					v->y++;
					if (rmap[(int) (v->x)][(int) (v->y)] > avoid_rating
							&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
						new_queue.push_back(new CL_Vector(*v));
						vmap[(int) (v->x)][(int) (v->y)] = distance;
						if (rmap[(int) (v->x)][(int) (v->y)] >= dest_rating) {
							dest = *v;
						}
					}
					v->y--;
					break;

				case DIR_LEFT:
					if (v->x == 0)
						break;
					v->x--;
					if (rmap[(int) (v->x)][(int) (v->y)] > avoid_rating
							&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
						new_queue.push_back(new CL_Vector(*v));
						vmap[(int) (v->x)][(int) (v->y)] = distance;
						if (rmap[(int) (v->x)][(int) (v->y)] >= dest_rating) {
							dest = *v;
						}
					}
					v->x++;
					break;
				default:
					break;
				}
			}
		} while (dest.x < 0 && working_queue.size() > 0);
	} while (max_distance >= distance && dest.x < 0 && new_queue.size() > 0);

	if (dest.x < 0) {
		return false;
	}

	std::vector<Job*> rev_way;

	while (start != dest) {
		distance--;

		if (dest.y > 0) {
			dest.y--;
			if (vmap[(int) (dest.x)][(int) (dest.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_DOWN));
				continue;
			}
			dest.y++;
		}

		if (dest.x < MAP_WIDTH - 1) {
			dest.x++;
			if (vmap[(int) (dest.x)][(int) (dest.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_LEFT));
				continue;
			}
			dest.x--;
		}

		if (dest.y < MAP_HEIGHT - 1) {
			dest.y++;
			if (vmap[(int) (dest.x)][(int) (dest.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_UP));
				continue;
			}
			dest.y--;
		}

		if (dest.x > 0) {
			dest.x--;
			if (vmap[(int) (dest.x)][(int) (dest.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_RIGHT));
				continue;
			}
			dest.x++;
		}
	}

	if (dest_rating > 0) //hack: seeking an extra?
			{
		jobs.push_back(rev_way.back());
	} else {
		for (int i = rev_way.size() - 1; i >= 0; i--) {
			jobs.push_back(rev_way[i]);
		}
	}

	rev_way.clear();

	return true;
}

bool Controller_AI::is_hotspot(int x, int y) {
	return (rmap[x][y] <= RATING_HOT);
}

bool Controller_AI::is_death(int x, int y) {
	return (rmap[x][y] <= RATING_X);
}

bool Controller_AI::free_extras(int max_distance) {
	int x, y;
	int vmap[MAP_WIDTH][MAP_HEIGHT];
	int dmap[MAP_WIDTH][MAP_HEIGHT];

	// set unvisited
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			vmap[x][y] = -1;
		}
	}

	// set neutral
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			dmap[x][y] = 0;
		}
	}

	boost::ptr_list<CL_Vector> new_queue;
	boost::ptr_list<CL_Vector> working_queue;
	int distance = 0;
	CL_Vector start(bomber->get_map_x(), bomber->get_map_y());
	CL_Vector best(-1, -1, 0);

	vmap[(int) (start.x)][(int) (start.y)] = 0;
	new_queue.push_back(new CL_Vector(start));

	do {
		working_queue.transfer(working_queue.end(), new_queue.begin(),
				new_queue.end(), new_queue);
		distance++;

		do {
			boost::ptr_list<CL_Vector>::auto_type v = working_queue.release(
					working_queue.begin());

			int i;
			x = (int) (v->x);
			y = (int) (v->y);

			for (i = 1; (i <= bomber->get_power() && x + i < MAP_WIDTH); i++) {
				if (map->get_maptile(x + i, y)->get_type() == MapTile::WALL) {
					break;
				}
				dmap[x][y] += bomber_rating(x + i, y); // friends or enemies?
				if (map->get_maptile(x + i, y)->bomb) // please no explosion domino
				{
					dmap[x][y] += DRATING_BOMB;
					break;
				}
				if (map->get_maptile(x + i, y)->has_extra()) // dont blast that extras!
				{
					dmap[x][y] += extra_rating(x + i, y);
					break;
				}
				if (rmap[x + i][y] < RATING_BLOCKING
						|| map->get_maptile(x + i, y)->destroyed) // a box already being destroyed or going to be
						{
					dmap[x][y] += DRATING_DBOX;
					break;
				}
				if (map->get_maptile(x + i, y)->get_type() == MapTile::BOX) {
					dmap[x][y] += DRATING_BOX;
					break;
				}
				if (map->get_maptile(x + i, y)->blocking) {
					break;
				}
			}
			for (i = 1; (i <= bomber->get_power() && x - i >= 0); i++) {
				if (map->get_maptile(x - i, y)->get_type() == MapTile::WALL) {
					break;
				}
				dmap[x][y] += bomber_rating(x - i, y); // friends or enemies?
				if (map->get_maptile(x - i, y)->bomb) // please no explosion domino
				{
					dmap[x][y] += DRATING_BOMB;
					break;
				}
				if (map->get_maptile(x - i, y)->has_extra()) // dont blast that extras!
				{
					dmap[x][y] += extra_rating(x - i, y);
					break;
				}
				if (rmap[x - i][y] < RATING_BLOCKING
						|| map->get_maptile(x - i, y)->destroyed) // a box already being destroyed or going to be
						{
					dmap[x][y] += DRATING_DBOX;
					break;
				}
				if (map->get_maptile(x - i, y)->get_type() == MapTile::BOX) {
					dmap[x][y] += DRATING_BOX;
					break;
				}
				if (map->get_maptile(x - i, y)->blocking) {
					break;
				}
			}
			for (i = 1; (i <= bomber->get_power() && y + i < MAP_HEIGHT); i++) {
				if (map->get_maptile(x, y + i)->get_type() == MapTile::WALL) {
					break;
				}
				dmap[x][y] += bomber_rating(x, y + i); // friends or enemies?
				if (map->get_maptile(x, y + i)->bomb) // please no explosion domino
				{
					dmap[x][y] += DRATING_BOMB;
					break;
				}
				if (map->get_maptile(x, y + i)->has_extra()) // dont blast that extras!
				{
					dmap[x][y] += extra_rating(x, y + i);
					break;
				}
				if (rmap[x][y + i] < RATING_BLOCKING
						|| map->get_maptile(x, y + i)->destroyed) // a box already being destroyed or going to be
						{
					dmap[x][y] += DRATING_DBOX;
					break;
				}
				if (map->get_maptile(x, y + i)->get_type() == MapTile::BOX) {
					dmap[x][y] += DRATING_BOX;
					break;
				}
				if (map->get_maptile(x, y + i)->blocking) {
					break;
				}
			}
			for (i = 1; (i <= bomber->get_power() && y - i >= 0); i++) {
				if (map->get_maptile(x, y - i)->get_type() == MapTile::WALL) {
					break;
				}
				dmap[x][y] += bomber_rating(x, y - i); // friends or enemies?
				if (map->get_maptile(x, y - i)->bomb) // please no explosion domino
				{
					dmap[x][y] += DRATING_BOMB;
					break;
				}
				if (map->get_maptile(x, y - i)->has_extra()) // dont blast that extras!
				{
					dmap[x][y] += extra_rating(x, y - i);
					break;
				}
				if (rmap[x][y - i] < RATING_BLOCKING
						|| map->get_maptile(x, y - i)->destroyed) // a box already being destroyed or going to be
						{
					dmap[x][y] += DRATING_DBOX;
					break;
				}
				if (map->get_maptile(x, y - i)->get_type() == MapTile::BOX) {
					dmap[x][y] += DRATING_BOX;
					break;
				}
				if (map->get_maptile(x, y - i)->blocking) {
					break;
				}
			}

			// is there a way out if a bomb was put here?
			if (dmap[x][y] > 0) {
				int rmap_org[MAP_WIDTH][MAP_HEIGHT];
				int x_org = bomber->get_x();
				int y_org = bomber->get_y();
				int xx, yy;

				for (yy = 0; yy < MAP_HEIGHT; yy++)
					for (xx = 0; xx < MAP_WIDTH; xx++)
						rmap_org[xx][yy] = rmap[xx][yy];

				bomber->set_pos(x * 40, y * 40);
				apply_bomb_rating(x, y, bomber->get_power(),
						Config::get_bomb_countdown() / 10.0f, DIR_NONE);
				rmap[x][y] = -1;
				if (!find_way(0, RATING_HOT)) {
					dmap[x][y] = -1000;
				}

				clear_all_jobs();
				for (yy = 0; yy < MAP_HEIGHT; yy++)
					for (xx = 0; xx < MAP_WIDTH; xx++)
						rmap[xx][yy] = rmap_org[xx][yy];
				bomber->set_pos(x_org, y_org);
			}

			if (dmap[x][y] > 0 && dmap[x][y] == best.z && WELLRNG512() % 2) {
				if (distance >= max_distance) {
					best.z = 666;
					break;
				}
				best.x = x;
				best.y = y;
			}

			if (dmap[x][y] > best.z) {
				if (best.z > 0 && distance >= max_distance) {
					best.z = 666;
					break;
				}
				best.x = x;
				best.y = y;
				best.z = dmap[x][y];
			}

			if (v->y > 0) {
				v->y--;
				if (rmap[(int) (v->x)][(int) (v->y)] > RATING_HOT
						&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
					new_queue.push_back(new CL_Vector(*v));
					vmap[(int) (v->x)][(int) (v->y)] = distance;
				}
				v->y++;
			}

			if (v->x < MAP_WIDTH - 1) {
				v->x++;
				if (rmap[(int) (v->x)][(int) (v->y)] > RATING_HOT
						&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
					new_queue.push_back(new CL_Vector(*v));
					vmap[(int) (v->x)][(int) (v->y)] = distance;
				}
				v->x--;
			}

			if (v->y < MAP_HEIGHT - 1) {
				v->y++;
				if (rmap[(int) (v->x)][(int) (v->y)] > RATING_HOT
						&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
					new_queue.push_back(new CL_Vector(*v));
					vmap[(int) (v->x)][(int) (v->y)] = distance;
				}
				v->y--;
			}

			if (v->x > 0) {
				v->x--;
				if (rmap[(int) (v->x)][(int) (v->y)] > RATING_HOT
						&& vmap[(int) (v->x)][(int) (v->y)] < 0) {
					new_queue.push_back(new CL_Vector(*v));
					vmap[(int) (v->x)][(int) (v->y)] = distance;
				}
				v->x++;
			}
		} while (working_queue.size() > 0);
	} while (new_queue.size() > 0 && best.z != 666);

	if (best.x < 0) {
		return false;
	}

	std::vector<Job*> rev_way;
	distance = vmap[(int) (best.x)][(int) (best.y)];
	start.z = best.z;

	while (start != best) {
		distance--;

		if (best.y > 0) {
			best.y--;
			if (vmap[(int) (best.x)][(int) (best.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_DOWN));
				continue;
			}
			best.y++;
		}

		if (best.x < MAP_WIDTH - 1) {
			best.x++;
			if (vmap[(int) (best.x)][(int) (best.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_LEFT));
				continue;
			}
			best.x--;
		}

		if (best.y < MAP_HEIGHT - 1) {
			best.y++;
			if (vmap[(int) (best.x)][(int) (best.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_UP));
				continue;
			}
			best.y--;
		}

		if (best.x > 0) {
			best.x--;
			if (vmap[(int) (best.x)][(int) (best.y)] == distance) {
				rev_way.push_back(new Job_Go(this, DIR_RIGHT));
				continue;
			}
			best.x++;
		}
	}

	for (int i = rev_way.size() - 1; i >= 0; i--) {
		jobs.push_back(rev_way[i]);
//		if (jobs.get_num_items() > 3)
//			break;
	}
	rev_way.clear();

//	if (jobs.get_num_items() < 3)
	jobs.push_back(new Job_PutBomb(this));

	return true;
}

int Controller_AI::bomber_rating(int x, int y) {
	int rating = 0;

	for (auto object_iter : map->get_maptile(x, y)->objects) {
		if ((object_iter)->get_type() == GameObject::BOMBER) {
			Bomber* b = static_cast<Bomber*>(object_iter);
			if (bomber != b
					&& (b->get_team() == 0
							|| b->get_team() != bomber->get_team())) {
				rating += DRATING_ENEMY;
			} else if (bomber != b) {
				rating += DRATING_FRIEND;
			}
		}
	}
	return rating;
}

int Controller_AI::extra_rating(int x, int y) {
	int rating = 0;

	for (auto object_iter : map->get_maptile(x, y)->objects) {
		if ((object_iter)->get_type() == GameObject::EXTRA) {
			rating +=
					static_cast<Extra*>(object_iter)->get_ExtraType() * DRATING_EXTRA;
		}
	}
	if (rating > 0) {
		rating = DRATING_DISEASE;
	}
	return rating;
}

bool Controller_AI::is_left() {
	return dir == DIR_LEFT && active;
}

bool Controller_AI::is_right() {
	return dir == DIR_RIGHT && active;
}

bool Controller_AI::is_up() {
	return dir == DIR_UP && active;
}

bool Controller_AI::is_down() {
	return dir == DIR_DOWN && active;
}

bool Controller_AI::is_bomb() {
	switch (bomb_mode) {
	case NEVER:
		return false;
	case ALWAYS:
		return true;
	default:
		break;
	}
	return put_bomb && active;
}
