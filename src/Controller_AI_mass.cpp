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

#include "Controller_AI_mass.h"

#include "Bomb.h"
#include "Bomber.h"
#include "GameConfig.h"
#include "Explosion.h"
#include "Extra.h"
#include "MapTile.h"

#include <algorithm>
#include <assert.h>

#define random(num) (rand() % num)

#define RATING_EXTRA_GLOVE      300
#define RATING_EXTRA_KICK       450
#define RATING_EXTRA_BOMB       400
#define RATING_EXTRA_POWER      400
#define RATING_EXTRA_SKATEBOARD 550

#define RATING_DISEASE_JOINT  -200
#define RATING_DISEASE_KOKS   -300
#define RATING_DISEASE_VIAGRA -500

#define RATING_EXPLOSION  -1000000
#define RATING_UNPASSABLE  -900000
#define RATING_BLOCKING    -800000
#define RATING_VANISHING   -300000
#define RATING_BOMB         -90000
#define RATING_EXPLODING     -7000

#define RATING_ENEMY_BOMBER 42
#define RATING_BOMB_BOX     66
#define RATING_DISEASE      17

#define RATING_SURE   0
#define RATING_POS    (RATING_SURE + 1)
#define RATING_ESCAPE (RATING_EXPLODING * 4)
#define RATING_PANIC  RATING_VANISHING

enum
{
    MINIT,
    MADD,
    MDEL
};

AIPathItem::AIPathItem(const Controller_AI_mass* cai, CL_Vector* v, CL_Vector f, AIAction act)
{
    assert(v != NULL);
    map_v = v;
    ai = cai;
    was_passing = false;
    action = act;
    rating = ai->get_rating(map_v);

    if (*v == f)
    {
        v_dir = DIR_NONE;
    }
    else if ((int)v->x < (int)f.x)
    {
        v_dir = DIR_RIGHT;
    }
    else if ((int)v->x > (int)f.x)
    {
        v_dir = DIR_LEFT;
    }
    else if ((int)v->y < (int)f.y)
    {
        v_dir = DIR_DOWN;
    }
    else
    {
        v_dir = DIR_UP;
    }
}

AIPathItem::~AIPathItem()
{
    assert(map_v != NULL);
    delete map_v;
}

Direction AIPathItem::follow() const
{
    return v_dir;
}

bool AIPathItem::passing()
{
    bool pass = (ai->bomber->get_map_x() == (int)map_v->x &&
                 ai->bomber->get_map_y() == (int)map_v->y);

    if (pass)
    {
        was_passing = true;
    }

    return pass;
}

bool AIPathItem::passed()
{
    return (was_passing && !passing());
}

bool AIPathItem::verify_action() const
{
    if (ai->get_rating (map_v) != rating)
    {
        //printf ("new rating (%d) < old rating (%d)\n",
        //        ai->get_rating (map_v), rating);
        //return false;
    }

    bool verified = false;

    switch (action)
    {
    case AI_move:
    {
        verified = verify_move();
        //printf ("move (%d).\n", (int)verified);
        break;
    }
    case AI_escape:
    {
        verified = verify_escape();
        //printf ("escape (%d).\n", (int)verified);
        break;
    }
    case AI_panic:
    {
        verified = verify_panic();
        //printf ("panic (%d).\n", (int)verified);
        break;
    }
    case AI_wait:
    {
        verified = verify_wait();
        //printf ("wait (%d).\n", (int)verified);
        break;
    }
    case AI_get:
    {
        verified = verify_get();
        //printf ("get (%d).\n", (int)verified);
        break;
    }
    case AI_put:
    {
        verified = verify_put();
        //printf ("put (%d).\n", (int)verified);
        break;
    }
    case AI_throw:
    {
        verified = verify_throw();
        //printf ("throw (%d).\n", (int)verified);
        break;
    }
    default:
    {
        break;
    }
    }

    return verified;
}

bool AIPathItem::put_bomb() const
{
    return (action == AI_put);
}

bool AIPathItem::get_extra() const
{
    return (action == AI_get);
}

CL_Vector* AIPathItem::get_map_v() const
{
    return map_v;
}

void AIPathItem::set_end()
{
    v_dir = DIR_NONE;
}

bool AIPathItem::verify_move() const
{
    return ai->passable(map_v, RATING_SURE);
}

bool AIPathItem::verify_escape() const
{
    return ai->passable (map_v, RATING_ESCAPE);
}

bool AIPathItem::verify_panic() const
{
    return ai->passable (map_v, RATING_PANIC);
}

bool AIPathItem::verify_wait() const
{
    return false;
}

bool AIPathItem::verify_get() const
{
    return ai->has_extra(map_v);
}

bool AIPathItem::verify_put() const
{
    return (!ai->has_bomb (map_v) &&
            ai->get_rating (map_v) >= RATING_SURE &&
            ai->bomber->get_cur_bombs() > 0 &&
            ai->attack_rating ((int)map_v->x, (int)map_v->y) > 0);
}

bool AIPathItem::verify_throw() const
{
    return false;
}

