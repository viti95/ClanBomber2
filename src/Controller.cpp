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
#include "Controller.h"

#include "Controller_AI.h"
#include "Controller_AI_mass.h"
#include "Controller_Keyboard.h"
#include "Controller_Joystick.h"
#include "Controller_RCMouse.h"

#include "Bomber.h"

Controller::Controller()
{
    bomber = NULL;
    active = false;
    reverse = false;
    bomb_mode = NORMAL;
}

void Controller::attach(Bomber* _bomber)
{
    bomber = _bomber;
}

void Controller::activate()
{
    active = true;
}

void Controller::deactivate()
{
    active = false;
}

void Controller::revert()
{
    reverse = !reverse;
}

void Controller::bomb_normal()
{
    bomb_mode = NORMAL;
}

void Controller::bomb_always()
{
    bomb_mode = ALWAYS;
}

/* Keyboard Controller */

Controller* Controller::create( CONTROLLER_TYPE _type )
{
    switch( _type )
    {
    case KEYMAP_1:
        return new Controller_Keyboard(0);
    case KEYMAP_2:
        return new Controller_Keyboard(1);
    case KEYMAP_3:
        return new Controller_Keyboard(2);
    case RCMOUSE:
        return new Controller_RCMouse();
    case JOYSTICK_1:
        return new Controller_Joystick(0);
    case JOYSTICK_2:
        return new Controller_Joystick(1);
    case JOYSTICK_3:
        return new Controller_Joystick(2);
    case JOYSTICK_4:
        return new Controller_Joystick(3);
    case JOYSTICK_5:
        return new Controller_Joystick(4);
    case JOYSTICK_6:
        return new Controller_Joystick(5);
    case JOYSTICK_7:
        return new Controller_Joystick(6);
    case JOYSTICK_8:
        return new Controller_Joystick(7);
    case AI:
        return new Controller_AI();
    case AI_mass:
        return new Controller_AI_mass();
    default:
        ;
    }
    return NULL; // fuck, wrong type!?
}

Controller::CONTROLLER_TYPE Controller::get_type()
{
    return c_type;
}
