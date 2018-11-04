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

#include "Bomber.h"

#include "Bomber_Corpse.h"
#include "Bomb.h"
#include "GameConfig.h"
#include "Timer.h"
#include "Controller.h"
#include "Disease.h"
#include "Map.h"
#include "MapTile.h"
#include "Observer.h"

#include "Extra_Bomb.h"
#include "Extra_Skateboard.h"
#include "Extra_Power.h"
#include "Extra_Kick.h"
#include "Extra_Glove.h"
#include "Utils.h"

#include <math.h>

#include <sys/time.h>

#include <boost/math/constants/constants.hpp>

Bomber::Bomber( int _x, int _y, COLOR _color, Controller* _controller, std::string _name, int _team, int _number, ClanBomberApplication *_app) : GameObject( _x, _y, _app )
{
    name = _name;
    team = _team;
    number = _number;

    surface = Resources::Bombers(_color);

    color = _color;
    points = 0;
    kills = 0;
    deaths = 0;
    disease = NULL;

    switch (color)
    {
    case 0 :
    case 1 :
    case 3 :
        offset_x = 60;
        offset_y = 11;
        break;
    case 2 :
    case 4 :
    case 5 :
    case 6 :
    case 7 :
        offset_x = 60;
        offset_y = 40;
    }
    controller = _controller;
    if (controller != NULL)
    {
        controller->attach(this);
        controller->reset();
        controller->deactivate();
    }
    reset();
        app->bomber_objects.push_back(this);
}

void Bomber::reset()
{
    opacity_scaled = 255;

    if (orig_x > 300)
    {
        cur_dir = DIR_LEFT;
    }
    else
    {
        cur_dir = DIR_RIGHT;
    }
    sprite_nr = 0;

    can_pass_bomber = true;
    can_throw = true;

    if (disease)
    {
        disease->stop();
        delete disease;
        disease = NULL;
    }
    dead = false;
    falling = false;
    fallen_down = false;
    flying = false;
    extra_gloves = 0;
    extra_skateboards =0;
    extra_power = 0;
    extra_kick = 0;
    extra_bombs = 0;

    delete_me = false;
    local_dir = cur_dir;
    allow_putbomb_timer.reset();

    skateboards = Config::get_start_skateboards();
    speed = 90 + (skateboards*40);

    can_kick = Config::get_start_kick();
    cur_bombs = Config::get_start_bombs();
    bombs = Config::get_start_bombs();
    power = Config::get_start_power();
    gloves = Config::get_start_glove();

    x = orig_x;
    y = orig_y;
    anim_count = 0;
}

int Bomber::get_team() const
{
    return team;
}

int Bomber::get_number() const
{
    return number;
}

void Bomber::set_team(int _team)
{
    team = _team;
}

std::string Bomber::get_name() const
{
    return name;
}

bool Bomber::is_diseased()
{
    return (disease != NULL);
}

Disease* Bomber::get_disease()
{
    return disease;
}

Bomber::~Bomber()
{
    if (controller)
    {
        delete controller;
    }
}

void Bomber::show()
{
    if (disease && !dead && !falling)
    {
        float scale = sin(fmod(Timer::get_time() / 50.0,2.0*boost::math::constants::pi<double>()))  * 0.15f + 1.0f;
        GameObject::show((int)x+60,(int)y+40,scale);
    }
    else if (!dead && cur_dir != DIR_NONE)
    {
        GameObject::show();
    }
}

int Bomber::get_skateboards()
{
    return skateboards;
}

void Bomber::set_skateboards(int nr_skates)
{
    skateboards = nr_skates;
}

bool Bomber::allow_putbomb()
{
    if (allow_putbomb_timer.elapsed() > 177)
    {
        allow_putbomb_timer.reset();
        return true;
    }
    return false;
}

