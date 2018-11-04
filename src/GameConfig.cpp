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

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "ClanBomber.h"
#include "GameConfig.h"

#include "Controller.h"

#include <stdio.h>

boost::filesystem::path Config::filename = "clanbomber.cfg";
boost::filesystem::path Config::path     = "";

std::string Config::last_server= "intruder";

int	Config::round_time			= 90;
int Config::sound_enabled		= true;
int	Config::max_skateboards		= 5;
int	Config::max_power			= 12;
int	Config::max_bombs			= 9;

int	Config::start_skateboards	= 0;
int	Config::start_power			= 1;
int	Config::start_bombs			= 1;
int	Config::start_kick			= false;
int	Config::start_glove			= false;

int	Config::skateboards			= true;
int	Config::power				= true;
int	Config::bombs				= true;
int	Config::kick				= true;
int	Config::glove				= true;

int	Config::joint				= true;
int	Config::viagra				= true;
int	Config::koks				= true;

int	Config::start_map			= 0;
int Config::random_map_order	= false;

int	Config::music				= false;

int	Config::kids_mode			= false;
int	Config::shaky_explosions	= true;
int	Config::random_positions	= true;
int	Config::corpse_parts		= 10;

int	Config::bomb_countdown		= 25;
int	Config::bomb_delay			= 10;
int	Config::bomb_speed			= 160;

int	Config::points_to_win		= 5;
int	Config::theme				= 0;

bool Config::fullscreen = false;

bool Config::softwareRendering = false;

BomberConfig Config::bomber[8];

BomberConfig::BomberConfig()
{
    local_client = true;
    server_bomber = false;
    client_index = -1;
    config_index = -1;
    client_ip = NULL;
    enabled = true;
    team = 0;
    skin = 0;
    controller = 0;
    name = "Fischlustig";
}

BomberConfig::~BomberConfig()
{
}

void BomberConfig::set_name(std::string _name)
{
    if (_name.length())
    {
        name = _name;
    }
}

std::string BomberConfig::get_name()
{
    return name;
}

void BomberConfig::set_skin(int _skin)
{
    skin = _skin%NR_BOMBERSKINS;
}

int BomberConfig::get_skin()
{
    return skin;
}

void BomberConfig::set_team(int _team)
{
    team = _team%5;
}

int BomberConfig::get_team()
{
    return team;
}

void BomberConfig::set_controller(int _controller)
{
    controller = _controller % (6 + SDL_NumJoysticks());
}

int BomberConfig::get_controller()
{
    return controller;
}

void BomberConfig::enable()
{
    enabled = true;
}

void BomberConfig::disable()
{
    enabled = false;
}

void BomberConfig::set_highlight_maptile(bool _highlight_maptile)
{
    highlight_maptile = _highlight_maptile;
}

int BomberConfig::get_highlight_maptile()
{
    return highlight_maptile;
}

int BomberConfig::is_enabled()
{
    return enabled;
}

void BomberConfig::set_enabled(bool _enabled)
{
    enabled = _enabled;
}

void BomberConfig::set_client_index(int index)
{
    client_index = index;
}

int BomberConfig::get_client_index()
{
    return client_index;
}

int BomberConfig::get_config_index()
{
    return config_index;
}

void BomberConfig::set_config_index(int index)
{
    config_index=index;
}

char* BomberConfig::get_client_ip()
{
    return client_ip;
}

void BomberConfig::set_client_ip(char* ip_string)
{
    client_ip = ip_string;
}

int Config::get_number_of_players()
{
    int nr = 0;

    for (int i=0; i<8; i++)
    {
        if (bomber[i].is_enabled())
        {
            nr++;
        }
    }

    return nr;
}

int Config::get_number_of_opponents()
{
    int nrs = 0;
    int team_cunt[4] = {0,0,0,0};

    for (int i=0; i<8; i++)
    {
        if (bomber[i].is_enabled())
        {
            if (bomber[i].get_team() == 0)
            {
                nrs++;
            }
            else
            {
                if (team_cunt[bomber[i].get_team()-1] == 0)
                {
                    nrs++;
                }
                team_cunt[bomber[i].get_team()-1]++;
            }
        }
    }

    return nrs;
}

void Config::set_round_time( int _round_time)
{
    round_time = _round_time;
}

void Config::set_random_map_order( int _random_map_order)
{
    random_map_order = _random_map_order;
}

void Config::set_max_skateboards(int _max_skateboards)
{
    max_skateboards = _max_skateboards;
}

void Config::set_max_power(int _max_power)
{
    max_power = _max_power;
}

