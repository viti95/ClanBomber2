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

#ifndef MAPEDITOR_H
#define MAPEDITOR_H

class Map;

/**
  * @author Andreas Hundt
  * @author Denis Oliver Kropp
  */
class MapEditor
{
public:
    MapEditor( ClanBomberApplication *app );
    ~MapEditor();

    void exec();

protected:
    ClanBomberApplication	*app;

    int			current_map;
    int			map_at_top;
    Map			*map;

    int list_width;
    int cur_x, cur_y;
    bool text_editor_mode;

    void draw_editor();
    void show_help();
    void draw_select_screen(bool flip=true);
    void edit_map( int number );
    bool new_map();
    std::string get_new_author();
    void clip_cursor();
};

#endif
