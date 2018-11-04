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
#include "Observer.h"

#include "GameConfig.h"
#include "Timer.h"
#include "GameStatus_Team.h"
#include "Map.h"
#include "MapTile.h"
#include "Controller.h"
#include "Extra.h"

#include <stdio.h>
#include <math.h>
#include <boost/format.hpp>

#include "Utils.h"

Observer::Observer(int _x, int _y, ClanBomberApplication *_app) :
		GameObject(_x, _y, _app) {
	x = 0;
	y = 0;
	z = Z_OBSERVER;
	end_of_game = false;
	surface = Resources::Observer_observer();
	sprite_nr = 0;
	offset_x = 10;
	offset_y = 2;
	speed = 300;

	mode = NORMAL;
	for (auto bomber_object_iter : app->bomber_objects) {
		if ((bomber_object_iter)->get_team() != 0) {
			mode = TEAM;
		}
	}
	if (mode == TEAM) {
		game_status = new GameStatus_Team(_app);
	} else {
		game_status = new GameStatus(_app);
	}

	reset_round_time();
	first_destruction = true;
	repaint_hack = 0;
}

Observer::~Observer() {
	delete game_status;
}

bool Observer::end_of_game_requested() {
	return end_of_game;
}

int Observer::active_players() {
	int c = 0;
	int t = -1;

	for (auto bomber_object_iter : app->bomber_objects) {
		if (!(bomber_object_iter)->dead) {
			if (((bomber_object_iter)->get_team() != t)
					|| ((bomber_object_iter)->get_team() == 0)) {
				t = (bomber_object_iter)->get_team();
				c++;
			}
		}
	}
	return c;
}

float Observer::get_round_time() const {
	return round_time;
}

void Observer::reset_round_time() {
	round_time = Config::get_round_time();
	play_hurryup = true;
	first_destruction = true;
	repaint_hack = 0;
	x = 0;
	y = 0;
	flying = false;
}

void Observer::act() {
	GameObject::act();

	if (round_time == Config::get_round_time()) {
		for (auto bomber_object_iter : app->bomber_objects) {
			if ((bomber_object_iter)->is_flying()) {
				return;
			}
		}
		for (auto bomber_object_iter : app->bomber_objects) {
			(bomber_object_iter)->controller->activate();
		}
	}
	round_time -= Timer::time_elapsed();
	if (round_time < 0) {
		offset_x = 60;
		offset_y = 40;
		if (!flying) {
			if (first_destruction) {
				Resources::Observer_time_over()->play();
				first_destruction = false;
			} else {
				MapTile* vanish_maptile = get_maptile();
				vanish_maptile->vanish();
				Resources::Observer_crunch()->play();
			}
			MapTile* maptile = app->map->get_random();
			if (maptile) {
				fly_to(maptile, 500);
			}
		}
	}
	if (active_players() < 2) {

		for (auto object_iter : app->objects) {
			if ((object_iter)->get_type() == GameObject::EXPLOSION) {
				return;
			}
			if ((object_iter)->get_type() == GameObject::EXTRA
					&& (object_iter)->is_flying()) {
				(object_iter)->show();
				return;
			}
		}

		for (auto bomber_object_iter : app->bomber_objects) {
			if (!(bomber_object_iter)->dead
					&& (bomber_object_iter)->is_falling()) {
				return;
			}
		}
		offset_x = 10;
		offset_y = 2;

		for (auto bomber_object_iter : app->bomber_objects) {
			if (!(bomber_object_iter)->dead) {
				(bomber_object_iter)->inc_points();
			}
		}
		if (Config::get_random_map_order()) {
			app->map->load_random_valid();
		} else {
			app->map->load_next_valid();
		}

		reset_round_time();
		game_status->analyze_game();
		game_status->show();
		Timer::reset();
		end_of_game = game_status->get_end_of_game(); //FIXME
		for (auto object_iter : app->objects) {
			delete object_iter;
		}
		app->objects.clear();
		CL_Vector pos;
		if (Config::get_random_positions()) {
			app->map->randomize_bomber_positions();
		}

		int c = 0;

		for (auto bomber_object_iter : app->bomber_objects) {
			pos = app->map->get_bomber_pos(c++);
			(bomber_object_iter)->set_orig((int) (pos.x * 40),
					(int) (pos.y * 40));
			(bomber_object_iter)->reset();
			(bomber_object_iter)->set_pos(350, 270);
			(bomber_object_iter)->fly_to((int) (pos.x * 40), (int) (pos.y * 40),
					200);
			(bomber_object_iter)->controller->deactivate();
		}
	}
}

void Observer::kill_all_bombers() {
	for (auto bomber_object_iter : app->bomber_objects) {
		(bomber_object_iter)->die();
	}
}

void Observer::show() {

	Resources::Game_cb_logo_small()->blit(325, 0);
	GameObject::show();

	if (round_time > 0) {
		Resources::Observer_observer()->put_screen(600, 1, 1);
		Resources::Font_big()->render(
				str(boost::format("%d") % (int) round_time), 650, 3,
				cbe::FontAlignment_0topleft);
	} else {
		Resources::Observer_observer()->put_screen(600, 1, 2);
	}

	Resources::Font_small()->render(app->map->get_name(), 80, 5,
			cbe::FontAlignment_0topleft);

	if ((round_time < 21) && (round_time > 18)) {
		if (fmod(round_time, 0.3f) > 0.15f) {

			CB_FillRect(300, 250, 200, 30, 0, 0, 0, 128);

			Resources::Font_small()->render(_("HURRY UP !!!!"), 400, 255,
					cbe::FontAlignment_0topcenter);
		}
		if (play_hurryup) {
			Resources::Observer_hurry_up()->play();
			play_hurryup = false;
		}
	}
}

GameStatus* Observer::get_game_status() {
	return game_status;
}
