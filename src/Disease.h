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

#ifndef DISEASE_H
#define DISEASE_H


class Bomber;
class Extra;

/**
 * Base for all diseases.
 * A disease is an negative effect given to a Bomber.
 * The disease are set manually before game start or are get by "touching" a
 * Extra of certain type while in game.
 * @todo Comment this and its derived right
 * @author Andreas Hundt
 * @author Denis Oliver Kropp
 * @see Extra
 */
class Disease
{
public:
    /**
     * Creates a Disease and attach it to a Bomber.
     * Creates a Disease and attach it to a {@link Bomber}.
     * @param _bomber Bomber which will get the disease.
     */
    Disease(Bomber* _bomber);
    /**
     *
     */
    virtual ~Disease();
    /**
     *
     */
    void act();
    /**
     *
     */
    float get_countdown() const;
    /**
     * Start the disease effects.
     */
    virtual void start()=0;
    /**
     * Stop the disease effects.
     */
    virtual void stop()=0;
    /**
     *
     */
    virtual Extra* spawn_extra(int _x, int _y)=0;
    /**
     *
     */
    virtual Disease* spawn(Bomber* _bomber)=0;
    /**
     *
     */
    typedef enum
    {
        /**
         * Frozen.
         * Represents the Frozen disease.
         */
        FROZEN = -4,
        /**
         * Put Bomb
         */
        PUTBOMB = -3,
        /**
         * Stoned
         */
        STONED = -2,
        /**
         * Fast
         */
        FAST = -1,
    } DiseaseType;
    /**
     *
     */
    virtual DiseaseType get_DiseaseType()=0;
    /**
     *
     */
    static Disease* create(DiseaseType type, Bomber* _bomber);
protected:
    Bomber* bomber;
    float countdown;
};

#endif
