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

#ifndef MAPTILE_H
#define MAPTILE_H

class GameObject;
class Bomb;

/**
 *@author Andreas Hundt, Denis Oliver Kropp
 */

class MapTile {
	friend class Map;

public:
	MapTile(int _x, int _y, ClanBomberApplication* _app);
	virtual ~MapTile();

	typedef enum {
		NONE = -3, ARROW = -2, ICE = -1, GROUND = 0, WALL = 1, BOX = 2, TRAP = 3,
	} TYPE;

	virtual TYPE get_type() =0;

	static MapTile* create(TYPE type, int _x, int _y, Direction _dir,
			ClanBomberApplication* _app);
	static MapTile* create(TYPE type, int _x, int _y,
			ClanBomberApplication* _app);
	static MapTile* spawn(MapTile* old_tile);

	void set_pos(int _x, int _y);
	int get_x();
	int get_y();
	void set_sprite_nr(int sprite);
	int get_sprite_nr();
	void set_vanish_countdown(float cd);
	void set_destroy_countdown(float cd);
	void set_vanishing(bool v);
	void spawn_extra();

	virtual void act();
	virtual void draw(int addx = 0, int addy = 0);
	virtual void draw_tiny(int x, int y, float factor);
	virtual void draw_shaken();

	virtual bool get_addon_rect(SDL_Rect *ret_rect);

	/**
	 * Contains pointers to all GameObjects that are on this MapTile.
	 */
	std::list<GameObject*> objects;

    bool has_extra();
    bool has_bomber();

	bool vanishing;
	bool destroyed;
	bool passable;
	bool blocking;

	virtual void vanish();
	virtual void destroy();

	Bomb *bomb;

	bool delete_me;

protected:
	int x;
	int y;
	int sprite_nr;
	TYPE next;

	float vanish_countdown;
	float destroy_countdown;

	ClanBomberApplication* app;
};

#endif

