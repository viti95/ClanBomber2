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
#include "GameObject.h"

#include "Map.h"
#include "MapTile.h"
#include "Timer.h"
#include "Bomber.h"
#include "Bomb.h"
#include "Utils.h"

#include <iostream>
#include <math.h>

const char* GameObject::objecttype2string(ObjectType t) {
	switch (t) {
	case GameObject::BOMB:
		return "*BOMB*";
	case GameObject::BOMBER:
		return "*BOMBER*";
	case GameObject::BOMBER_CORPSE:
		return "*BOMBER_CORPSE*";
	case GameObject::EXPLOSION:
		return "*EXPLOSION*";
	case GameObject::EXTRA:
		return "*EXTRA*";
	case GameObject::OBSERVER:
		return "*OBSERVER*";
	case GameObject::CORPSE_PART:
		return "*CORPSE_PART*";
	}
	return "*UNKNOWN*";
}

GameObject::GameObject(int _x, int _y, ClanBomberApplication *_app) {
	app = _app;

	offset_x = 60;
	offset_y = 40;
	delete_me = false;
	remainder = 0;
	speed = 240;

	cur_dir = DIR_NONE;
	can_kick = false;
	can_pass_bomber = false;
	can_fly_over_walls = true;
	flying = false;
	falling = false;
	fallen_down = false;
	stopped = false;
	fly_progress = 0;	// must be set to 0!

	x = orig_x = _x;
	y = orig_y = _y;
	z = 0;

	opacity = 0xff;
	opacity_scaled = 0xff;

	object_id = 0;

	local_dir = cur_dir;
	reset_next_fly_job();
}

GameObject::~GameObject() {
}

int GameObject::get_object_id() {
	return object_id;
}

void GameObject::set_object_id(int obj_id) {
	object_id = obj_id;
}

int GameObject::get_orig_x() {
	return orig_x;
}

int GameObject::get_orig_y() {
	return orig_y;
}

void GameObject::set_cur_dir(int cd) {
	cur_dir = (Direction) cd;
}

void GameObject::set_offset(int _x, int _y) {
	offset_x = _x;
	offset_y = _y;
}

bool GameObject::move_right() {
	MapTile* right_maptile = app->map->get_maptile_xy((int) (x + 40),
			(int) (y + 20));

	if (right_maptile->blocking) {
		return false;
	}

	if ((right_maptile->bomb == NULL)
			|| ((get_type() == BOMB) && (right_maptile->bomb == this))
			|| (get_x() % 40 > 19)) {
		x++;

		if (get_y() % 40 > 19) {
			if (app->map->get_maptile((get_x() + 40) / 40, (get_y() - 20) / 40)->blocking) {
				y++;
			}
		} else if (get_y() % 40 > 0) {
			if (app->map->get_maptile((get_x() + 40) / 40, (get_y() + 60) / 40)->blocking) {
				y--;
			}
		}

		if (!can_pass_bomber) {
			if (right_maptile->has_bomber()) {
				if (right_maptile != get_maptile()) {
					x--;
					return false;
				}
			}
		}
	} else {
		if (can_kick && (right_maptile->bomb->get_cur_dir() == DIR_NONE)) {
			right_maptile->bomb->kick(DIR_RIGHT);
		}
		return false;
	}

	return true;
}

bool GameObject::move_left() {
	MapTile* left_maptile = app->map->get_maptile_xy((int) (x - 1),
			(int) (y + 20));
	if (left_maptile->blocking) {
		return false;
	}

	if ((left_maptile->bomb == NULL)
			|| ((get_type() == BOMB) && (left_maptile->bomb == this))
			|| (get_x() % 40 < 20)) {
		x--;

		if (get_y() % 40 > 19) {
			if (app->map->get_maptile((get_x() - 1) / 40, (get_y() - 20) / 40)->blocking) {
				y++;
			}
		} else if (get_y() % 40 > 0) {
			if (app->map->get_maptile((get_x() - 1) / 40, (get_y() + 60) / 40)->blocking) {
				y--;
			}
		}

		if (!can_pass_bomber) {
			if (left_maptile->has_bomber()) {
				if (left_maptile != get_maptile()) {
					x++;
					return false;
				}
			}
		}
	} else {
		if (can_kick && (left_maptile->bomb->get_cur_dir() == DIR_NONE)) {
			left_maptile->bomb->kick(DIR_LEFT);
		}
		return false;
	}
	return true;
}

