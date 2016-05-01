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

#include <stddef.h>
#include <sys/time.h>
#include "Timer.h"

long Timer::begin_time = 0;
float Timer::elapsed = 0;

Timer::Timer()
{
    reset();
}

Timer::~Timer()
{
}

void Timer::reset()
{
    begin_time = Timer::get_time();
    elapsed = 0;
}

long Timer::get_time()
{
    struct timeval tv;

    gettimeofday( &tv, NULL );

    return (tv.tv_sec*1000  +  tv.tv_usec/1000);
}

float Timer::time_elapsed(bool update)
{
    if (update)
    {
        elapsed = (Timer::get_time() - begin_time)/1000.0f;
        begin_time = Timer::get_time();
    }

    return elapsed;
}
