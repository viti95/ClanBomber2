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

#include <cmath>
#include <string>

#include <SDL2/SDL.h>

#include "ClanBomber.h"
#include "GameStatus.h"
#include "GameObject.h"
#include "GameConfig.h"
#include "Timer.h"
#include "Bomber.h"
#include "Map.h"
#include "Utils.h"

static GameStatus *game_status = NULL;
static SimpleTimer demo_mode_timer;
static float pokal_scroll_in = 800;

GameStatus::GameStatus (ClanBomberApplication * _app)
{
    app = _app;
    game_status = this;
}

void
GameStatus::show ()
{
    pokal_scroll_in = 800;
    demo_mode_timer.reset ();
    if (end_of_game)
    {
        Resources::Gamestatus_klatsch ()->play ();
    }

    bool space_pressed = false;

    while (!space_pressed)
    {
        SDL_Event event;
        while (SDL_PollEvent (&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_SPACE:
                    space_pressed = true;
                    break;
                default:
                    ;
                }
            }
        }
        draw ();
    }

    CB_FillRect(0, 0, 800, 600, 0, 0, 0);
    CB_Flip ();
}

GameStatus::~GameStatus ()
{
}

void
GameStatus::analyze_game ()
{
    end_of_game = false;
    winner = NULL;

    for (const auto &bomber_object_iter : app->bomber_objects)
    {
        if (!(bomber_object_iter)->dead)
        {
            winner = (bomber_object_iter);
        }
        if ((bomber_object_iter)->get_points () ==
                Config::get_points_to_win ())
        {
            end_of_game = true;
        }
    }
}

bool
GameStatus::get_end_of_game ()
{
    return end_of_game;
}

void
GameStatus::draw ()
{
    Resources::Gamestatus_background ()->blit (0, 0);

    Resources::Font_small ()->render (_("PRESS SPACE TO CONTINUE"), 400,
                                              570,
                                              cbe::FontAlignment_0topcenter);

    if (!game_status->winner)
    {
        Resources::Font_big ()->render (_("Draw Game"), 500, 40,
                                        cbe::FontAlignment_0topcenter);
    }
    else
    {
        std::string nstr = game_status->winner->get_name() + _(" won");
        Resources::Font_big ()->render (nstr, 500, 40,
                                        cbe::FontAlignment_0topcenter);
    }
    int nr = 0;

    for (const auto &bomber_object_iter : game_status->app->bomber_objects)
    {
        nr++;

            (bomber_object_iter)->act ();

        Resources::Font_big ()->render ((bomber_object_iter)->get_name (), 70,
                                        157 + nr * 43,
                                        cbe::FontAlignment_0topleft);
        if ((bomber_object_iter) == game_status->winner)
        {
            if (!game_status->end_of_game)
            {
                GameObject *obj =
                    static_cast < GameObject * >(bomber_object_iter);
                if (obj != NULL)
                {
                    obj->show (5 + (nr % 2) * 20, 150 + nr * 43);
                }
                for (int i = 0; i < (bomber_object_iter)->get_points () - 1;
                        i++)
                {
                    Resources::Gamestatus_tools ()->put_screen (300 + i * 43,
                            150 + nr * 43,
                            0);
                }
                Resources::Gamestatus_tools ()->
                put_screen (std::
                            max (257 +
                                 (bomber_object_iter)->get_points () * 43,
                                 (int) pokal_scroll_in), 150 + nr * 43, 0);
            }
            else
            {
                float scalefactor;
                scalefactor =
                    (float) (sin (Timer::get_time () / 100.0f) + 2.0f) / 2;
                GameObject *obj =
                    static_cast < GameObject * >(bomber_object_iter);
                if (obj != NULL)
                {
                    obj->show ((int) (5 + (800 - pokal_scroll_in) / 3),
                               (int) (150 + nr * 43), scalefactor);
                }
                for (int i = 0; i < (bomber_object_iter)->get_points () - 1;
                        i++)
                {
                    Resources::Gamestatus_tools ()->
                    put_screen ((int) (320 + i * 43 - 20 * scalefactor),
                                (int) (170 + nr * 43 - 20 * scalefactor),
                                scalefactor, scalefactor, 0);
                }
                Resources::Gamestatus_tools ()->
                put_screen (std::
                            max ((int)
                                 (277 +
                                  (bomber_object_iter)->get_points () * 43 -
                                  20 * scalefactor), (int) pokal_scroll_in),
                            (int) (170 + nr * 43 - 20 * scalefactor),
                            scalefactor, scalefactor, 0);
            }
        }
        else
        {
            GameObject *obj = static_cast < GameObject * >(bomber_object_iter);
            if (obj != NULL)
            {
                obj->show (5 + (nr % 2) * 20, 150 + nr * 43);
            }
            for (int i = 0; i < (bomber_object_iter)->get_points (); i++)
            {
                Resources::Gamestatus_tools ()->put_screen (300 + i * 43,
                        150 + nr * 43, 0);
            }
        }
    }
    if (game_status->end_of_game)
    {
        Resources::Font_big ()->render (_("the Match"), 500, 80, cbe::FontAlignment_0topcenter);
    }
    else
    {
        Resources::Font_small ()->render (_("NEXT LEVEL"), 785, 10, cbe::FontAlignment_0topright);

		std::string temp_string = game_status->app->map->get_name ();
		Resources::Font_small ()->render (temp_string, 785, 125, cbe::FontAlignment_0topright);

        game_status->app->map->show_preview (790 - 119, 30, 0.18f);
    }

    CB_Flip ();
    if (pokal_scroll_in > 100)
    {
        if (!game_status->end_of_game)
        {
            pokal_scroll_in -= Timer::time_elapsed (true) * 1000.0f;
        }
        else
        {
            pokal_scroll_in -= Timer::time_elapsed (true) * 333.0f;
        }
    }
}
