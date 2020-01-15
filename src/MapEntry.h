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

#ifndef MAPENTRY_H
#define MAPENTRY_H

#define MAP_WIDTH	17
#define MAP_HEIGHT	13

#define MAP_PIXELS	40

#include <string>

#include <filesystem>

#include "UtilsCL_Vector.h"

class MapEntry
{
public:
    MapEntry(std::filesystem::path _path, std::filesystem::path _filename);
    MapEntry(std::string _name, bool write );
    ~MapEntry();

    CL_Vector	get_bomber_pos(int nr);
    int			get_max_players();
    std::string	get_name();
    std::string	get_author();
    bool		is_enabled();
    bool		is_writable();
    bool		toggle();
    void		enable();
    void		disable();

    int	get_data(int x, int y);
    bool	is_random(int x, int y);
    void	set_data(int x, int y, char data);
    void	set_bomber_pos(int x, int y, int number);
    void	set_max_players(int nr);
    void	set_author(std::string _author);
    void	set_name(std::string _name);
    void	reset_filename(char* client_name);
    void	write_back();
    void 	clear();
    void	reload();
    void	read_bomber_positions();
    void	delete_file();
    void	randomize_bomber_positions();

    unsigned int get_name_checksum(const char* name);
    unsigned int get_data_checksum();
protected:
    std::filesystem::path path;
    std::filesystem::path filename;
    std::string	map_name;
    std::string	author;
    bool		enabled;
    bool		writable;
    int			max_players;
    CL_Vector	bomber_pos[8];
    char		map_data[MAP_WIDTH][MAP_HEIGHT];
};

#endif


