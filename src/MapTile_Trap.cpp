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
#include "MapTile_Trap.h"
#include "Map.h"
#include "GameObject.h"
#include "Bomb.h"
#include "Timer.h"

#include <algorithm>

MapTile_Trap::MapTile_Trap(int _x, int _y, ClanBomberApplication* _app) :
		MapTile_Ground(_x, _y, _app) {
}

MapTile_Trap::~MapTile_Trap() {
	for (const auto &bomb_iter : bombs) {
		delete (bomb_iter)->get_bomb();
		delete (bomb_iter);
	}
}

void MapTile_Trap::act() {
	MapTile::act();

	for (std::list<GameObject*>::iterator object_iter = objects.begin();
			object_iter != objects.end(); object_iter++) {
		if ((*object_iter)->get_type() == GameObject::BOMB) {
			std::list<GameObject*>::iterator iter;
			iter = find(app->objects.begin(), app->objects.end(), *object_iter);
			//TODO does this really not need an check
			app->objects.erase(iter);
			bomb = NULL;
			bombs.push_back(new Bomb_Entry(static_cast<Bomb*>(*object_iter)));
		}
	}

	for (std::list<Bomb_Entry*>::iterator bomb_iter = bombs.begin();
			bomb_iter != bombs.end(); ++bomb_iter) {
		if ((*bomb_iter)->ready_to_shoot()) {
			MapTile *tile = app->map->get_passable();
			if (tile) {
				app->objects.push_back((*bomb_iter)->get_bomb());

				(*bomb_iter)->get_bomb()->set_fly_over_walls(true);
				(*bomb_iter)->get_bomb()->set_countdown(0.01f);
				(*bomb_iter)->get_bomb()->set_speed(200);

				(*bomb_iter)->get_bomb()->fly_to(tile);
			} else {
				delete (*bomb_iter)->get_bomb();
			}

			delete *bomb_iter;
			bomb_iter = bombs.erase(bomb_iter);
		} else {
			(*bomb_iter)->count();
		}
	}
}

void MapTile_Trap::draw(int addx, int addy) {
	MapTile::draw(addx, addy);
	if (!vanishing) {
		Resources::Game_maptile_addons()->put_screen(x + 60 + addx,
				y + 40 + addy, 7);
	}
}

void MapTile_Trap::draw_tiny(int x, int y, float factor) {
	Uint8 opacity = 255;

	if (vanishing) {
		opacity = (Uint8)(vanish_countdown * 255);
	}

	MapTile::draw_tiny(x, y, factor);
	Resources::Game_maptile_addons()->put_screen(x, y, factor, factor, 7,
			opacity);
}

bool MapTile_Trap::get_addon_rect(SDL_Rect *ret_rect) {
	Resources::Game_maptile_addons()->get_rect(7, ret_rect);

	return true;
}

MapTile_Trap::Bomb_Entry::Bomb_Entry(Bomb *_bomb) {
	bomb = _bomb;
	countdown = 1;
}

void MapTile_Trap::Bomb_Entry::count() {
	countdown -= Timer::time_elapsed();
}

bool MapTile_Trap::Bomb_Entry::ready_to_shoot() {
	return (countdown <= 0);
}
