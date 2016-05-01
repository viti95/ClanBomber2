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

#ifndef TIMER_H
#define TIMER_H

/**
 * Returns time in milliseconds.
 * @author Andreas Hundt
 * @author Denis Oliver Kropp
 */
class Timer
{
public:
    Timer();
    ~Timer();

    static void reset();
    /**
     * Return the time elapse between calls
     * Return the time elapse between calls where update is
     * true or the value of the previous call if it is false.
     * @param update If the time elapsed need to be updated
     * @return Elapsed time in milliseconds.
     */
    static float time_elapsed(bool update=false);
    /**
     * Returns time in milliseconds
     * @return Time in millisecons
     */
    static long get_time();
protected:

    static long begin_time;
    static float elapsed;
};

#endif
