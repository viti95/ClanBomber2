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
#include "GameConfig.h"

#include "Credits.h"

#include "Corpse_Part.h"
#include "Timer.h"

#include "Utils.h"

#include <vector>
#include <SDL2/SDL.h>

#define SCROLL_SPEED 400

Credits::Credits( ClanBomberApplication *_app )
{
    app = _app;

    text.push_back(_("ClanBomber Credits"));
    text.push_back("");
    text.push_back("Andreas Hundt");
    text.push_back("Andreeshchev Eugeni");
    text.push_back("Clanner");
    text.push_back("Denis Oliver Kropp");
    text.push_back("Ivar");
    text.push_back("James Andrews");
    text.push_back("Magnus Reftel");
    text.push_back("Martin Pitt");
    text.push_back("Rene Lopez");
    text.push_back("mass");
    text.push_back("non");

    text.push_back("");
    text.push_back(_("ClanBomber Uses"));
    //text.push_back("");
    text.push_back("DejaVu");
    text.push_back("SDL2");
    //text.push_back("SDL2_gfx");
    text.push_back("SDL2_image");
    text.push_back("SDL2_mixer");
    text.push_back("SDL2_ttf");
    text.push_back("");


    text.push_back("");
    text.push_back(_("Thanks to"));
    //text.push_back("");
    text.push_back(_("Everyone supporting this game..."));
    text.push_back(_("... and playing it"));

    yoffset = yoffset_start;
    yoffset_end = -text.size() * text_height + 100;
}

Credits::~Credits()
{

}

void Credits::exec()
{
    bool escape, up, down;
    escape = false;
    up = false;
    down = false;

    float t = 0;

    Timer::reset();

    draw();

    while (1)
    {

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    escape = true;
                    break;
                case SDL_SCANCODE_UP:
                    up = true;
                    break;
                case SDL_SCANCODE_DOWN:
                    down = true;
                    break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    escape = false;
                    break;
                case SDL_SCANCODE_UP:
                    up = false;
                    break;
                case SDL_SCANCODE_DOWN:
                    down = false;
                    break;
                }
                break;
            }
        }

        if (escape)
        {
            for(const auto &iter : app->objects)
            {
                delete iter;
            }
            app->objects.clear();
            return;
        }
        if (up)
        {
            speed = -SCROLL_SPEED;
        }
        else if (down)
        {
            speed = SCROLL_SPEED;
        }
        else
        {
            speed = normal_speed;
        }

        yoffset -= speed * Timer::time_elapsed(true);
        if (yoffset > yoffset_start)
        {
            if (speed != 20 && !Config::get_kids_mode())
            {
                t += Timer::time_elapsed();
                for (; t>0.04f; t-=0.04f)
                {
                	Corpse_Part* cp = new Corpse_Part( fast_random()%800-60, -40, app );
                	cp->fly_to( fast_random()%800-60, 540 );
                	Resources::Splash(fast_random()%2)->play();
                }
            }
            yoffset = yoffset_start;
            speed = normal_speed;// = 20;

            if (!stopped)
            {
                Resources::Credits_stop()->play();
            }
            stopped = true;
        }
        else if (yoffset < yoffset_end)
        {
            if (speed != 40 && !Config::get_kids_mode())
            {
                t += Timer::time_elapsed();
                for (; t>0.04f; t-=0.04f)
                {
                    Corpse_Part* cp = new Corpse_Part( fast_random()%800-60, -40, app );
                    cp->fly_to( fast_random()%800-60, fast_random()%600-60 );
                    Resources::Splash(fast_random()%2)->play();
                }
            }
            yoffset = yoffset_end;

            if (!stopped)
            {
                Resources::Credits_stop()->play();
            }
            stopped = true;
        }
        else
        {
            stopped = false;
        }

        // Let them do their stuff
        for(const auto &object_iter : app->objects)
        {
            object_iter->act();
        }
        // Check if we should delete some
        for(auto object_iter = app->objects.begin();
                object_iter != app->objects.end();)
        {
            if ((*object_iter)->delete_me)
            {
                delete *object_iter;
                object_iter = app->objects.erase(object_iter);
            }else{
				++object_iter;
			}
        }

        draw();
    }
}

void Credits::draw()
{
    Resources::Credits_horst_evil()->blit(0, 0);

    Resources::Game_cb_logo_small()->blit(10, yoffset+10);
    Resources::Game_cb_logo_small()->blit(640, yoffset+10);

    Resources::Intro_fl_logo()->blit(100, yoffset + text.size()*text_height + 130 );

    int i = 0;
    for(const auto &text_iter : text)
    {
        Resources::Font_big()->render(text_iter, 400, yoffset + i*40 + 60,
                                      cbe::FontAlignment_0center);
        i++;
    }

    if (yoffset < -10)
    {
        Resources::Font_big()->render("+", 10, 9, cbe::FontAlignment_0topleft);
        Resources::Font_big()->render("+", 790, 9, cbe::FontAlignment_0topright);
    }

    if (yoffset > yoffset_end + 250)
    {
        Resources::Font_big()->render("-", 10, 560, cbe::FontAlignment_0topleft);
        Resources::Font_big()->render("-", 790, 560, cbe::FontAlignment_0topright);
    }

    draw_objects();

    CB_Flip();
}

void Credits::draw_objects()
{
    (app->objects).sort(game_object_compare);

    for(const auto &draw_list_iter : app->objects)
    {
        draw_list_iter->show();
    }
}