bool GameObject::move_up() {
	MapTile* up_maptile = app->map->get_maptile_xy((int) (x + 20),
			(int) (y - 1));
	if (up_maptile->blocking) {
		return false;
	}

	if ((up_maptile->bomb == NULL)
			|| ((get_type() == BOMB) && (up_maptile->bomb == this))
			|| (get_y() % 40 < 20)) {
		y--;

		if (get_x() % 40 > 19) {
			if (app->map->get_maptile((get_x() - 20) / 40, (get_y() - 1) / 40)->blocking) {
				x++;
			}
		} else if (get_x() % 40 > 0) {
			if (app->map->get_maptile((get_x() + 60) / 40, (get_y() - 1) / 40)->blocking) {
				x--;
			}
		}

		if (!can_pass_bomber) {
			if (up_maptile->has_bomber()) {
				if (up_maptile != get_maptile()) {
					y++;
					return false;
				}
			}
		}
	} else {
		if (can_kick && (up_maptile->bomb->get_cur_dir() == DIR_NONE)) {
			up_maptile->bomb->kick(DIR_UP);
		}
		return false;
	}

	return true;
}

bool GameObject::move_down() {
	MapTile* down_maptile = app->map->get_maptile_xy((int) (x + 20),
			(int) (y + 40));

	if (down_maptile->blocking) {
		return false;
	}

	if ((down_maptile->bomb == NULL)
			|| ((get_type() == BOMB) && (down_maptile->bomb == this))
			|| (get_y() % 40 > 19)) {
		y++;

		if (get_x() % 40 > 19) {
			if (app->map->get_maptile((get_x() - 20) / 40, (get_y() + 40) / 40)->blocking) {
				x++;
			}
		} else if (get_x() % 40 > 0) {
			if (app->map->get_maptile((get_x() + 60) / 40, (get_y() + 40) / 40)->blocking) {
				x--;
			}
		}

		if (!can_pass_bomber) {
			if (down_maptile->has_bomber()) {
				if (down_maptile != get_maptile()) {
					y--;
					return false;
				}
			}
		}
	} else {
		if (can_kick && (down_maptile->bomb->get_cur_dir() == DIR_NONE)) {
			down_maptile->bomb->kick(DIR_DOWN);
		}
		return false;
	}

	return true;
}

bool GameObject::move() {
	if (!flying) {
		int span = (int) (Timer::time_elapsed() * speed);
		remainder += Timer::time_elapsed() * speed
				- (int) (Timer::time_elapsed() * speed);
		span += (int) (remainder);
		remainder -= (int) (remainder);

		for (int i = 0; i < span; i++) {
			switch (cur_dir) {
			case DIR_LEFT:
				if (!move_left()) {
					stop();
					return false;
				}
				break;
			case DIR_RIGHT:
				if (!move_right()) {
					stop();
					return false;
				}
				break;
			case DIR_UP:
				if (!move_up()) {
					stop();
					return false;
				}
				break;
			case DIR_DOWN:
				if (!move_down()) {
					stop();
					return false;
				}
				break;
			default:
				break;
			}
		}
	}
	return true;
}

void GameObject::act() {
	stopped = false;
	if (flying) {
		continue_flying();
		if (flying && is_next_fly_job()) {
			flying = false;
			fly_progress = 1;
			x = fly_dest_x;
			y = fly_dest_y;
		}
		return;
	}

	if (falling) {
		continue_falling();
	}
}

void GameObject::fly_to(int _x, int _y, int _speed) {
	if (!flying && !falling && !fallen_down) {
		flying = true;

		fly_dest_x = _x;
		fly_dest_y = _y;

		fly_progress = 0;	// goes from 0 to 1
		fly_dist_x = _x - x;
		fly_dist_y = _y - y;

		fly_speed = _speed ? _speed : speed;
		int send_speed = (int) fly_speed;
		fly_speed /= sqrt(fly_dist_x * fly_dist_x + fly_dist_y * fly_dist_y);
		z += Z_FLYING;
	}
}

