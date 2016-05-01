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

#include <stdlib.h>

#include "Disease.h"

#include "Timer.h"

#include "Disease_Frozen.h"
#include "Disease_PutBomb.h"
#include "Disease_Stoned.h"
#include "Disease_Fast.h"

Disease::Disease(Bomber* _bomber)
{
    bomber = _bomber;
    countdown = -1;
}


void Disease::act()
{
    if (countdown != -1)
    {
        countdown-= Timer::time_elapsed();
        if (countdown < 0)
        {
            stop();
            countdown =-1;
        }
    }
}

float Disease::get_countdown() const
{
    return countdown;
}

Disease::~Disease()
{
}

Disease* Disease::create(DiseaseType type, Bomber* _bomber)
{
    switch (type)
    {
    case FROZEN:
        return new Disease_Frozen(_bomber);
    case PUTBOMB:
        return new Disease_PutBomb(_bomber);
    case STONED:
        return new Disease_Stoned(_bomber);
    case FAST:
        return new Disease_Fast(_bomber);
    }
    return NULL;
}
