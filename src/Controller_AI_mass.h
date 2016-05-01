/*
 * This file is part of ClanBomber;
 * you can get it at "http://www.nongnu.org/clanbomber".
 *
 * Copyright (C) 2003, 2004, 2007 mass
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

#ifndef CONTROLLER_AI_MASS_H
#define CONTROLLER_AI_MASS_H

#include "ClanBomber.h"
#include "Controller.h"
#include "Map.h"

#include <vector>

#include "UtilsCL_Vector.h"

class Bomb;
class Controller_AI_mass;
class Explosion;

enum AIAction
{
    AI_move,
    AI_escape,
    AI_panic,
    AI_wait,
    AI_get,
    AI_put,
    AI_throw
};

class AIPathItem
{
public:
    AIPathItem(const Controller_AI_mass* cai, CL_Vector* v, CL_Vector f, AIAction act);
    virtual ~AIPathItem();
    Direction follow() const;
    bool passing();//no
    bool passed();//no
    bool verify_action() const;
    bool put_bomb() const;
    bool get_extra() const;
    CL_Vector* get_map_v() const;
    void set_end();//no
private:
    bool verify_move() const;
    bool verify_escape() const;
    bool verify_panic() const;
    bool verify_wait() const;
    bool verify_get() const;
    bool verify_put() const;
    bool verify_throw() const;
private:
    bool was_passing;
    CL_Vector* map_v;
    Direction v_dir;
    const Controller_AI_mass* ai;
    AIAction action;
    int rating;
};

class AIPath
{
public:
    AIPath(Controller_AI_mass* cai, const std::vector<CL_Vector*> &way, AIAction act);
    virtual ~AIPath();
    Direction follow();//no
    bool passed() const;
    bool verify();//no
    bool continue_path();//no
private:
    bool verify_step();//no
    bool verify_actions();//const? ver_action
    bool continue_move(Direction dir) const;
private:
    std::vector<AIPathItem*> path;
    Controller_AI_mass* ai;
    Direction curr_dir;
    int curr_item;
};

//Public part checked for const
class Controller_AI_mass: public Controller
{
public:
    Controller_AI_mass();
    virtual ~Controller_AI_mass();
    void reset();
    void update();
    bool is_left();// const;
    bool is_right();// const;
    bool is_up();// const;
    bool is_down();// const;
    bool is_bomb();// const;
public:
    bool passable (CL_Vector* map_v, int min_rating) const;
    bool has_extra (CL_Vector* map_v) const;
    bool has_bomb (CL_Vector* map_v) const;
    int get_rating (CL_Vector* map_v) const;
    void set_put_bomb();
    int attack_rating (int map_x, int map_y) const;
private:
    void update_ratings();
    void update_ratings_passable();
    void update_ratings_explosion();
    void update_ratings_bomb (bool update);
    void update_ratings_extra (bool update);
    bool ground_changed();
    void clear_path();
    bool verify_path();
    void assign_path();
    void follow_path();
    std::vector<CL_Vector*>* seek_util();
    std::vector<CL_Vector*>* sure_position();
    bool place_bomb();
    std::vector<CL_Vector*>* bomb_path();
    Explosion* get_explosion (int map_x, int map_y) const;
    bool has_extra (int map_x, int map_y) const;
    bool has_disease (int map_x, int map_y) const;
    bool enemy_bomber (int map_x, int map_y) const;
    bool team_bomber (int map_x, int map_y) const;
    bool destroys_extras (int map_x, int map_y);
    void apply_explosion (Explosion* exp);
    void apply_bomb (int map_x, int map_y, int power, int mode);
    int extra_rating (int map_x, int map_y) const;
    std::vector<CL_Vector*>* path(CL_Vector start_v, CL_Vector dest_v, int min_path_rating, int min_end_rating);
private:
    Map* curr_map;
    Direction curr_dir;
    bool put_bomb;
    int ratings[MAP_WIDTH][MAP_HEIGHT];
    AIPath* curr_path;
    bool box_hits[MAP_WIDTH][MAP_HEIGHT];
    int explosions[MAP_WIDTH][MAP_HEIGHT];
};

#endif
