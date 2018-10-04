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

#ifndef CLANBOMBER_H
#define CLANBOMBER_H

#include <list>

#include <boost/filesystem.hpp>

#include "AudioSimple.h"

//XXX Not needed here but in most files that include this one
#include "Resources.h"

class Menu;
class Observer;
class Map;
class Bomb;
class Bomber;
class GameObject;
class Server;
class Client;

enum
{
    MENU_GAME,
    MENU_OPTIONS,
    MENU_GRAPHICS,
    MENU_CREDITS,
    MENU_PLAYER_SETUP,
    MENU_EXTRA_VALUES,
    MENU_EXTRA_ONOFF,
    MENU_EXIT,
    MENU_CONTROLLER,
    MENU_HELP,
    MENU_DISEASE_ONOFF,
    MENU_MAP_EDITOR,
    MENU_TIMING,

    CONFIG_START_BOMBS,
    CONFIG_START_POWER,
    CONFIG_START_SKATES,
    CONFIG_START_KICK,
    CONFIG_START_GLOVE,

    CONFIG_BOMBS,
    CONFIG_POWER,
    CONFIG_SKATES,
    CONFIG_KICK,
    CONFIG_GLOVE,

    CONFIG_MAX_BOMBS,
    CONFIG_MAX_POWER,
    CONFIG_MAX_SKATES,

    CONFIG_POINTS,
    CONFIG_MAP_SEL,
    CONFIG_ROUND_TIME,

    CONFIG_JOINT,
    CONFIG_VIAGRA,
    CONFIG_KOKS,

    CONFIG_KIDS_MODE,
    CONFIG_SHAKE,
    CONFIG_CORPSE_PARTS,
    CONFIG_RANDOM_POSITIONS,
    CONFIG_RANDOM_MAP_ORDER,

    CONFIG_BOMB_COUNTDOWN,
    CONFIG_BOMB_DELAY,
    CONFIG_BOMB_SPEED,

    CONFIG_RANDOM_POSITIONS_DUPLICATE,
    CONFIG_RANDOM_MAP_ORDER_DUPLICATE,
    CONFIG_POINTS_DUPLICATE,
    CONFIG_ROUND_TIME_DUPLICATE,

    LOCALGAME_START,
    SERVERGAME_START,
    CLIENTGAME_START
};

enum Direction
{
    DIR_NONE  = -1,
    DIR_DOWN  = 0,
    DIR_LEFT  = 1,
    DIR_UP    = 2,
    DIR_RIGHT = 3
};

unsigned long WELLRNG512();

/**
 * @author Andreas Hundt
 * @author Denis Oliver Kropp
 */
class ClanBomberApplication
{
public:
    ClanBomberApplication();
    ~ClanBomberApplication();
    int main();
    Map* map;
    std::list<GameObject*> objects;
    std::vector<Bomber*> bomber_objects;

    int get_server_status();
    static int run_server_with_players();

    static Menu* get_menu();

    void set_client_disconnected_from_server(bool d);
    bool is_client_disconnected_from_server();
    void set_client_connecting_to_new_server(bool c);
    bool is_client_connecting_to_new_server();
    void client_to_server();
    void make_observer();
    void make_map();
    Observer* get_observer();
    Map* get_map();
    bool paused_game();
    void set_pause_game(bool paused);
    void reload_map();
    void delete_all_game_objects();

    GameObject* get_object_by_id(int object_id);

    static unsigned short get_next_object_id();

    static boost::filesystem::path get_map_path();
    static boost::filesystem::path get_local_map_path();

    bool bombers_received_by_client;
    static int get_server_frame_counter();
    static void inc_server_frame_counter();
    std::list<Bomb*> suspended_client_bombs;

    cbe::AudioSimple *get_audio()
    {
        return AS;
    };

protected:
    int init_SDL();
    void init_audio();
    void run_intro();
    bool init_game(); // single player init
    void deinit_game();
    void show_tutorial();

    void run_game();
    void show_all();
    void act_all();
    void delete_some();

    Menu* menu;

    Observer* observer;

    static boost::filesystem::path map_path;
    static boost::filesystem::path local_map_path;

    bool show_fps;
    bool play_music;
    int fps;
    int frame_count;
    float frame_time;
    int server_status;
    static int server_frame_counter;
    bool pause_game;
    bool client_disconnected_from_server;
    bool client_connecting_to_new_server;

private:
    cbe::AudioSimple *AS;
};

#endif
