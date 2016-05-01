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
#include "Explosion.h"

#include "Extra.h"
#include "Timer.h"
#include "Map.h"
#include "MapTile.h"
#include "Bomb.h"
#include "Bomber.h"
#include "Bomber_Corpse.h"
#include "Utils.h"

Explosion::Explosion( int _x, int _y, int _power, Bomber *_bomber, ClanBomberApplication *_app ) : GameObject( _x, _y, _app )
{
    X = (_x+20) / 40;
    Y = (_y+20) / 40;
    power = _power;
    bomber = _bomber;

    init();
    find_lengths();
    do_destruction();
    splatter_corpses();
}

Explosion::~Explosion()
{
}

void Explosion::init()
{
    z = Z_EXPLOSION;
    detonation_period = 0.5f;

    Resources::Game_explode()->play();

        app->objects.push_back( this );
}

void Explosion::find_lengths()
{
        int i;
        for (i=1; i<=power; i++)
        {
            if ((app->map->get_maptile(X-i,Y)->blocking) || app->map->get_maptile(X-i,Y)->has_extra())
            {
                break;
            }
        }
        l_left = i-1;
        for (i=1; i<=power; i++)
        {
            if ((app->map->get_maptile(X+i,Y)->blocking) || app->map->get_maptile(X+i,Y)->has_extra())
            {
                break;
            }
        }
        l_right = i-1;
        for (i=1; i<=power; i++)
        {
            if ((app->map->get_maptile(X,Y-i)->blocking) || app->map->get_maptile(X,Y-i)->has_extra())
            {
                break;
            }
        }
        l_up = i-1;
        for (i=1; i<=power; i++)
        {
            if ((app->map->get_maptile(X,Y+i)->blocking) || app->map->get_maptile(X,Y+i)->has_extra())
            {
                break;
            }
        }
        l_down = i-1;
}

void Explosion::show()
{
    int anim = 14;
    if (detonation_period < 0.4f)
    {
        anim = 7;
    }
    if (detonation_period < 0.3f)
    {
        anim = 0;
    }
    if (detonation_period < 0.2f)
    {
        anim = 7;
    }
    if (detonation_period < 0.1f)
    {
        anim = 14;
    }

    Resources::Game_explosion()->put_screen( (int)(x+60), (int)(y+40), EXPLODE_X+anim );
    int i;
    for (i=1; i<=l_left; i++)
    {
        if (i == power)
        {
            Resources::Game_explosion()->put_screen( (int)(x-i*40+60), (int)(y+40), EXPLODE_LEFT+anim );
        }
        else
        {
            Resources::Game_explosion()->put_screen( (int)(x-i*40+60), (int)(y+40), EXPLODE_H+anim );
        }
    }
    for (i=1; i<=l_right; i++)
    {
        if (i == power)
        {
            Resources::Game_explosion()->put_screen( (int)(x+i*40+60), (int)(y+40), EXPLODE_RIGHT+anim );
        }
        else
        {
            Resources::Game_explosion()->put_screen( (int)(x+i*40+60), (int)(y+40), EXPLODE_H+anim );
        }
    }
    for (i=1; i<=l_up; i++)
    {
        if (i == power)
        {
            Resources::Game_explosion()->put_screen( (int)(x+60), (int)(y-i*40+40), EXPLODE_UP+anim );
        }
        else
        {
            Resources::Game_explosion()->put_screen( (int)(x+60), (int)(y-i*40+40), EXPLODE_V+anim );
        }
    }
    for (i=1; i<=l_down; i++)
    {
        if (i == power)
        {
            Resources::Game_explosion()->put_screen( (int)(x+60), (int)(y+i*40+40), EXPLODE_DOWN+anim );
        }
        else
        {
            Resources::Game_explosion()->put_screen( (int)(x+60), (int)(y+i*40+40), EXPLODE_V+anim );
        }
    }
}