void Bomber::act()
{
    if (dead)
    {
        return;
    }
    controller->update();
    if (disease)
    {
        infect_others();
        disease->act();
        if (disease->get_countdown() == -1)
        {
            delete disease;
            disease = NULL;
        }
    }

    GameObject::act();
    bool moved = false;

    if (fallen_down)
    {
        die();
        return;
    }

    if (controller->is_left())
    {
        moved = true;
        cur_dir = DIR_LEFT;
    }
    else if (controller->is_up())
    {
        moved = true;
        cur_dir = DIR_UP;
    }
    else if (controller->is_right())
    {
        moved = true;
        cur_dir = DIR_RIGHT;
    }
    else if (controller->is_down())
    {
        moved = true;
        cur_dir = DIR_DOWN;
    }
    else
    {
        anim_count = 0;
    }
    if (moved)
    {
        anim_count += Timer::time_elapsed()*20*(speed/90);
        move();
    }

    if (anim_count >= 9)
    {
        anim_count = 1;
    }

    sprite_nr = cur_dir*10 + (int)anim_count;

    if ( controller->is_bomb() )
    {
        if (get_maptile()->bomb)
        {
            if (gloves && !moved)
            {
                get_maptile()->bomb->throww(cur_dir);
                anim_count = 2;
            }
        }
        else
        {
            put_bomb();
        }
    }

    if (!falling)
    {
        z = Z_BOMBER + get_y();
    }
}

void Bomber::infect_others()
{

    std::list<GameObject*>::iterator object_iter;

    for (object_iter = get_maptile()->objects.begin();
            object_iter != get_maptile()->objects.end();
            object_iter++)
    {
        if ((*object_iter)->get_type() == GameObject::BOMBER && !(static_cast<Bomber*>(*object_iter)->is_diseased()))
        {
            static_cast<Bomber*>(*object_iter)->infect(disease->spawn(static_cast<Bomber*>(*object_iter)));
        }
    }
}

void Bomber::put_bomb()
{
    if (cur_bombs && !falling)
    {
        if (app->map->add_bomb( (int)((x+20)/40), (int)((y+20)/40), power, this, (int)x, (int)y ))
        {
            Resources::Game_putbomb()->play();
        }
    }
}

void Bomber::put_bomb(int mapx, int mapy)
{
    if (cur_bombs && !falling)
    {
        if (app->map->add_bomb( mapx, mapy, power, this ))
        {
            Resources::Game_putbomb()->play();
        }
    }
}

void Bomber::gain_extra_skateboard()
{
        if (Config::get_max_skateboards() > skateboards)
        {
            skateboards++;
            extra_skateboards++;
            speed += 40;
        }
}


void Bomber::gain_extra_bomb()
{
        if (Config::get_max_bombs() > bombs)
        {
            bombs++;
            extra_bombs++;
            cur_bombs++;
        }
}

void Bomber::gain_extra_glove()
{
        gloves++;
        extra_gloves++;
}

Bomber::COLOR Bomber::get_color() const
{
    return color;
}

void Bomber::gain_extra_power()
{
        if (Config::get_max_power() > power)
        {
            power++;
            extra_power++;
        }
}

void Bomber::loose_all_extras()
{
        int i;
        Extra* extra;

        for (i=0; i<extra_bombs; i++)
        {
            extra = new Extra_Bomb((int)x,(int)y,app);
            extra->fly_to(app->map->get_passable());
        }
        extra_bombs = 0;

        for (i=0; i<extra_power; i++)
        {
            extra = new Extra_Power((int)x,(int)y,app);
            extra->fly_to(app->map->get_passable());
        }
        extra_power = 0;

        for (i=0; i<extra_skateboards; i++)
        {
            extra = new Extra_Skateboard((int)x,(int)y,app);
            extra->fly_to(app->map->get_passable());
        }
        extra_skateboards = 0;

        for (i=0; i<extra_kick; i++)
        {
            extra = new Extra_Kick((int)x,(int)y,app);
            extra->fly_to(app->map->get_passable());
        }
        extra_kick = 0;

        for (i=0; i<extra_gloves; i++)
        {
            extra = new Extra_Glove((int)x,(int)y,app);
            extra->fly_to(app->map->get_passable());
        }
        extra_gloves = 0;
}

void Bomber::delete_disease()
{
    if (disease)
    {
        disease->stop();
        delete disease;
        disease = NULL;
    }
}

void Bomber::loose_disease()
{
        Extra* extra;
        if (disease)
        {

            disease->stop();
            extra = disease->spawn_extra((int)x,(int)y);
            extra->fly_to(app->map->get_passable());

            delete disease;
            disease = NULL;
        }
}

void Bomber::set_disease(Disease* _disease, bool play_sound)
{
    delete_disease();
    disease = _disease;
    if (play_sound)
    {
        if (disease->get_DiseaseType()==Disease::FAST)
        {
            Resources::Extras_schnief()->play();
        }
        else if (disease->get_DiseaseType()==Disease::STONED)
        {
            Resources::Extras_joint()->play();
        }
        else if (disease->get_DiseaseType()==Disease::PUTBOMB)
        {
            Resources::Extras_horny()->play();
        }
    }
}

