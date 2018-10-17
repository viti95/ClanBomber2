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

#include "ClanBomber.h"
#include "Controller_Keyboard.h"

extern Uint8 *keyboard;

Controller_Keyboard::Controller_Keyboard(int keymap_nr) : Controller()
{
    switch (keymap_nr)
    {
    case 0:
        left_key = SDL_SCANCODE_LEFT;
        right_key = SDL_SCANCODE_RIGHT;
        up_key = SDL_SCANCODE_UP;
        down_key = SDL_SCANCODE_DOWN;
        bomb_key = SDL_SCANCODE_RETURN;
        break;
    case 1:
        left_key = SDL_SCANCODE_A;
        right_key = SDL_SCANCODE_D;
        up_key = SDL_SCANCODE_W;
        down_key = SDL_SCANCODE_S;
        bomb_key = SDL_SCANCODE_TAB;
        break;
    case 2:
        left_key = SDL_SCANCODE_J;
        right_key = SDL_SCANCODE_L;
        up_key = SDL_SCANCODE_I;
        down_key = SDL_SCANCODE_K;
        bomb_key = SDL_SCANCODE_SPACE;
        break;
    case 3:
        left_key = SDL_SCANCODE_KP_4;
        right_key = SDL_SCANCODE_KP_6;
        up_key = SDL_SCANCODE_KP_8;
        down_key = SDL_SCANCODE_KP_5;
        bomb_key = SDL_SCANCODE_KP_0;
        break;
    }
    c_type = KEYMAP_1;
    reset();
}

void Controller_Keyboard::update()
{
    SDL_PumpEvents();

    if ( (keyboard[bomb_key]) && (!bomb_key_state))
    {
        put_bomb = true;
    }
    else
    {
        put_bomb = false;
    }

    bomb_key_state = keyboard[bomb_key];
}

void Controller_Keyboard::reset()
{
    put_bomb = false;
    bomb_key_state = 0;
    reverse = false;
}

bool Controller_Keyboard::is_left()
{
    bool ks;

    if (reverse)
    {
        ks = keyboard[right_key];
    }
    else
    {
        ks = keyboard[left_key];
    }

    return ( ks ) && active;
}

bool Controller_Keyboard::is_right()
{
    bool ks;

    if (reverse)
    {
        ks = keyboard[left_key];
    }
    else
    {
        ks = keyboard[right_key];
    }

    return ks && active;
}

bool Controller_Keyboard::is_up()
{
    bool ks;

    if (reverse)
    {
        ks = keyboard[down_key];
    }
    else
    {
        ks = keyboard[up_key];
    }

    return ks && active;
}

bool Controller_Keyboard::is_down()
{
    bool ks;

    if (reverse)
    {
        ks = keyboard[up_key];
    }
    else
    {
        ks = keyboard[down_key];
    }

    return ks && active;
}

bool Controller_Keyboard::is_bomb()
{
    switch (bomb_mode)
    {
    case NEVER:
        return false;
    case ALWAYS:
        return true;
    default:
        break;
    }
    return put_bomb && active;
}