AIPath::AIPath(Controller_AI_mass* cai, const std::vector<CL_Vector*> &way, AIAction act)
{
    ai = cai;
    curr_dir = DIR_NONE;
    curr_item = 0;

    if (way.size() < 2)
    {
        return;
    }

    AIAction first_act = AI_move;
    AIAction step_act = AI_move;
    AIAction last_act = AI_move;

    if (act == AI_panic)
    {
        first_act = AI_panic;
        step_act = AI_escape;
        last_act = AI_move;
    }
    else if (act == AI_escape)
    {
        first_act = AI_panic;
        step_act = AI_escape;
        last_act = AI_move;
    }
    else
    {
        first_act = AI_panic;
        step_act = AI_move;
        last_act = act;
    }

    for (int i = 0; i < way.size(); i++)
    {
        if (i == 0)
        {
            assert(i+1 < way.size());
            path.push_back(new AIPathItem(cai, way[i], *way[i+1], first_act));
        }
        else
        {
            if (i == (way.size() - 1))
            {
                path.push_back (new AIPathItem (cai, way[i], *(way[i]), last_act));
            }
            else
            {
                assert(i+1 < way.size());
                path.push_back (new AIPathItem (cai, way[i], *(way[i+1]), step_act));
            }
        }
    }
}

AIPath::~AIPath()
{
    //todo free resources
    /*if (path != NULL)
      {
        while (path->get_num_items() > 0)
          {
            AIPathItem *pi = path->get_first();
            path->del (pi);
            delete pi;
          }

        delete path;
        path = NULL;
        }*/
    for(std::vector<AIPathItem*>::iterator iter = path.begin();
            iter != path.end(); ++iter)
    {
        delete *iter;
    }
}

bool AIPath::passed() const
{
    /*if (path == NULL)
      {
        return true;
        }*/

    assert(curr_item >= 0);
    return ((curr_item + 1) >= path.size());
}

bool AIPath::continue_move(Direction dir) const
{
    int off_x = (ai->bomber->get_x() + 20) % 40;
    int off_y = (ai->bomber->get_y() + 20) % 40;

    if (dir == DIR_LEFT)
    {
        if (off_x > 30)
        {
            return true;
        }
    }
    else if (dir == DIR_RIGHT)
    {
        if (off_x < 10)
        {
            return true;
        }
    }
    else if (dir == DIR_UP)
    {
        if (off_y > 10)
        {
            return true;
        }
    }
    else if (dir == DIR_DOWN)
    {
        if (off_y < 30)
        {
            return true;
        }
    }

    return false;
}

Direction AIPath::follow()
{
    curr_dir = DIR_NONE;

    AIPathItem *pi = path[curr_item];

    if (pi->passing())
    {
        curr_dir = pi->follow();
    }
    else
    {
        Direction fdir = pi->follow();
        if (continue_move (fdir))
        {
            curr_dir = fdir;
        }
        else
        {
            curr_item++;
            if (curr_item < path.size())
            {
                pi = path[curr_item];
                curr_dir = pi->follow();
            }
        }
    }

    return curr_dir;
}

bool AIPath::verify()
{
    if (verify_step())
    {
        //printf ("step verified!\n");
        if (verify_actions())
        {
            //printf ("actions verified!\n");
            return true;
        }
    }

    //printf ("not verified.\n");

    return false;
}

bool AIPath::verify_step()
{
    //if (path == NULL)
    if(path.size() == 0)
    {
        return false;
    }

    AIPathItem* pi = path[curr_item];

    bool ver = pi->passing();

    if (!ver && pi->passed() && (curr_item + 1) < path.size())
    {
        curr_item++;
        pi = path[curr_item];
        ver = pi->passing();
    }

    if (pi->put_bomb() && ver)
    {
        //printf("--- put_bomb ---\n");
        ai->set_put_bomb();
    }

    return ver;
}

bool AIPath::verify_actions()
{
    int verified = true;

    for (int i = curr_item; i < path.size(); i++)
    {
        AIPathItem* pi = path[i];
        if (!pi->verify_action())
        {
            verified = false;
            break;
        }
    }

    return verified;
}

bool AIPath::continue_path()
{
    bool verified = true;
    AIPathItem* pi = NULL;

    int i = 0;
    for (i = curr_item; i < path.size(); i++)
    {
        pi = path[i];
        if (!pi->verify_action())
        {
            verified = false;
            break;
        }
    }

    if (!verified)
    {
        if (pi != path.back() &&
                (path.back()->put_bomb() || path.back()->get_extra()) &&
                path.size() - i > 1)
        {
            for (int j = i; j < path.size(); j++)
            {
                path.erase(path.rbegin().base());
            }

            path.back()->set_end();

            verified = true;
        }
    }

    return verified;
}

Controller_AI_mass::Controller_AI_mass() : Controller()
{
    c_type = AI_mass;
    curr_path = NULL;
    curr_map = NULL;
    curr_dir = DIR_NONE;
    put_bomb = false;

    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            ratings[map_x][map_y] = RATING_SURE;
            box_hits[map_x][map_y] = 0;
            explosions[map_x][map_y] = 0;
        }
    }
}

Controller_AI_mass::~Controller_AI_mass()
{
}

void Controller_AI_mass::reset()
{
    curr_dir = DIR_NONE;
    put_bomb = false;
    curr_map = bomber->app->map;

    clear_path();

    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            ratings[map_x][map_y] = RATING_SURE;
            box_hits[map_x][map_y] = 0;
            explosions[map_x][map_y] = 0;
        }
    }
}

void Controller_AI_mass::update()
{
    if (!active)
    {
        return;
    }

    update_ratings();
    put_bomb = false;

    if (!verify_path())
    {
        if (curr_path != NULL)
        {
            if (!curr_path->continue_path())
            {
                clear_path();
                assign_path();
            }
        }
        else
        {
            clear_path();
            assign_path();
        }
    }
    else
    {
        if (false && ground_changed())
        {
            clear_path();
            assign_path();
        }
        else
        {
            follow_path();
        }
    }
}

