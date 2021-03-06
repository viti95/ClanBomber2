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

#ifndef Controller_Keyboard_h
#define Controller_Keyboard_h


#include "Controller.h"
///#include <directfb.h>
#include <SDL2/SDL.h>

class Controller_Keyboard : public Controller
{
public:
    Controller_Keyboard (int keymap_nr);
    virtual ~Controller_Keyboard () {};

    void update();
    void reset();
    bool is_left();
    bool is_right();
    bool is_up();
    bool is_down();
    bool is_bomb();

private:
    SDL_Scancode left_key;
    SDL_Scancode right_key;
    SDL_Scancode up_key;
    SDL_Scancode down_key;
    SDL_Scancode bomb_key;
    bool bomb_key_state;
};
#endif