void Explosion::kill_bombers()
{
        for(auto bomber_object_iter : app->bomber_objects)
        {
            int bx = ((bomber_object_iter)->get_x()+20)/40;
            int by = ((bomber_object_iter)->get_y()+20)/40;
            if ((by==Y && bx>=X-l_left && bx<=X+l_right)
                    || (bx==X && by>=Y-l_up && by<=Y+l_down))
            {
                if ((bomber_object_iter)->die()
                        && (bomber_object_iter) != bomber)
                {
                    bomber->inc_kills();
                }
            }
        }
}

void Explosion::destroy_extras()
{
        for(auto object_iter : app->objects)
        {
            if ((object_iter)->get_type() == GameObject::EXTRA)
            {
                int bx = ((object_iter)->get_x()+20)/40;
                int by = ((object_iter)->get_y()+20)/40;

                if ((by==Y && bx>=X-l_left && bx<=X+l_right)
                        || (bx==X && by>=Y-l_up && by<=Y+l_down))
                {
                    if (!((object_iter)->is_flying()))
                    {
                        static_cast<Extra*>(object_iter)->destroy();
                    }
                }
            }
        }
}

void Explosion::splatter_corpses()
{
        for(auto object_iter : app->objects)
        {
            if ((object_iter)->get_type() == GameObject::BOMBER_CORPSE)
            {
                int bx = ((object_iter)->get_x()+20)/40;
                int by = ((object_iter)->get_y()+20)/40;

                if ((by==Y && bx>=X-l_left && bx<=X+l_right)
                        || (bx==X && by>=Y-l_up && by<=Y+l_down))
                {
                    static_cast<Bomber_Corpse*>(object_iter)->explode();
                }
            }
        }
}

int Explosion::get_power() const
{
    return power;
}

void Explosion::act()
{
    detonate_other_bombs();
    kill_bombers();
    destroy_extras();

    detonation_period -= Timer::time_elapsed();
    if (detonation_period < 0)
    {
            delete_me = true;
    }
}

void Explosion::do_destruction()
{
        app->map->get_maptile(X,Y)->destroy();

        if (l_left < power)
        {
            app->map->get_maptile(X-l_left-1,Y)->destroy();
        }
        if (l_right < power)
        {
            app->map->get_maptile(X+l_right+1,Y)->destroy();
        }
        if (l_up < power)
        {
            app->map->get_maptile(X,Y-l_up-1)->destroy();
        }
        if (l_down < power)
        {
            app->map->get_maptile(X,Y+l_down+1)->destroy();
        }
}

void Explosion::detonate_other_bombs()
{
        int i;
        for (i=0; i<=l_left; i++)
        {
            if (app->map->get_maptile(X-i,Y)->bomb)
            {
                app->map->get_maptile(X-i,Y)->bomb->explode_delayed();
                break;
            }
        }
        for (i=1; i<=l_right; i++)
        {
            if (app->map->get_maptile(X+i,Y)->bomb)
            {
                app->map->get_maptile(X+i,Y)->bomb->explode_delayed();
                break;
            }
        }
        for (i=1; i<=l_up; i++)
        {
            if (app->map->get_maptile(X,Y-i)->bomb)
            {
                app->map->get_maptile(X,Y-i)->bomb->explode_delayed();
                break;
            }
        }
        for (i=1; i<=l_down; i++)
        {
            if (app->map->get_maptile(X,Y+i)->bomb)
            {
                app->map->get_maptile(X,Y+i)->bomb->explode_delayed();
                break;
            }
        }
}

Bomber* Explosion::get_bomber()
{
    return bomber;
}

int Explosion::get_length_left()
{
    return l_left;
}

int Explosion::get_length_right()
{
    return l_right;
}

int Explosion::get_length_up()
{
    return l_up;
}

int Explosion::get_length_down()
{
    return l_down;
}

void Explosion::set_lengths(int leftlen, int rightlen, int uplen, int downlen)
{
    l_left = leftlen;
    l_right = rightlen;
    l_up = uplen;
    l_down = downlen;
}