void GameObject::fly_to(MapTile *maptile, int _speed) {
	if (maptile) {
		fly_to(maptile->get_x(), maptile->get_y(), _speed);
	}
}

void GameObject::continue_flying() {
	if (Timer::time_elapsed() == 0) {
		return;
	}
	float time_span = Timer::time_elapsed();
	float time_step = time_span;
	int steps = 1;
	while (abs((int) (time_step * fly_speed * fly_dist_x)) > 5
			|| abs((int) (time_step * fly_speed * fly_dist_y)) > 5) {
		time_step /= 2.0f;
		steps *= 2;
	}

	while (steps--) {
		x += time_step * fly_speed * fly_dist_x;
		y += time_step * fly_speed * fly_dist_y;

		if (get_type() == CORPSE_PART && app->map != NULL) {
			if (!can_fly_over_walls
					&& get_maptile()->get_type() == MapTile::WALL) {
				x -= time_step * fly_speed * fly_dist_x;
				y -= time_step * fly_speed * fly_dist_y;
				fly_dest_x = x;
				fly_dest_y = y;
//				break;
			}
		} else {
			if (!can_fly_over_walls && app->map != NULL
					&& (app->map->get_maptile_xy((int) (x), (int) (y))->get_type()
							== MapTile::WALL
							|| app->map->get_maptile_xy((int) (x + 39),
									(int) (y))->get_type() == MapTile::WALL
							|| app->map->get_maptile_xy((int) (x),
									(int) (y + 39))->get_type() == MapTile::WALL
							|| app->map->get_maptile_xy((int) (x + 39),
									(int) (y + 39))->get_type() == MapTile::WALL)) {
				x -= time_step * fly_speed * fly_dist_x;
				y -= time_step * fly_speed * fly_dist_y;
				fly_dest_x = x;
				fly_dest_y = y;
//				break;
			}
		}

		fly_progress += time_step * fly_speed;

		if (fly_progress >= 1) {
			flying = false;
			fly_progress = 1;// do not set to 0! 1 indicates a finished flight!
			if (z > Z_FLYING) {
				z -= Z_FLYING;
			}
			x = fly_dest_x;
			y = fly_dest_y;
		}
	}
}

bool GameObject::move(int _speed, Direction _dir) {
	if (_dir == DIR_NONE && cur_dir != DIR_NONE) {
		return move(_speed, cur_dir);
	}

	int span = (int) (Timer::time_elapsed() * _speed);

	remainder += Timer::time_elapsed() * _speed
			- (int) (Timer::time_elapsed() * _speed);
	span += (int) (remainder);
	remainder -= (int) (remainder);

	for (int i = 0; i < span; i++) {
		switch (_dir) {
		case DIR_LEFT:
			if (!move_left()) {
				stop();
				return false;
			}
			break;
		case DIR_RIGHT:
			if (!move_right()) {
				stop();
				return false;
			}
			break;
		case DIR_UP:
			if (!move_up()) {
				stop();
				return false;
			}
			break;
		case DIR_DOWN:
			if (!move_down()) {
				stop();
				return false;
			}
			break;
		default:
			break;
		}
	}
	return true;
}

void GameObject::fall() {
	if (!falling) {
		falling = true;
		z = Z_FALLING;
		fall_countdown = 1.0f;
		if (get_type() != CORPSE_PART && get_type() != EXTRA) {
			Resources::Game_deepfall()->play();
		}
	}
}

void GameObject::continue_falling() {
	speed = (int) (fall_countdown * 60);
	fall_countdown -= Timer::time_elapsed();
	if (fall_countdown < 0) {
		fallen_down = true;
		fall_countdown = 0;
	}
	opacity_scaled = (Uint8) (fall_countdown * 255);
}

void GameObject::stop(bool by_arrow) {
	stopped = true;
}

void GameObject::snap() {
	x = ((get_x() + 20) / 40) * 40;
	y = ((get_y() + 20) / 40) * 40;
}

void GameObject::set_dir(Direction _dir) {
	cur_dir = _dir;
}

void GameObject::set_pos(int _x, int _y) {
	x = _x;
	y = _y;
}

void GameObject::set_orig(int _x, int _y) {
	orig_x = _x;
	orig_y = _y;
}