bool Controller_AI_mass::is_left()// const
{
    return active && (curr_dir == DIR_LEFT);
}

bool Controller_AI_mass::is_right()// const
{
    return active && (curr_dir == DIR_RIGHT);
}

bool Controller_AI_mass::is_up()// const
{
    return active && (curr_dir == DIR_UP);
}

bool Controller_AI_mass::is_down()// const
{
    return active && (curr_dir == DIR_DOWN);
}

bool Controller_AI_mass::is_bomb()// const
{
    switch (bomb_mode)
    {
    case ALWAYS:
    {
        return true;
    }
    case NEVER:
    {
        return false;
    }
    default:
    {
        break;
    }
    }

    return active && put_bomb;
}

void Controller_AI_mass::update_ratings()
{
    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            ratings[map_x][map_y] = RATING_SURE;
            box_hits[map_x][map_y] = 0;

            MapTile* mt = curr_map->get_maptile (map_x, map_y);
            bool explosion = (get_explosion (map_x, map_y) != NULL);
            bool bomb = (mt->bomb != NULL);

            if (explosions[map_x][map_y] == 0 && bomb)
            {
                explosions[map_x][map_y] = 1;
            }
            else if (explosions[map_x][map_y] == 1)
            {
                if (bomb)
                {
                    explosions[map_x][map_y] = 2;
                }
                else
                {
                    explosions[map_x][map_y] = 0;
                }
            }
            else if (explosions[map_x][map_y] == 2)
            {
                if (!bomb)
                {
                    explosions[map_x][map_y] = 3;
                }
            }
            else if (explosions[map_x][map_y] == 3 && explosion)
            {
                explosions[map_x][map_y] = 4;
            }
            else if (explosions[map_x][map_y] == 4 && ! explosion)
            {
                explosions[map_x][map_y] = 5;
            }
            else if (explosions[map_x][map_y] == 5)
            {
                explosions[map_x][map_y] = 0;
            }
        }
    }

    update_ratings_extra (false);
    update_ratings_bomb (true);
    update_ratings_passable();
    update_ratings_explosion();
}

void Controller_AI_mass::update_ratings_passable()
{
    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            MapTile* mt = curr_map->get_maptile (map_x, map_y);

            if (mt->vanishing)
            {
                ratings[map_x][map_y] = RATING_VANISHING;
            }
            else if (mt->blocking)
            {
                ratings[map_x][map_y] += RATING_BLOCKING;
            }
            else if (!mt->passable)
            {
                ratings[map_x][map_y] += RATING_UNPASSABLE;
            }
        }
    }
}

void Controller_AI_mass::update_ratings_explosion()
{
    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            Explosion* exp = get_explosion (map_x, map_y);
            if (exp != NULL)
            {
                apply_explosion (exp);
            }
        }
    }
}

void Controller_AI_mass::update_ratings_bomb (bool update)
{
    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            MapTile* mt = curr_map->get_maptile (map_x, map_y);

            if (mt->bomb != NULL)
            {
                apply_bomb (mt->bomb->get_map_x(), mt->bomb->get_map_y(), mt->bomb->get_power(), update?1:0);
            }
        }
    }
}

void Controller_AI_mass::update_ratings_extra (bool update)
{
    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            if (update)
            {
                ratings[map_x][map_y] += extra_rating (map_x, map_y);
            }
            else
            {
                ratings[map_x][map_y] = extra_rating (map_x, map_y);
            }
        }
    }
}

bool Controller_AI_mass::ground_changed()
{
    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            if (explosions[map_x][map_y] == 1 || explosions[map_x][map_y] == 5)
            {
                return true;
            }
        }
    }

    return false;
}

bool Controller_AI_mass::passable(CL_Vector* map_v, int min_rating) const
{
    return (ratings[(int)map_v->x][(int)map_v->y] >= min_rating);
}

bool Controller_AI_mass::has_extra (CL_Vector* map_v) const
{
    for(std::list<GameObject*>::iterator object_iter = curr_map->get_maptile((int)map_v->x, (int)map_v->y)->objects.begin();
            object_iter != curr_map->get_maptile((int)map_v->x, (int)map_v->y)->objects.end();
            ++object_iter)
    {
        if ((*object_iter)->get_type() == GameObject::EXTRA)
        {
            return true;
        }
    }

    return false;
}

bool Controller_AI_mass::has_bomb(CL_Vector* map_v) const
{
    return (curr_map->get_maptile((int)map_v->x, (int)map_v->y)->bomb != NULL);
}

int Controller_AI_mass::get_rating (CL_Vector* map_v) const
{
    return ratings[(int)map_v->x][(int)map_v->y];
}

void Controller_AI_mass::set_put_bomb()
{
    if (curr_map->get_maptile (bomber->get_map_x(), bomber->get_map_y())->bomb == NULL &&
            bomber->get_cur_bombs() > 0 &&
            ratings[bomber->get_map_x()][bomber->get_map_y()] >= RATING_SURE &&
            attack_rating (bomber->get_map_x(), bomber->get_map_y()) > 0)
    {
        put_bomb = true;
    }
}

void Controller_AI_mass::clear_path()
{
    if (curr_path != NULL)
    {
        delete curr_path;
        curr_path = NULL;
        curr_dir = DIR_NONE;
    }
}

bool Controller_AI_mass::verify_path()
{
    if (curr_path == NULL)
    {
        return false;
    }

    if (!curr_path->verify())
    {
        clear_path();
    }

    return true;
}

