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

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <string>

#include <boost/filesystem.hpp>

#define CURRENT_CONFIGFILE_VERSION 30

class BomberConfig
{
public:
    BomberConfig();
    ~BomberConfig();

    int		get_skin();
    std::string	get_name();
    int		get_team();
    int		get_controller();

    void	set_skin(int _skin);
    void	set_name(std::string _name);
    void	set_team(int _team);
    void	set_controller(int _controller);
    void	set_highlight_maptile(bool _highlight_maptile);
    int		get_highlight_maptile();
    void	enable();
    void	disable();
    int		is_enabled();
    void	set_enabled(bool _enabled);
    int		get_config_index();
    void	set_config_index(int index);

protected:
    int		enabled;
    int		skin;
    std::string	name;
    int		team;
    int		controller;
    int		highlight_maptile;
    int		client_index;
    int 	config_index;
};

class Config
{
public:
    static void set_round_time(int);

    static void set_max_skateboards(int);
    static void set_max_power(int);
    static void set_max_bombs(int);

    static void set_start_skateboards(int);
    static void set_start_power(int);
    static void set_start_bombs(int);
    static void set_start_kick(int);
    static void set_start_glove(int);

    static void set_skateboards(int);
    static void set_power(int);
    static void set_bombs(int);
    static void set_kick(int);
    static void set_glove(int);

    static void set_joint(int);
    static void set_viagra(int);
    static void set_koks(int);

    static void set_start_map(int);
    static void set_random_map_order(int);

    static void set_points_to_win(int);
    static void set_theme(int);
    static void set_music(int);

    static void set_kids_mode(int);
    static void set_shaky_explosions(int);
    static void set_random_positions(int);
    static void set_corpse_parts(int);

    static void set_bomb_countdown(int);
    static void set_bomb_delay(int);
    static void set_bomb_speed(int);

    static void set_filename(boost::filesystem::path _filename);
    static void set_path(boost::filesystem::path _path);

    static int get_round_time();
    static int get_random_map_order();

    static int get_max_skateboards();
    static int get_max_power();
    static int get_max_bombs();

    static int get_start_skateboards();
    static int get_start_power();
    static int get_start_bombs();
    static int get_start_kick();
    static int get_start_glove();

    static int get_skateboards();
    static int get_power();
    static int get_bombs();
    static int get_kick();
    static int get_glove();

    static int get_joint();
    static int get_viagra();
    static int get_koks();

    static int get_start_map();

    static int get_points_to_win();
    static int get_theme();

    static int get_music();

    static int get_kids_mode();
    static int get_shaky_explosions();
    static int get_random_positions();
    static int get_corpse_parts();

    static int get_number_of_players();
    static int get_number_of_opponents();

    static int get_bomb_countdown();
    static int get_bomb_delay();
    static int get_bomb_speed();

    static bool get_fullscreen();
    static void set_fullscreen(bool val);

    static bool get_softwareRendering();
    static void set_softwareRendering(bool val);

    static bool load();
    static bool save(bool init=false);
    static int get_sound_enabled();
    static BomberConfig bomber[8];

protected:
    static int round_time;
    static int random_map_order;
    static int sound_enabled;
    static int max_skateboards;
    static int max_power;
    static int max_bombs;

    static int start_bombs;
    static int start_power;
    static int start_skateboards;
    static int start_kick;
    static int start_glove;

    static int bombs;
    static int power;
    static int skateboards;
    static int kick;
    static int glove;

    static int joint;
    static int viagra;
    static int koks;

    static int start_map;
    static int music;
    static int kids_mode;
    static int shaky_explosions;
    static int random_positions;
    static int corpse_parts;

    static int bomb_countdown;
    static int bomb_delay;
    static int bomb_speed;

    static int points_to_win;
    static int theme;

    static bool fullscreen;

    static bool softwareRendering;

    static boost::filesystem::path filename;
    static boost::filesystem::path path;
};

#endif


