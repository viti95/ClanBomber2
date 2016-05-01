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
        ///left_key = DIKI_LEFT;
        left_key = SDLK_LEFT;
        ///right_key = DIKI_RIGHT;
        right_key = SDLK_RIGHT;
        ///up_key = DIKI_UP;
        up_key = SDLK_UP;
        ///down_key = DIKI_DOWN;
        down_key = SDLK_DOWN;
        ///bomb_key = DIKI_ENTER;
        bomb_key = SDLK_RETURN;
        break;
    case 1:
        // left_key = DIKI_A;
// 		right_key = DIKI_D;
// 		up_key = DIKI_W;
// 		down_key = DIKI_S;
// 		bomb_key = DIKI_TAB;
        left_key = SDLK_a;
        right_key = SDLK_d;
        up_key = SDLK_w;
        down_key = SDLK_s;
        bomb_key = SDLK_TAB;
        break;
    case 2:
// 		left_key = DIKI_J;
// 		right_key = DIKI_L;
// 		up_key = DIKI_I;
// 		down_key = DIKI_K;
// 		bomb_key = DIKI_SPACE;
        left_key = SDLK_j;
        right_key = SDLK_l;
        up_key = SDLK_i;
        down_key = SDLK_k;
        bomb_key = SDLK_SPACE;
        break;
    case 3:
// 		left_key = DIKI_4;
// 		right_key = DIKI_6;
// 		up_key = DIKI_8;
// 		down_key = DIKI_5;
// 		bomb_key = DIKI_0;
        left_key = SDLK_KP4;
        right_key = SDLK_KP6;
        up_key = SDLK_KP8;
        down_key = SDLK_KP5;
        bomb_key = SDLK_KP0;
        break;
    }
    c_type = KEYMAP_1;
    reset();
}

void Controller_Keyboard::update()
{
    ///DFBInputDeviceKeyState bk;

    ///keyboard->GetKeyState( keyboard, bomb_key, &bk);
    SDL_PumpEvents();

    //if ( (bk==DIKS_DOWN) && (bomb_key_state==DIKS_UP)) {
    if ( (keyboard[bomb_key]) && (!bomb_key_state))
    {
        put_bomb = true;
    }
    else
    {
        put_bomb = false;
    }
    ///keyboard->GetKeyState( keyboard, bomb_key, &bomb_key_state );
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
    ///DFBInputDeviceKeyState ks;
    bool ks;

    if (reverse)
    {
        ///keyboard->GetKeyState( keyboard, right_key, &ks);
        ks = keyboard[right_key];
    }
    else
    {
        ///keyboard->GetKeyState( keyboard, left_key, &ks);
        ks = keyboard[left_key];
    }
    ///return ( ks==DIKS_DOWN ) && active;
    return ( ks ) && active;
}

bool Controller_Keyboard::is_right()
{
    ///DFBInputDeviceKeyState ks;
    bool ks;

    if (reverse)
    {
        ///keyboard->GetKeyState( keyboard, left_key, &ks);
        ks = keyboard[left_key];
    }
    else
    {
        ///keyboard->GetKeyState( keyboard, right_key, &ks);
        ks = keyboard[right_key];
    }
    ///return ( ks==DIKS_DOWN ) && active;
    return ks && active;
}

bool Controller_Keyboard::is_up()
{
    ///DFBInputDeviceKeyState ks;
    bool ks;

    if (reverse)
    {
        ///keyboard->GetKeyState( keyboard, down_key, &ks);
        ks = keyboard[down_key];
    }
    else
    {
        ///keyboard->GetKeyState( keyboard, up_key, &ks);
        ks = keyboard[up_key];
    }
    ///return ( ks==DIKS_DOWN ) && active;
    return ks && active;
}

bool Controller_Keyboard::is_down()
{
    ///DFBInputDeviceKeyState ks;
    bool ks;

    if (reverse)
    {
        ///keyboard->GetKeyState( keyboard, up_key, &ks);
        ks = keyboard[up_key];
    }
    else
    {
        ///keyboard->GetKeyState( keyboard, down_key, &ks);
        ks = keyboard[down_key];
    }
    ///return ( ks==DIKS_DOWN ) && active;
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
