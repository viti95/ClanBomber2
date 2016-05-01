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

#ifndef Bomber_Corpse_h
#define Bomber_Corpse_h

#include "GameObject.h"

class Bomber_Corpse: public GameObject {
public:
	Bomber_Corpse(int _x, int _y, int _color, int _sprite_nr,
			ClanBomberApplication *_app);
	void act();
	~Bomber_Corpse();

	void explode();

	ObjectType get_type() const {
		return BOMBER_CORPSE;
	}
protected:
	int color;
	int splash_counter;
	bool ready_to_splash;

	float explode_delay;

private:
	void init();
};

#endif