void Bomber::infect(Disease* _disease)
{
        if (disease)
        {
            disease->stop();
            delete disease;
        }
        disease = _disease;
}


void Bomber::gain_extra_kick()
{
        can_kick = true;
        extra_kick++;
}

void Bomber::inc_cur_bombs()
{
        cur_bombs++;
}

void Bomber::dec_cur_bombs()
{
        if (cur_bombs)
        {
            cur_bombs--;
        }
}

GameObject::ObjectType Bomber::get_type() const
{
    return BOMBER;
}

bool Bomber::die()
{
    if (dead || is_falling())
    {
        return false;
    }
    dead = true;

        deaths++;

        Resources::Game_die()->play();
        if (!fallen_down)
        {
            loose_all_extras();
            if (!Config::get_kids_mode())
            {
                Bomber_Corpse* tmp = new Bomber_Corpse((int)x, (int)y, color, cur_dir*10+9, app);
            }
        }
        x = 0;
        y = 0;
        sprite_nr = cur_dir*10+9;

    return true;
}

void Bomber::inc_kills()
{
        kills++;
}

int Bomber::get_cur_bombs() const
{
    return cur_bombs;
}

int Bomber::get_power() const
{
    return power;
}

int Bomber::get_bombs() const
{
    return bombs;
}

int Bomber::get_kills() const
{
    return kills;
}

int Bomber::get_points() const
{
    return points;
}

void Bomber::set_points( int _points )
{
    points = _points;
}

void Bomber::inc_points()
{
        points++;
}

void Bomber::dec_points()
{
        points--;
}

int Bomber::get_deaths() const
{
    return deaths;
}

int Bomber::get_gloves()
{
    return gloves;
}

int Bomber::get_extra_gloves()
{
    return extra_gloves;
}

int Bomber::get_extra_skateboards()
{
    return extra_skateboards;
}

int Bomber::get_extra_power()
{
    return extra_power;
}

int Bomber::get_extra_kick()
{
    return extra_kick;
}

int Bomber::get_extra_bombs()
{
    return extra_bombs;
}

void Bomber::set_deaths(int d)
{
    deaths = d;
}

void Bomber::set_kills(int k)
{
    kills = k;
}

void Bomber::set_cur_bombs(int cb)
{
    cur_bombs = cb;
}

void Bomber::set_bombs(int b)
{
    bombs = b;
}

void Bomber::set_extra_bombs(int eb)
{
    extra_bombs = eb;
}

void Bomber::set_power(int p)
{
    power = p;
}

void Bomber::set_extra_power(int ep)
{
    extra_power = ep;
}

void Bomber::set_extra_skateboards(int es)
{
    extra_skateboards = es;
}

void Bomber::set_is_able_to_kick(bool k)
{
    can_kick = k;
}

void Bomber::set_extra_kick(int ek)
{
    extra_kick = ek;
}

void Bomber::set_is_able_to_throw(bool t)
{
    can_throw = t;
}

void Bomber::set_gloves(int g)
{
    gloves = g;
}

void Bomber::set_extra_gloves(int eg)
{
    extra_gloves = eg;
}

void Bomber::set_dead()
{
    dead = true;
}

void Bomber::set_sprite_nr(int snr)
{
    sprite_nr = snr;
}

void Bomber::set_anim_count(float animcnt)
{
    anim_count = animcnt;
}

SimpleTimer::SimpleTimer()
{
    start_time=new (struct timeval);
    gettimeofday(start_time, NULL);
    current_time=new (struct timeval);
    gettimeofday(current_time, NULL);
}

SimpleTimer::~SimpleTimer()
{
    delete start_time;
    delete current_time;
}

void SimpleTimer::reset()
{
    gettimeofday(start_time, NULL);
    gettimeofday(current_time, NULL);
}

float SimpleTimer::elapsed()
{
    gettimeofday(current_time, NULL);
    int time_start=start_time->tv_sec*1000000+start_time->tv_usec;
    int time_now=current_time->tv_sec*1000000+current_time->tv_usec;
    return (time_now-time_start)/1000.0f;
}

float SimpleTimer::elapsedn()
{
    gettimeofday(current_time, NULL);
    int time_start=start_time->tv_sec*1000000+start_time->tv_usec;
    int time_now=current_time->tv_sec*1000000+current_time->tv_usec;
    return (time_now-time_start);
}
