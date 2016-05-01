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
#include "Disease_Frozen.h"
#include "Extra_Viagra.h"

#include "Bomber.h"
#include "Controller.h"

Disease_Frozen::Disease_Frozen(Bomber* _bomber) : Disease (_bomber)
{
    start();
}

Disease_Frozen::~Disease_Frozen()
{
}

void Disease_Frozen::start()
{
    countdown = 3.0f;
    bomber->controller->deactivate();
}

void Disease_Frozen::stop()
{
    bomber->controller->activate();
}

Disease* Disease_Frozen::spawn(Bomber* _bomber)
{
    return new Disease_Frozen(_bomber);
}

Extra* Disease_Frozen::spawn_extra( int _x, int _y)
{
    return new Extra_Viagra (_x,_y, bomber->app);
}
