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

#ifndef Bomb_h
#define Bomb_h

class Bomber;
class MapTile;

#include "GameObject.h"

class Bomb: public GameObject {
public:
	void explode_delayed();
	void explode();
	void kick(Direction dir, bool by_arrow = false);
	void throww(Direction dir, int fields = 4);
	void stop(bool by_arrow = false);
	int get_power();
	float get_countdown();
	void set_countdown(float _countdown);
	void set_stopped();
	Bomber* get_bomber() const {
		return bomber;
	}
	Bomb(int _x, int _y, int _power, Bomber *_bomber,
			ClanBomberApplication *_app);
	~Bomb();

	virtual void show();
	virtual void act();
	ObjectType get_type() const {
		return BOMB;
	}
	int get_reactivation_frame_counter();
	void set_reactivation_frame_counter(int counter);
protected:
	float cur_scale;
	bool exploding;
	int power;
	float detonation_countdown;
	Bomber *bomber;
	int reactivation_frame_counter;
private:
	void init();
};

#endif