void Controller_AI_mass::assign_path()
{
    CL_Vector b_v(bomber->get_map_x(), bomber->get_map_y());
    int curr_rating = ratings[(int)bomber->get_map_x()][(int)bomber->get_map_y()];

    if (passable (&b_v, RATING_SURE))
    {
        //printf ("placing bomb\n");
        if (place_bomb())
        {
            //printf ("placed bomb.\n");
            return;
        }
    }
    else
    {
        //printf ("sure position.\n");
        std::vector<CL_Vector*>* sure_path = sure_position();
        if (sure_path != NULL)
        {
            //printf ("sure pos ok.\n");
            curr_path = new AIPath(this, *sure_path, AI_panic);
            sure_path->clear();
            delete sure_path;
        }
        return;
    }

    //printf ("seeking util.\n");
    std::vector<CL_Vector*>* util_path = seek_util();
    if (util_path != NULL)
    {
        //printf ("util path ok.\n");
        AIAction action = AI_get;
        if (curr_rating <= RATING_EXPLODING)
        {
            action = AI_panic;
        }

        curr_path = new AIPath (this, *util_path, action);
        util_path->clear();
        delete util_path;
        return;
    }

    //printf ("placing bomb.\n");
    std::vector<CL_Vector*>* bombpath = bomb_path();
    if (bombpath != NULL)
    {
        //printf ("bomb path found, path len (%d).\n", bombpath->get_num_items());
        curr_path = new AIPath (this, *bombpath, AI_put);
        bombpath->clear();
        delete bombpath;
        return;
    }
    //printf ("no paths ok - wait.\n");
}

void Controller_AI_mass::follow_path()
{
    curr_dir = DIR_NONE;

    if (curr_path != NULL)
    {
        curr_dir = curr_path->follow();
        if (curr_path->passed())
        {
            clear_path();
        }
    }
}

static int vlen(CL_Vector v1, CL_Vector v2)
{
    return (abs((int)v2.x - (int)v1.x) + abs((int)v2.y - (int)v1.y));
}

std::vector<CL_Vector*>* Controller_AI_mass::seek_util()
{
    std::vector<CL_Vector*>* ret_path = NULL;
    CL_Vector b_v(bomber->get_map_x(), bomber->get_map_y());
    std::vector<CL_Vector*> ret_list;

    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            if (ratings[map_x][map_y] > 0)
            {
                std::vector<CL_Vector*>::iterator a_link = ret_list.end();
                if (ret_list.size() > 0)
                {
                    for (std::vector<CL_Vector*>::iterator list_iter = ret_list.begin();
                            list_iter != ret_list.end();
                            ++list_iter)
                    {
                        if ((int)(*list_iter)->z <= ratings[map_x][map_y])
                        {
                            a_link = list_iter;
                            break;
                        }
                    }
                }
                ret_list.insert(a_link, new CL_Vector(map_x, map_y, ratings[map_x][map_y]));
            }
        }
    }
  
    for (std::vector<CL_Vector*>::iterator iter = ret_list.begin();
            iter != ret_list.end(); ++iter)
    {
        assert(*iter != NULL);
        ret_path = path(b_v, *(*iter), RATING_SURE, RATING_SURE);

        if (ret_path != NULL)
        {
            break;
        }
    }

    for (std::vector<CL_Vector*>::iterator iter = ret_list.begin();
            iter != ret_list.end(); ++iter)
    {
        delete *iter;
    }

    return ret_path;
}

std::vector<CL_Vector*>* Controller_AI_mass::sure_position()
{
    //CL_List<CL_Vector>* ret_path = NULL;
    std::vector<CL_Vector*>* ret_path = NULL;
    CL_Vector b_v(bomber->get_map_x(), bomber->get_map_y());
    CL_Vector sure_v(-1, -1, -1);

    ret_path = path(b_v, sure_v, RATING_ESCAPE, RATING_SURE);
    if (ret_path == NULL)
    {
        ret_path = path(b_v, sure_v, RATING_PANIC, RATING_SURE);
        if (ret_path == NULL)
        {
            ret_path = path(b_v, sure_v, RATING_PANIC, RATING_ESCAPE);
            if (ret_path == NULL)
            {
                ret_path = path(b_v, sure_v, RATING_PANIC, RATING_PANIC);
            }
        }
    }
    return ret_path;
}

bool Controller_AI_mass::place_bomb()
{
    if (bomber->get_cur_bombs() <= 0)
    {
        return false;
    }

    int attack = attack_rating (bomber->get_map_x(), bomber->get_map_y());
    //printf ("attack rating returned (%d)\n", attack);
    if (attack > 0)
    {
        apply_bomb(bomber->get_map_x(), bomber->get_map_y(), bomber->get_power(), MADD);

        CL_Vector b_v(bomber->get_map_x(), bomber->get_map_y());
        CL_Vector sure_v(-1, -1, -1);

        std::vector<CL_Vector*>* way_out = path(b_v, sure_v, RATING_EXPLODING, RATING_SURE);
        if (way_out != NULL)
        {
            put_bomb = true;

            for (std::vector<CL_Vector*>::iterator iter = way_out->begin();
                    iter != way_out->end(); ++iter)
            {
                delete *iter;
            }
            delete way_out;

            return true;
        }
        else
        {
            apply_bomb(bomber->get_map_x(), bomber->get_map_y(), bomber->get_power(), MDEL);
        }
    }

    return false;
}

