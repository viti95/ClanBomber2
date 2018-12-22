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
#include "Corpse_Part.h"

#include "Timer.h"

Corpse_Part::Corpse_Part(int _x, int _y, ClanBomberApplication* _app) : GameObject(_x, _y, _app)
{
    speed = 800;
    surface = Resources::Corpse_part();
    sprite_nr = fast_random()%4;
    z = Z_CORPSE_PART;
    countdown = 2.0f;
    can_fly_over_walls = false;
    app->objects.push_back(this);
}

Corpse_Part::~Corpse_Part()
{
}

void Corpse_Part::act()
{
    if (!flying)
    {
        countdown -= Timer::time_elapsed();
        if (countdown < 0)
        {
            countdown = 0;
            delete_me = true;
        }
        opacity = (Uint8) std::min( 255, (int)(255 * countdown) );
    }

    GameObject::act();
}
