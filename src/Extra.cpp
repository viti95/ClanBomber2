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
#include "Extra.h"

#include "Timer.h"
#include "Bomber.h"
#include "Map.h"
#include "MapTile.h"

#include "Extra_Viagra.h"
#include "Extra_Joint.h"
#include "Extra_Koks.h"
#include "Extra_Glove.h"
#include "Extra_Kick.h"
#include "Extra_Skateboard.h"
#include "Extra_Bomb.h"
#include "Extra_Power.h"

Extra::Extra( int _x, int _y, ClanBomberApplication *_app ) : GameObject( _x, _y, _app )
{
    surface = Resources::Extras_extras();
    opacity_scaled = 0xBB;

    app->objects.push_back( this );
 
    destroyed = false;
    destroyable = true;
    z = Z_EXTRA;
}

Extra::~Extra()
{
}

Extra* Extra::create( ExtraType type, int _x, int _y, ClanBomberApplication* _app )
{
    switch (type)
    {
    case VIAGRA:
        return new Extra_Viagra(_x, _y, _app);
    case JOINT:
        return new Extra_Joint(_x, _y, _app);
    case KOKS:
        return new Extra_Koks(_x, _y, _app);
    case GLOVE:
        return new Extra_Glove(_x, _y, _app);
    case KICK:
        return new Extra_Kick(_x, _y, _app);
    case SKATEBOARD:
        return new Extra_Skateboard(_x, _y, _app);
    case BOMB:
        return new Extra_Bomb(_x, _y, _app);
    case POWER:
        return new Extra_Power(_x, _y, _app);
    }
    return NULL;
}

void Extra::act()
{
    GameObject::act();

    if (falling)
    {
        return;
    }
    if (fallen_down)
    {
        delete_me = true;
        return;
    }

    if (destroyed)
    {
        destroy_countdown -= Timer::time_elapsed();
        if (destroy_countdown <= 0)
        {
            delete_me = true;
            return;
        }
    }

    if (!flying)
    {
        for(const auto &game_object_iter : get_maptile()->objects)
        {
            if (game_object_iter->get_type() == BOMBER)
            {
                effect( static_cast<Bomber*>(game_object_iter) );
                delete_me = true;
            }
        }
    }
}

void Extra::destroy()
{
    if (!destroyed)
    {
        if (destroyable)
        {
            destroyed = true;
            destroy_countdown = 0.5f;
        }
        else
        {
            fly_to(app->map->get_passable());
        }
    }
}

bool Extra::is_destroyable() const
{
    return destroyable;
}
