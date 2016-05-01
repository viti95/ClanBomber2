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

#ifndef Explosion_h
#define Explosion_h

#include "GameObject.h"

class ClanBomberApplication;
class Bomber;

class Explosion : public GameObject
{
public:
    Explosion( int _x, int _y, int _power, Bomber *_bomber, ClanBomberApplication *_app );
    ~Explosion();
    virtual void show();
    virtual void act();

    const char* get_name()
    {
        return "Explosion";
    }

    ObjectType get_type() const
    {
        return EXPLOSION;
    }

    enum
    {
        EXPLODE_LEFT	= 0,
        EXPLODE_H	= 1,
        EXPLODE_RIGHT	= 2,
        EXPLODE_UP	= 3,
        EXPLODE_V	= 4,
        EXPLODE_DOWN	= 5,
        EXPLODE_X	= 6,
    } ;

    int get_power() const;
    Bomber* get_bomber();
    int get_length_left();
    int get_length_right();
    int get_length_up();
    int get_length_down();
    void set_lengths(int leftlen, int rightlen, int uplen, int downlen);
protected:
    void do_destruction();
    void detonate_other_bombs();
    void kill_bombers();
    void destroy_extras();
    void find_lengths();
    void splatter_corpses();

    void init();

    float detonation_period;

    int Y;
    int X;
    int power;
    int l_left;
    int l_right;
    int l_up;
    int l_down;
    Bomber *bomber;
};

#endif