void GameObject::move_pos(int _x, int _y) {
	x += _x;
	y += _y;
}

int GameObject::get_x() const {
	return (int) x;
}

int GameObject::get_y() const {
	return (int) y;
}

int GameObject::get_z() const {
	return z;
}

int GameObject::get_speed() const {
	return speed;
}

int GameObject::get_map_x() const {
	int tmp = (get_x() + 20) / 40;
	if (tmp < 0) {
		tmp = 0;
	} else if (tmp >= MAP_WIDTH) {
		tmp = MAP_WIDTH - 1;
	}
	return tmp;
}

int GameObject::get_map_y() const {
	int tmp = (get_y() + 20) / 40;
	if (tmp < 0) {
		tmp = 0;
	} else if (tmp >= MAP_HEIGHT) {
		tmp = MAP_HEIGHT - 1;
	}
	return tmp;
}

void GameObject::inc_speed(int _c) {
	speed += _c;
}

void GameObject::dec_speed(int _c) {
	speed -= _c;
}

void GameObject::set_speed(int _speed) {
	speed = _speed;
}

bool GameObject::is_flying() const {
	return flying;
}

bool GameObject::is_stopped() const {
	return stopped;
}

Direction GameObject::get_cur_dir() const {
	return cur_dir;
}

int GameObject::whats_left() {
	return app->map->get_maptile_xy((int) (x - 1), (int) (y + 20))->get_type();
}

int GameObject::whats_right() {
	return app->map->get_maptile_xy((int) (x + 40), (int) (y + 20))->get_type();
}

int GameObject::whats_up() {
	return app->map->get_maptile_xy((int) (x + 20), (int) (y - 1))->get_type();
}

int GameObject::whats_down() {
	return app->map->get_maptile_xy((int) (x + 20), (int) (y + 40))->get_type();
}

MapTile* GameObject::get_maptile() const {
	return app->map->get_maptile_xy((int) (x + 20), (int) (y + 20));
}

void GameObject::show() {
	if (falling) {
		show((int) (x + 60), (int) (y + 40), fall_countdown);
	} else if (flying) {
		float scalefactor = sinf(fly_progress * 3.14f) * (1.3f / fly_speed) + 1;
		surface->put_screen((int) (x + offset_x + 20 - scalefactor * 20),
				(int) (y + offset_y + 40 - scalefactor * 40), scalefactor,
				scalefactor, sprite_nr, opacity_scaled);
	} else {
		surface->put_screen((int) (x + offset_x), (int) (y + offset_y),
				sprite_nr, opacity);
	}
}

void GameObject::show(int _x, int _y) {
	surface->put_screen(_x, _y - 40 + offset_y, sprite_nr, opacity);
}

void GameObject::show(int _x, int _y, float _scale) {
	surface->put_screen((int) (20 + _x - 20 * _scale),
			(int) (_y + 20 + ((-40 + offset_y) * _scale) - 20 * _scale), _scale,
			_scale, sprite_nr, opacity_scaled);
}

bool GameObject::is_falling() {
	return (falling && !fallen_down);
}

void GameObject::output_object_info() {
	std::cout << " type=" << objecttype2string(get_type()) << " id="
			<< object_id << " x=" << x << " y=" << y << " z=" << z << " del_me="
			<< (int) delete_me << " is_flying=" << (int) flying
			<< " is_falling=" << (int) falling << " to_x=" << fly_dest_x
			<< " to_y=" << fly_dest_y << " progress=" << fly_progress
			<< " dist_x=" << fly_dist_x << " dist_y=" << fly_dist_y << " speed="
			<< fly_speed << std::endl;
}

void GameObject::set_next_fly_job(int flyjobx, int flyjoby, int flyjobspeed) {
	next_fly_job[0] = flyjobx;
	next_fly_job[1] = flyjoby;
	next_fly_job[2] = flyjobspeed;
}

void GameObject::reset_next_fly_job() {
	next_fly_job[0] = 0;
	next_fly_job[1] = 0;
	next_fly_job[2] = 0;
}

bool GameObject::is_next_fly_job() {
	return (next_fly_job[0] != 0 || next_fly_job[1] != 0 || next_fly_job[2] != 0);
}