std::vector<CL_Vector*>* Controller_AI_mass::bomb_path()
{
    if (bomber->get_cur_bombs() <= 0)
    {
        return NULL;
    }

    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            if (ratings[map_x][map_y] >= RATING_SURE)
            {
                ratings[map_x][map_y] = attack_rating (map_x, map_y);
            }
        }
    }

    CL_Vector b_v(bomber->get_map_x(), bomber->get_map_y());
    CL_Vector sure_v(-1, -1, -1);

    //printf ("get put bomb path.\n");
    //CL_List<CL_Vector>* bombpath = path (b_v, sure_v, RATING_SURE, RATING_POS);
    std::vector<CL_Vector*>* bombpath = path(b_v, sure_v, RATING_SURE, RATING_POS);

    return bombpath;
}

Explosion* Controller_AI_mass::get_explosion (int map_x, int map_y) const
{
    for (std::list<GameObject*>::iterator iter
            = curr_map->get_maptile(map_x, map_y)->objects.begin();
            iter != curr_map->get_maptile(map_x, map_y)->objects.end(); ++iter)
    {
        if ((*iter)->get_type() == GameObject::EXPLOSION)
        {
            return static_cast<Explosion*>(*iter);
        }
    }

    return NULL;
}

bool Controller_AI_mass::has_extra (int map_x, int map_y) const
{
    if (!curr_map->get_maptile(map_x, map_y)->vanishing)
    {
        for (std::list<GameObject*>::iterator iter
                = curr_map->get_maptile(map_x, map_y)->objects.begin();
                iter != curr_map->get_maptile(map_x, map_y)->objects.end(); ++iter)
        {
            if ((*iter)->get_type() == GameObject::EXTRA)
            {
                return true;
            }
        }
    }

    return false;
}

