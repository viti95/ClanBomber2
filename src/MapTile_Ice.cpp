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
#include "MapTile_Ice.h"
#include "GameObject.h"

MapTile_Ice::MapTile_Ice(int _x, int _y, ClanBomberApplication* _app) : MapTile_Ground(_x, _y, _app)
{
}

MapTile_Ice::~MapTile_Ice()
{
}

void MapTile_Ice::act()
{
    MapTile::act();

    for(auto object_iter : objects)
    {
        (object_iter)->move(80,DIR_NONE);
    }
}



void MapTile_Ice::draw(int addx, int addy)
{
    MapTile::draw (addx, addy);

    if (!vanishing)
    {
        Resources::Game_maptile_addons()->put_screen( x+60+addx, y+40+addy,0);
    }
}

void MapTile_Ice::draw_tiny( int x, int y, float factor )
{
    Uint8 opacity = 255;

    if (vanishing)
    {
        opacity = (Uint8)(vanish_countdown * 255);
    }

    MapTile::draw_tiny(  x,  y,  factor );
    Resources::Game_maptile_addons()->put_screen( x, y, factor, factor, 0, opacity );
}

bool MapTile_Ice::get_addon_rect( SDL_Rect *ret_rect )
{
    Resources::Game_maptile_addons()->get_rect( 0, ret_rect );

    return true;
}

