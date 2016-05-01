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

#ifndef GAMESTATUS_TEAM_H
#define GAMESTATUS_TEAM_H

#include "GameStatus.h"

class ClanBomberApplication;

class GameStatus_Team :public GameStatus
{
public:
    GameStatus_Team(ClanBomberApplication* _app);
    void show();
    void	analyze_game();
    ~GameStatus_Team();
    static void draw();
protected:
    int team_count[4];
    int team_points[4];

    typedef struct
    {
        unsigned char r,g,b;
    } team_color;

    static team_color team_colors[4];

    static const char* team_names[4];
};

#endif