bool Controller_AI_mass::has_disease (int map_x, int map_y) const
{
    if (!curr_map->get_maptile(map_x, map_y)->vanishing)
    {
        for (std::list<GameObject*>::iterator iter
                = curr_map->get_maptile (map_x, map_y)->objects.begin();
                iter != curr_map->get_maptile (map_x, map_y)->objects.end(); ++iter)
        {
            if ((*iter)->get_type() == GameObject::EXTRA)
            {
                Extra* extra = static_cast<Extra*>(*iter);
                if (extra->get_ExtraType() == Extra::JOINT
                        || extra->get_ExtraType() == Extra::KOKS
                        || extra->get_ExtraType() == Extra::VIAGRA)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

int Controller_AI_mass::attack_rating(int map_x, int map_y) const
{
    int num = 0;
    int i = map_x;
    int j = map_y;
    int power = bomber->get_power();

    if (ratings[i][j] < 0)
    {
        return -1;
    }
    else
    {
        if (enemy_bomber (i, j))
        {
            num += RATING_ENEMY_BOMBER;
        }
    }

    int x = 1;
    for (x = 1; x <= power; x++)
    {
        if (i-x >= 0 && curr_map->get_maptile (i-x, j)->get_type() != MapTile::WALL)
        {
            if (box_hits[i-x][j] || team_bomber (i-x, j) || (has_extra (i-x, j) && !has_disease (i-x, j)))
            {
                return -1;
            }

            if (enemy_bomber (i-x, j))
            {
                num += RATING_ENEMY_BOMBER;
            }

            if (curr_map->get_maptile (i-x, j)->get_type() == MapTile::BOX)
            {
                num += RATING_BOMB_BOX;
                break;
            }

            if (has_disease (i-x, j))
            {
                num += RATING_DISEASE;
                break;
            }
        }
        else
        {
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (i+x < MAP_WIDTH && !curr_map->get_maptile(i+x, j)->get_type() != MapTile::WALL)
        {
            if (box_hits[i+x][j] || team_bomber (i+x, j) || (has_extra (i+x, j) && !has_disease (i+x, j)))
            {
                return -1;
            }

            if (enemy_bomber (i+x, j))
            {
                num += RATING_ENEMY_BOMBER;
            }

            if (curr_map->get_maptile (i+x, j)->get_type() == MapTile::BOX)
            {
                num += RATING_BOMB_BOX;
                break;
            }

            if (has_disease (i+x, j))
            {
                num += RATING_DISEASE;
                break;
            }
        }
        else
        {
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j-x >= 0 && !curr_map->get_maptile(i, j-x)->get_type() != MapTile::WALL)
        {
            if (box_hits[i][j-x] || team_bomber (i, j-x) || (has_extra (i, j-x) && !has_disease (i, j-x)))
            {
                return -1;
            }

            if (enemy_bomber (i, j-x))
            {
                num += RATING_ENEMY_BOMBER;
            }

            if (curr_map->get_maptile (i, j-x)->get_type() == MapTile::BOX)
            {
                num += RATING_BOMB_BOX;
                break;
            }

            if (has_disease (i, j-x))
            {
                num += RATING_DISEASE;
                break;
            }
        }
        else
        {
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j+x < MAP_HEIGHT && !curr_map->get_maptile(i, j+x)->get_type() != MapTile::WALL)
        {
            if (box_hits[i][j+x] || team_bomber (i, j+x) || (has_extra (i, j+x) && !has_disease (i, j+x)))
            {
                return -1;
            }

            if (enemy_bomber (i, j+x))
            {
                num += RATING_ENEMY_BOMBER;
            }

            if (curr_map->get_maptile (i, j+x)->get_type() == MapTile::BOX)
            {
                num += RATING_BOMB_BOX;
                break;
            }

            if (has_disease (i, j+x))
            {
                num += RATING_DISEASE;
                break;
            }
        }
        else
        {
            break;
        }
    }

    int bx = bomber->get_map_x();
    int by = bomber->get_map_y();
    int tmp = abs(bx - map_x) + abs(by - map_y);
    if (tmp == 0)
    {
        tmp = 1;
    }

    return (num / tmp);
}

bool Controller_AI_mass::enemy_bomber(int map_x, int map_y) const
{
    if (!curr_map->get_maptile (map_x, map_y)->vanishing)
    {
        for (std::list<GameObject*>::iterator iter
                = curr_map->get_maptile (map_x, map_y)->objects.begin();
                iter != curr_map->get_maptile (map_x, map_y)->objects.end();
                ++iter)
        {
            if ((*iter)->get_type() == GameObject::BOMBER)
            {
                Bomber* foreign_bomber = static_cast<Bomber*>(*iter);
                if (foreign_bomber != bomber)
                {
                    if (foreign_bomber->get_team() == 0)
                    {
                        return true;
                    }
                    else
                    {
                        if (foreign_bomber->get_team() == bomber->get_team())
                        {
                            return false;
                        }
                        else
                        {
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    return false;
}

bool Controller_AI_mass::team_bomber(int map_x, int map_y) const
{
    if (bomber->get_team() == 0)
    {
        return false;
    }

    if (!curr_map->get_maptile (map_x, map_y)->vanishing)
    {
        for (std::list<GameObject*>::iterator iter
                = curr_map->get_maptile(map_x, map_y)->objects.begin();
                iter != curr_map->get_maptile(map_x, map_y)->objects.end(); ++iter)
        {
            if ((*iter)->get_type() == GameObject::BOMBER)
            {
                Bomber* foreign_bomber = static_cast<Bomber*>(*iter);
                if (foreign_bomber->get_team() == bomber->get_team() && foreign_bomber != bomber)
                {
                    return true;
                }
            }
        }

        return false;
    }

    return false;
}

bool Controller_AI_mass::destroys_extras(int map_x, int map_y)
{
    int i = map_x;
    int j = map_y;
    int power = bomber->get_power();

    int x = 1;
    for (x = 1; x <= power; x++)
    {
        if (i-x >= 0 && curr_map->get_maptile (i-x, j)->blocking)
        {
            if (has_extra (i-x, j) && !has_disease (i-x, j))
            {
                return true;
            }
        }
        else
        {
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (i+x < MAP_WIDTH && !curr_map->get_maptile(i+x, j)->blocking)
        {
            if (has_extra (i+x, j) && !has_disease (i+x, j))
            {
                return true;
            }
        }
        else
        {
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j-x >= 0 && !curr_map->get_maptile(i, j-x)->blocking)
        {
            if (has_extra (i, j-x) && !has_disease (i, j-x))
            {
                return true;
            }
        }
        else
        {
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j+x < MAP_HEIGHT && !curr_map->get_maptile(i, j+x)->blocking)
        {
            if (has_extra (i, j+x) && !has_disease (i, j+x))
            {
                return true;
            }
        }
        else
        {
            break;
        }
    }

    return false;
}

void Controller_AI_mass::apply_explosion(Explosion* exp)
{
    int power = exp->get_power();
    int i = exp->get_map_x();
    int j = exp->get_map_y();

    ratings[i][j] = RATING_EXPLOSION;

    if (curr_map->get_maptile(i, j)->get_type() == MapTile::BOX)
    {
        box_hits[i][j] = true;
    }

    int x = 1;
    for (x = 1; x <= power; x++)
    {
        if (i-x >= 0 && !curr_map->get_maptile(i-x, j)->blocking)
        {
            ratings[i-x][j] = RATING_EXPLOSION;
            if (has_extra (i-x, j))
            {
                break;
            }
        }
        else
        {
            if (i-x >= 0 && curr_map->get_maptile(i-x, j)->get_type() == MapTile::BOX)
            {
                box_hits[i-x][j] = true;
            }
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (i+x < MAP_WIDTH && !curr_map->get_maptile(i+x, j)->blocking)
        {
            ratings[i+x][j] = RATING_EXPLOSION;
            if (has_extra (i+x, j))
            {
                break;
            }
        }
        else
        {
            if (i+x < MAP_WIDTH && curr_map->get_maptile(i+x, j)->get_type() == MapTile::BOX)
            {
                box_hits[i+x][j] = true;
            }
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j-x >= 0 && !curr_map->get_maptile(i, j-x)->blocking)
        {
            ratings[i][j-x] = RATING_EXPLOSION;
            if (has_extra (i, j-x))
            {
                break;
            }
        }
        else
        {
            if (j-x >= 0 && curr_map->get_maptile(i, j-x)->get_type() == MapTile::BOX)
            {
                box_hits[i][j-x] = true;
            }
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j+x < MAP_HEIGHT && !curr_map->get_maptile(i, j+x)->blocking)
        {
            ratings[i][j+x] = RATING_EXPLOSION;
            if (has_extra (i, j+x))
            {
                break;
            }
        }
        else
        {
            if (j+x < MAP_HEIGHT && curr_map->get_maptile(i, j+x)->get_type() == MapTile::BOX)
            {
                box_hits[i][j+x] = true;
            }
            break;
        }
    }
}

void Controller_AI_mass::apply_bomb(int map_x, int map_y, int power, int mode)
{
    int i = map_x;
    int j = map_y;

    if (mode == MINIT)
    {
        ratings[i][j] = RATING_BOMB;
    }
    else if (mode == MADD)
    {
        ratings[i][j] += RATING_BOMB;
    }
    else
    {
        ratings[i][j] -= RATING_BOMB;
    }

    if (curr_map->get_maptile(i, j)->get_type() == MapTile::BOX)
    {
        box_hits[i][j] = true;
    }

    int rating = RATING_EXPLODING;
    if (curr_map->get_maptile (i, j)->bomb != NULL)
    {
        rating = RATING_EXPLODING - ((int)(Config::get_bomb_countdown() * 100) - (int)(curr_map->get_maptile (map_x, map_y)->bomb->get_countdown() * 1000));
    }

    if (mode == MDEL)
    {
        rating = -rating;
    }

    int x = 1;
    for (x = 1; x <= power; x++)
    {
        if (i-x >= 0 && !curr_map->get_maptile(i-x, j)->blocking)
        {
            if (mode == MINIT)
            {
                ratings[i-x][j] = rating;
            }
            else
            {
                ratings[i-x][j] += rating;
            }

            if (has_extra (i-x, j))
            {
                break;
            }
        }
        else
        {
            if (i-x >= 0 && curr_map->get_maptile(i-x, j)->get_type() == MapTile::BOX)
            {
                box_hits[i-x][j] = true;
            }
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (i+x < MAP_WIDTH && !curr_map->get_maptile(i+x, j)->blocking)
        {
            if (mode == MINIT)
            {
                ratings[i+x][j] = rating;
            }
            else
            {
                ratings[i+x][j] += rating;
            }

            if (has_extra (i+x, j))
            {
                break;
            }
        }
        else
        {
            if (i+x < MAP_WIDTH && curr_map->get_maptile(i+x, j)->get_type() == MapTile::BOX)
            {
                box_hits[i+x][j] = true;
            }
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j-x >= 0 && !curr_map->get_maptile(i, j-x)->blocking)
        {
            if (mode == MINIT)
            {
                ratings[i][j-x] = rating;
            }
            else
            {
                ratings[i][j-x] += rating;
            }

            if (has_extra (i, j-x))
            {
                break;
            }
        }
        else
        {
            if (j-x >= 0 && curr_map->get_maptile(i, j-x)->get_type() == MapTile::BOX)
            {
                box_hits[i][j-x] = true;
            }
            break;
        }
    }

    for (x = 1; x <= power; x++)
    {
        if (j+x < MAP_HEIGHT && !curr_map->get_maptile(i, j+x)->blocking)
        {
            if (mode == MINIT)
            {
                ratings[i][j+x] = rating;
            }
            else
            {
                ratings[i][j+x] += rating;
            }

            if (has_extra (i, j+x))
            {
                break;
            }
        }
        else
        {
            if (j+x < MAP_HEIGHT && curr_map->get_maptile(i, j+x)->get_type() == MapTile::BOX)
            {
                box_hits[i][j+x] = true;
            }
            break;
        }
    }
}

int Controller_AI_mass::extra_rating(int map_x, int map_y) const
{
    int bomber_x = bomber->get_map_x();
    int bomber_y = bomber->get_map_y();
    int rating = 0;
    bool has_viagra = (bomb_mode == ALWAYS);
    bool can_kick = bomber->is_able_to_kick();
    bool can_throw = bomber->is_able_to_throw();
    bool max_power = (Config::get_max_power() <= bomber->get_power());
    bool max_bomb = (Config::get_max_bombs() <= bomber->get_bombs());
    bool max_speed = (Config::get_max_skateboards() <= bomber->get_skateboards());

    for (std::list<GameObject*>::iterator iter
            = curr_map->get_maptile(map_x, map_y)->objects.begin();
            iter != curr_map->get_maptile(map_x, map_y)->objects.end(); ++iter)
    {
        if ((*iter)->get_type() == GameObject::EXTRA)
        {
            Extra* extra = static_cast<Extra*>(*iter);
            switch (extra->get_ExtraType())
            {
            case Extra::BOMB:
                rating += RATING_EXTRA_BOMB;
                break;
            case Extra::POWER:
                rating += RATING_EXTRA_POWER;
                break;
            case Extra::KICK:
                rating += RATING_EXTRA_KICK;
                break;
            case Extra::GLOVE:
                rating += RATING_EXTRA_GLOVE;
                break;
            case Extra::SKATEBOARD:
                rating += RATING_EXTRA_SKATEBOARD;
                break;
            case Extra::JOINT:
                rating += RATING_DISEASE_JOINT;
                break;
            case Extra::KOKS:
                rating += RATING_DISEASE_KOKS;
                break;
            case Extra::VIAGRA:
                rating += RATING_DISEASE_VIAGRA;
                break;
            default:
                break;
            }
        }
    }

    int ret_tmp = abs (bomber_x - map_x) + abs (bomber_y - map_y);
    if (ret_tmp == 0)
    {
        ret_tmp = 1;
    }

    return (rating / ret_tmp);
}

static std::vector<CL_Vector*>* reversel(std::vector<CL_Vector*>* list)
{
    if (list == NULL)
    {
        return NULL;
    }

    std::vector<CL_Vector*>* rev_list = new std::vector<CL_Vector*>;

    for (std::vector<CL_Vector*>::reverse_iterator iter = list->rbegin();
            iter != list->rend(); ++iter)
    {
        rev_list->push_back(*iter);
    }

    delete list;

    return rev_list;
}

std::vector<CL_Vector*>* Controller_AI_mass::path(CL_Vector start_v, CL_Vector dest_v, int min_path_rating, int min_end_rating)
{
    std::vector<CL_Vector*>* ret_path = NULL;
    std::vector<CL_Vector*> expansions;
    std::vector<CL_Vector*> curr_queue;
    int distance_map[MAP_WIDTH][MAP_HEIGHT];
    int curr_distance = 0;
    bool found_dest = false;
    CL_Vector *curr_v = NULL;
    int perm_dirs[4] = {DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN};
    CL_Vector sure_v(-1, -1, -1);
    bool sure = (dest_v == sure_v);

    for (int map_x = 0; map_x < MAP_WIDTH; map_x++)
    {
        for (int map_y = 0; map_y < MAP_HEIGHT; map_y++)
        {
            distance_map[map_x][map_y] = -1;
        }
    }

    dest_v.z = 0;

    distance_map[(int)start_v.x][(int)start_v.y] = curr_distance;
    expansions.push_back(new CL_Vector (start_v));

    while(expansions.size() > 0 && !found_dest)
    {
        curr_distance++;

        //TODO optimize this using sort
        std::vector<CL_Vector*> sorted_expansions;
        while (expansions.size() > 0)
        {
            int curr_rating = -9999999;
            CL_Vector* add_expansion = NULL;
            //for compat
            std::vector<CL_Vector*>::iterator extra_iter;
            for (std::vector<CL_Vector*>::iterator iter = expansions.begin();
                    iter != expansions.end(); ++iter)
            {
                CL_Vector* expansion = *iter;
                if (ratings[(int)expansion->x][(int)expansion->y] > curr_rating)
                {
                    add_expansion = expansion;
                    curr_rating = ratings[(int)expansion->x][(int)expansion->y];
                    extra_iter = iter;
                }
            }

            sorted_expansions.push_back(add_expansion);
            expansions.erase(extra_iter);
        }

        curr_queue.insert(curr_queue.end(), sorted_expansions.begin(),
                          sorted_expansions.end());

        sorted_expansions.clear();

        for (std::vector<CL_Vector*>::iterator curr_iter = curr_queue.begin();
                (curr_iter != curr_queue.end()) && !found_dest;)
        {
            curr_v = *curr_iter;
            curr_iter = curr_queue.erase(curr_iter);

            //XXX cannot this be done using std::swap
            std::swap(perm_dirs[3], perm_dirs[random(3)]);
            std::swap(perm_dirs[2], perm_dirs[random(2)]);

            float save_x = curr_v->x;
            float save_y = curr_v->y;

            for (int i = 0; i < 4; i++)
            {
                bool feasible = false;
                switch (perm_dirs[i])
                {
                case DIR_LEFT:
                    if (curr_v->x == 0)
                    {
                        break;
                    }
                    curr_v->x--;
                    feasible = true;
                    break;
                case DIR_RIGHT:
                {
                    if (curr_v->x == MAP_WIDTH-1)
                    {
                        break;
                    }
                    curr_v->x++;
                    feasible = true;
                    break;
                }
                case DIR_UP:
                {
                    if (curr_v->y == 0)
                    {
                        break;
                    }
                    curr_v->y--;
                    feasible = true;
                    break;
                }
                case DIR_DOWN:
                {
                    if (curr_v->y == MAP_HEIGHT-1)
                    {
                        break;
                    }
                    curr_v->y++;
                    feasible = true;
                    break;
                }
                default:
                    break;
                }

                if (feasible)
                {
                    if ((*curr_v) == dest_v)
                    {
                        found_dest = true;
                        break;
                    }
                    else
                    {
                        if (sure && passable (curr_v, min_end_rating))
                        {
                            dest_v.x = curr_v->x;
                            dest_v.y = curr_v->y;
                            found_dest = true;
                            break;
                        }
                        else
                        {
                            if (passable (curr_v, min_path_rating)
                                    && distance_map[(int)curr_v->x][(int)curr_v->y] < 0)
                            {
                                expansions.push_back(new CL_Vector(*curr_v));
                                distance_map[(int)curr_v->x][(int)curr_v->y] = curr_distance;
                            }
                        }
                    }

                    curr_v->x = save_x;
                    curr_v->y = save_y;
                }
            }

            delete curr_v;

        }
    }

    if (found_dest)
    {

        ret_path = new std::vector<CL_Vector*>;
        ret_path->push_back(new CL_Vector(dest_v));

        while (curr_distance-- > 0)
        {
            if (dest_v.y > 0)
            {
                dest_v.y--;
                if (distance_map[(int)dest_v.x][(int)dest_v.y] == curr_distance)
                {

                    ret_path->push_back(new CL_Vector (dest_v));
                    continue;
                }
                dest_v.y++;
            }

            if (dest_v.x < MAP_WIDTH-1)
            {
                dest_v.x++;
                if (distance_map[(int)dest_v.x][(int)dest_v.y] == curr_distance)
                {
                    ret_path->push_back(new CL_Vector (dest_v));
                    continue;
                }
                dest_v.x--;
            }

            if (dest_v.y < MAP_HEIGHT-1)
            {
                dest_v.y++;
                if (distance_map[(int)dest_v.x][(int)dest_v.y] == curr_distance)
                {
                    ret_path->push_back(new CL_Vector (dest_v));
                    continue;
                }
                dest_v.y--;
            }

            if (dest_v.x > 0)
            {
                dest_v.x--;
                if (distance_map[(int)dest_v.x][(int)dest_v.y] == curr_distance)
                {
                     ret_path->push_back(new CL_Vector (dest_v));
                    continue;
                }
                dest_v.x++;
            }
        }
    }
    else
    {
    }

    return reversel (ret_path);
}