void Config::set_max_bombs(int _max_bombs)
{
    max_bombs = _max_bombs;
}

void Config::set_start_skateboards(int _start_skateboards)
{
    start_skateboards = _start_skateboards;
}

void Config::set_start_power(int _start_power)
{
    start_power = _start_power;
}

void Config::set_start_bombs(int _start_bombs)
{
    start_bombs = _start_bombs;
}

void Config::set_start_kick( int _start_kick)
{
    start_kick = _start_kick;
}

void Config::set_start_glove( int _start_glove)
{
    start_glove = _start_glove;
}

void Config::set_skateboards(int _skateboards)
{
    skateboards = _skateboards;
}

void Config::set_power(int _power)
{
    power = _power;
}

void Config::set_bombs(int _bombs)
{
    bombs = _bombs;
}

void Config::set_kick( int _kick)
{
    kick = _kick;
}

void Config::set_glove( int _glove)
{
    glove = _glove;
}

void Config::set_joint( int _joint)
{
    joint = _joint;
}

void Config::set_viagra( int _viagra)
{
    viagra = _viagra;
}

void Config::set_koks( int _koks)
{
    koks = _koks;
}

void Config::set_start_map(int _start_map)
{
    start_map = _start_map;
}

void Config::set_points_to_win(int _points_to_win)
{
    points_to_win = _points_to_win;
}

void Config::set_theme(int _theme)
{
    theme = _theme;
}

void Config::set_filename(boost::filesystem::path _filename)
{
    filename = _filename;
}

void Config::set_path(boost::filesystem::path _path)
{
    path = _path;
}

void Config::set_music( int _music)
{
    music = _music;
}

void Config::set_kids_mode( int _kids_mode)
{
    kids_mode = _kids_mode;
}

void Config::set_corpse_parts( int _corpse_parts)
{
    corpse_parts = _corpse_parts;
}

void Config::set_shaky_explosions(int _shaky_explosions)
{
    shaky_explosions = _shaky_explosions;
}

void Config::set_random_positions(int _random_positions)
{
    random_positions = _random_positions;
}

void Config::set_bomb_countdown(int _bomb_countdown)
{
    bomb_countdown = _bomb_countdown;
}

void Config::set_bomb_delay(int _bomb_delay)
{
    bomb_delay = _bomb_delay;
}

void Config::set_bomb_speed(int _bomb_speed)
{
    bomb_speed = _bomb_speed;
}

int Config::get_max_skateboards()
{
    return max_skateboards;
}

int Config::get_max_power()
{
    return max_power;
}

int Config::get_max_bombs()
{
    return max_bombs;
}

int Config::get_start_skateboards()
{
    return start_skateboards;
}

int Config::get_start_power()
{
    return start_power;
}

int Config::get_start_bombs()
{
    return start_bombs;
}

int Config::get_start_kick()
{
    return start_kick;
}

int Config::get_start_glove()
{
    return start_glove;
}

int Config::get_skateboards()
{
    return skateboards;
}

int Config::get_power()
{
    return power;
}

int Config::get_bombs()
{
    return bombs;
}

int Config::get_kick()
{
    return kick;
}

int Config::get_glove()
{
    return glove;
}

int Config::get_joint()
{
    return joint;
}

int Config::get_viagra()
{
    return viagra;
}

int Config::get_koks()
{
    return koks;
}

int Config::get_start_map()
{
    return start_map;
}

int Config::get_random_map_order()
{
    return random_map_order;
}

int Config::get_round_time()
{
    return round_time;
}

int Config::get_points_to_win()
{
    return points_to_win;
}

int Config::get_theme()
{
    return theme;
}

int Config::get_sound_enabled()
{
    return sound_enabled;
}

int Config::get_music()
{
    return music;
}

int Config::get_kids_mode()
{
    return kids_mode;
}

int Config::get_corpse_parts()
{
    return corpse_parts;
}

int Config::get_shaky_explosions()
{
    return shaky_explosions;
}

int Config::get_random_positions()
{
    return random_positions;
}

int Config::get_bomb_countdown()
{
    return bomb_countdown;
}

int Config::get_bomb_delay()
{
    return bomb_delay;
}

int Config::get_bomb_speed()
{
    return bomb_speed;
}

bool Config::get_fullscreen()
{
    return fullscreen;
}

void Config::set_fullscreen(bool val)
{
    fullscreen = val;
}

bool Config::get_softwareRendering()
{
    return softwareRendering;
}

void Config::set_softwareRendering(bool val)
{
    softwareRendering = val;
}


