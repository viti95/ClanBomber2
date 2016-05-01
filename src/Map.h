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

#ifndef Map_h
#define Map_h

#include "MapEntry.h"

#include <vector>

class ClanBomberApplication;
class MapTile;

class Map
{
public:
    Map( ClanBomberApplication* _app );
    ~Map();

    bool add_bomb( int bx, int by, int bpower, Bomber *bomber, int bomber_x=0, int bomber_y=0 );
    void clear();
    void load(int map_nr );
    void load_by_data_checksum( unsigned int checksum );
    void load_random_valid();
    void load_next_valid(int map_nr = -1);
    void load_previous_valid(int map_nr = -1);
    void show();
    void show_preview( int x, int y, float factor );
    void show_random_boxes();
    void show_start_positions();
    void refresh_holes();
    void maptilize_objects();
    void act();
    void shake( int _shake );
    void save_selection();
    void load_selection();
    MapTile* get_passable();
    MapTile* get_random();

	inline MapTile* get_maptile_fast(int x, int y)
	{
		return maptiles[x][y];
	}

    inline MapTile* get_maptile(int x, int y)
    {
    	return ((unsigned)(x) > MAP_WIDTH-1 || (unsigned)(y) > MAP_HEIGHT-1) ? dummy_maptile : maptiles[x][y];
    }
    
    inline MapTile* get_maptile_xy(int x, int y)
	{
		return ((unsigned)(x) >= MAP_WIDTH*MAP_PIXELS || (unsigned)(y) >= MAP_HEIGHT*MAP_PIXELS) ? dummy_maptile : maptiles[x/MAP_PIXELS][y/MAP_PIXELS];
	}

    void set_maptile(int x, int y, MapTile* mt);
    bool is_dummy_maptile(MapTile* mt);

    std::string get_name();
    std::string get_author();
    int	get_max_players();

    CL_Vector get_bomber_pos(int nr);
    void randomize_bomber_positions();

    void reload();
    int get_map_count();
    bool any_valid_map();

    int new_entry( std::string _name );
    int delete_entry( int nr );

    friend std::ostream& operator<<(std::ostream& os, const Map& m);

    // static stuff
    static std::vector<MapEntry*> map_list;
    bool is_received_by_client;

    unsigned int get_current_map_name_checksum();
    int get_map_name_checksums(unsigned int** chesksum_array);
    unsigned int get_current_map_data_checksum();
    int get_map_data_checksums(unsigned int** chesksum_array);
    MapEntry* get_current_map();
        
protected:
    ClanBomberApplication* app;
    int is_shaken;

    MapTile* maptiles[MAP_WIDTH][MAP_HEIGHT];
    MapTile* dummy_maptile; // this is a dummy maptile::wall for "clipping" in get_maptile(int x, int y)
    MapEntry *current_map;
    int maps;

    void enum_maps();
    void sort_maps();
    void spin_up();
    void spin_down();
    void spin_to(int nr);
};

#endif








