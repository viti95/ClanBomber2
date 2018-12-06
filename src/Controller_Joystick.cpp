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
#include "Controller_Joystick.h"

#include <SDL2/SDL.h>

Controller_Joystick::Controller_Joystick(int joystick_nr) : Controller()
{
    joystick = SDL_JoystickOpen(joystick_nr);
}

Controller_Joystick::~Controller_Joystick()
{
    SDL_JoystickClose(joystick);
}

void Controller_Joystick::update()
{
    const unsigned int tolerance = 12000;
    if (active && joystick != NULL)
    {
        Sint16 x = SDL_JoystickGetAxis(joystick, 0);
        Sint16 y = SDL_JoystickGetAxis(joystick, 1);

        //X Axis
        if (abs(x) > tolerance)
        {
            if (x > 0)
            {
                left = false;
                right = true;
            }
            else if (x < 0)
            {
                right = false;
                left = true;
            }
            else
            {
                right = false;
                left = false;
            }
        }
        else
        {
            right = false;
            left = false;
        }

        //Y Axis, negatives are up positives are down
        if (abs(y) > tolerance)
        {
            if (y > 0)
            {
                up = false;
                down = true;
            }
            else if (y < 0)
            {
                down = false;
                up = true;
            }
            else
            {
                down = false;
                up = false;
            }
        }
        else
        {
            down = false;
            up = false;
        }

        //Reverse
        if (reverse)
        {
            bool r_right = left;
            left = right;
            right = r_right;

            bool r_down = up;
            up = down;
            down = r_down;
        }

        //Button
        if (SDL_JoystickGetButton(joystick, 0))
        {
            if (bomb_button_down)
            {
                put_bomb = true;
            }
            else
            {
                put_bomb = false;
            }
            bomb_button_down = false;
        }
        else
        {
            put_bomb = false;
            bomb_button_down = true;
        }
    }
}

void Controller_Joystick::reset()
{
    right = false;
    left = false;
    down = false;
    up = false;

    put_bomb = false;
    bomb_button_down = true;

    reverse = false;
}

bool Controller_Joystick::is_left()
{
    return left;
}

bool Controller_Joystick::is_right()
{
    return right;
}

bool Controller_Joystick::is_up()
{
    return up;
}

bool Controller_Joystick::is_down()
{
    return down;
}

bool Controller_Joystick::is_bomb()
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