bool Config::save(bool init)
{
    if (init)
    {
        for (int i=0; i<8; i++)
        {
            bomber[i].set_skin(i);
        }
        bomber[0].set_name( "Are" );
        bomber[1].set_name( "You" );
        bomber[2].set_name( "Still" );
        bomber[3].set_name( "Watching" );
        bomber[4].set_name( "AIs" );
        bomber[5].set_name( "Playing" );
        bomber[6].set_name( "For" );
        bomber[7].set_name( "You" );
    }

    boost::filesystem::ofstream configfile(path / filename);

    configfile << CURRENT_CONFIGFILE_VERSION << std::endl; // version

    configfile << max_bombs			<< std::endl;
    configfile << max_power			<< std::endl;
    configfile << max_skateboards		<< std::endl;

    configfile << start_bombs		<< std::endl;
    configfile << start_power		<< std::endl;
    configfile << start_skateboards		<< std::endl;
    configfile << start_kick		<< std::endl;
    configfile << start_glove		<< std::endl;

    configfile << start_map			<< std::endl;
    configfile << points_to_win		<< std::endl;
    configfile << round_time		<< std::endl;
    configfile << theme			<< std::endl;
    configfile << music			<< std::endl;

    configfile << kids_mode			<< std::endl;
    configfile << corpse_parts		<< std::endl;
    configfile << shaky_explosions		<< std::endl;
    configfile << random_positions		<< std::endl;
    configfile << random_map_order		<< std::endl;

    configfile << bombs			<< std::endl;
    configfile << power			<< std::endl;
    configfile << skateboards		<< std::endl;
    configfile << kick			<< std::endl;
    configfile << glove			<< std::endl;

    configfile << joint			<< std::endl;
    configfile << viagra			<< std::endl;
    configfile << koks			<< std::endl;

    configfile << bomb_countdown		<< std::endl;
    configfile << bomb_delay		<< std::endl;
    configfile << bomb_speed		<< std::endl;

    for (int i=0; i<8; i++)
    {
        configfile << bomber[i].get_skin()		<< std::endl;
        configfile << bomber[i].get_name()              << std::endl;
        configfile << bomber[i].get_team()		<< std::endl;
        configfile << bomber[i].get_controller()	<< std::endl;
        configfile << bomber[i].is_enabled()		<< std::endl;
        configfile << bomber[i].get_highlight_maptile()	<< std::endl;
    }

    configfile << last_server << std::endl;

    return true;
}

bool Config::load()
{
    boost::filesystem::ifstream configfile(path / filename);

    if (configfile.fail())
    {
        configfile.close();
        save(true);
        return false;
    }

    int version;
    configfile >> version;

    if (version != CURRENT_CONFIGFILE_VERSION)
    {
        configfile.close();
        save(true);
        return false;
    }

    configfile >> max_bombs;
    configfile >> max_power;
    configfile >> max_skateboards;

    configfile >> start_bombs;
    configfile >> start_power;
    configfile >> start_skateboards;
    configfile >> start_kick;
    configfile >> start_glove;
    configfile >> start_map;

    configfile >> points_to_win;
    configfile >> round_time;
    configfile >> theme;
    configfile >> music;

    configfile >> kids_mode;
    configfile >> corpse_parts;
    configfile >> shaky_explosions;
    configfile >> random_positions;
    configfile >> random_map_order;

    configfile >> bombs;
    configfile >> power;
    configfile >> skateboards;
    configfile >> kick;
    configfile >> glove;

    configfile >> joint;
    configfile >> viagra;
    configfile >> koks;

    configfile >> bomb_countdown;
    configfile >> bomb_delay;
    configfile >> bomb_speed;


    for (int i=0; i<8; i++)
    {
        configfile >> version;
        bomber[i].set_skin(version);

        std::string player_name;
        configfile.ignore(1, '\n');
        getline(configfile, player_name);
        bomber[i].set_name(player_name);

        configfile >> version;
        bomber[i].set_team(version);

        configfile >> version;
        bomber[i].set_controller(version);

        configfile >> version;
        bomber[i].set_enabled(version != 0);

        configfile >> version;
        bomber[i].set_highlight_maptile(version != 0);
    }

    std::string server_name;
    configfile >> server_name;
    set_last_server(server_name);

    return true;
}

void Config::set_last_server(std::string server_name)
{
    last_server = server_name;
}

std::string Config::get_last_server()
{
    return last_server;
}


void BomberConfig::set_local(bool _local)
{
    local_client=_local;
}


bool BomberConfig::is_local()
{
    return local_client;
}

bool BomberConfig::is_server_bomber()
{
    return server_bomber;
}

void BomberConfig::set_server_bomber(bool from_server)
{
    server_bomber = from_server;
}
