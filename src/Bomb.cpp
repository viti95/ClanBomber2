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
#include "Resources.h"

#include "Bomb.h"

#include "Explosion.h"
#include "Timer.h"
#include "Bomber.h"
#include "Map.h"
#include "MapTile.h"
#include "GameConfig.h"

Bomb::Bomb(int _x, int _y, int _power, Bomber *_bomber,
		ClanBomberApplication *_app) :
		GameObject(_x, _y, _app) {
	surface = Resources::Game_bombs();
	detonation_countdown = Config::get_bomb_countdown() / 10.0f;
	cur_scale = 0;
	cur_dir = DIR_NONE;
	exploding = false;
	speed = Config::get_bomb_speed();
	can_kick = false;
	can_fly_over_walls = false;
	bomber = _bomber;
	power = _power;
	sprite_nr = bomber->get_color() * 4;
	reactivation_frame_counter = -1;
	bomber->dec_cur_bombs();
	app->objects.push_back(static_cast<GameObject*>(this));
}

Bomb::~Bomb() {
	if (bomber != NULL) {
		bomber->inc_cur_bombs();
	}
	if (get_maptile() != NULL) {
		get_maptile()->bomb = NULL;
	}
}

void Bomb::stop(bool by_arrow) {
	GameObject::stop();
	if (!flying) {
		cur_dir = DIR_NONE;
		snap();
	}
}

void Bomb::show() {
	GameObject::show();
}

void Bomb::kick(Direction dir, bool by_arrow) {
	if (!flying) {
		cur_dir = dir;
	}
}

void Bomb::throww(Direction dir, int fields) {
	can_fly_over_walls = false;
	if (fields > 0 && !flying) {
		cur_dir = dir;
		switch (dir) {
		case DIR_UP:
			fly_to(get_x(), get_y() - fields * 40);
			break;
		case DIR_DOWN:
			fly_to(get_x(), get_y() + fields * 40);
			break;
		case DIR_LEFT:
			fly_to(get_x() - fields * 40, get_y());
			break;
		case DIR_RIGHT:
			fly_to(get_x() + fields * 40, get_y());
			break;
		default:
			break;
		}
	}
}

void Bomb::act() {
	GameObject::act();

	if (fallen_down) {
		delete_me = true;
		return;
	}

	if (flying) {
		z = Z_BOMB + Z_FLYING + get_y();
		return;
	}

	if (fly_progress == 1) {
		fly_progress = 0;
		can_fly_over_walls = false;

		if (get_maptile()->bomb) {
			throww(cur_dir, 1);
		} else {
			cur_dir = DIR_NONE;
		}
	} else {
		move();
		cur_scale += Timer::time_elapsed() * 10;
		sprite_nr = bomber->get_color() * 4 + (int) cur_scale % 4;
		detonation_countdown -= Timer::time_elapsed();
		if (detonation_countdown <= 0) {
			explode();
		}
		z = Z_BOMB + get_y();
	}
}

void Bomb::explode() {
	if (!exploding && !falling) {
		snap();
		exploding = true;
		Explosion* tmp = new Explosion((int) x, (int) y, power, bomber, app);
		app->map->shake((int) (0.25f / Timer::time_elapsed()));
		delete_me = true;
	}
}

void Bomb::explode_delayed() {
	if (detonation_countdown > Config::get_bomb_delay() / 100.0f) {
		detonation_countdown = Config::get_bomb_delay() / 100.0f;

		if (detonation_countdown == 0) {
			explode();
		}
	}
}

int Bomb::get_power() {
	return power;
}

float Bomb::get_countdown() {
	return detonation_countdown;
}

void Bomb::set_countdown(float _countdown) {
	detonation_countdown = _countdown;
}

void Bomb::set_stopped() {
	stopped = true;
}

int Bomb::get_reactivation_frame_counter() {
	return reactivation_frame_counter;
}

void Bomb::set_reactivation_frame_counter(int counter) {
	reactivation_frame_counter = counter;
}
