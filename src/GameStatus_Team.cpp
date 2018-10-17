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

#include "GameStatus_Team.h"

#include <cmath>

#include <boost/format.hpp>

#include "ClanBomber.h"
#include "GameObject.h"
#include "GameConfig.h"
#include "Timer.h"
#include "Bomber.h"
#include "Map.h"
#include "Utils.h"

static GameStatus_Team* game_status_team = NULL;
static SimpleTimer demo_mode_timer;

const char* GameStatus_Team::team_names[4] =
{
    N_("Blood Team"),
    N_("Gold Team"),
    N_("Water Team"),
    N_("Leaf Team")
};

GameStatus_Team::team_color GameStatus_Team::team_colors[4] = {	{255, 128, 128},
    {255, 255, 0},
    {128, 128, 255},
    {128, 255, 128}
};

GameStatus_Team::GameStatus_Team(ClanBomberApplication* _app) : GameStatus( _app)
{
    game_status_team = this;
}

void GameStatus_Team::show()
{
    demo_mode_timer.reset();
    if (end_of_game)
    {
        Resources::Gamestatus_klatsch()->play();
    }

    bool space_pressed = false;

    while (!space_pressed)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
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
        draw();
    }
    
    CB_FillRect(0, 0, 800, 600, 0, 0, 0);
    CB_Flip();
}

GameStatus_Team::~GameStatus_Team()
{
}

void GameStatus_Team::analyze_game()
{
    team_count[0] = team_count[1] = team_count[2] = team_count[3] = 0;
    team_points[0] = team_points[1] = team_points[2] = team_points[3] = 0;
    end_of_game = false;
    winner = NULL;
    for(auto bomber_object_iter : game_status_team->app->bomber_objects)
    {
        if ((bomber_object_iter->get_team() == 0) && ! bomber_object_iter->dead)
        {
            winner = bomber_object_iter;
            if (bomber_object_iter->get_points() == Config::get_points_to_win())
            {
                end_of_game = true;
            }
        }
        if (bomber_object_iter->get_team() != 0)
        {
            if (!bomber_object_iter->dead)
            {
                winner = bomber_object_iter;
            }
            team_count[bomber_object_iter->get_team()-1] += 1;
            team_points[bomber_object_iter->get_team()-1] += bomber_object_iter->get_points();
        }
    }
    for (int i=0; i<4; i++)
    {
        if ((team_count[i] > 0) && (team_points[i] >= team_count[i]*Config::get_points_to_win()))
        {
            end_of_game = true;
        }
    }
}

void GameStatus_Team::draw()
{
    std::string temp_string;
    game_status_team->team_count[0] = game_status_team->team_count[1] = game_status_team->team_count[2] = game_status_team->team_count[3] = 0;
    Resources::Gamestatus_background()->blit(0, 0);

    Resources::Font_small()->render(_("PRESS SPACE TO CONTINUE"), 400,
                                            570, cbe::FontAlignment_0topcenter);

    int window_x_offset = 0;
    int window_y_offset = 170;
    for (int team = 0; team<4; team++)
    {
        for (auto bomber_object_iter : game_status_team->app->bomber_objects)
        {
            if (bomber_object_iter->get_team()-1 == team)
            {
                CB_FillRect(window_x_offset, window_y_offset+30+game_status_team->team_count[team]*30, 380, 30, team_colors[team].r, team_colors[team].g, team_colors[team].b, 128);
                static_cast<GameObject*>(bomber_object_iter)->show(window_x_offset,window_y_offset+25+game_status_team->team_count[team]*30, 0.5f);
                temp_string = bomber_object_iter->get_name();
                Resources::Font_small()->render(temp_string, window_x_offset+35,
                                                window_y_offset+35+game_status_team->team_count[team]*30, cbe::FontAlignment_0topleft);
                game_status_team->team_count[team]++;
            }
        }
        if (game_status_team->team_count[team] > 0)
        {
            Resources::Playersetup_teamlamps()->put_screen(window_x_offset,window_y_offset,  team);
            Resources::Font_small()->render(_(team_names[team]), window_x_offset+35, window_y_offset+6, cbe::FontAlignment_0topleft);
            for (int i=0 ; i<(game_status_team->team_points[team]/game_status_team->team_count[team]); i++ )
            {
                Resources::Gamestatus_tools()->put_screen( window_x_offset+180+i*20, window_y_offset+4, 0.5f, 0.5f, 0);
            }
            if (game_status_team->end_of_game && game_status_team->winner->get_team()-1 == team)
            {
                float scalefactor = (float)(std::sin(Timer::get_time()/100.0f)+2.0f);
                Resources::Gamestatus_tools()->put_screen((int)(window_x_offset+200-20*scalefactor), (int)(window_y_offset+70-20*scalefactor),scalefactor,scalefactor, 0 );
            }
            window_x_offset+=400;
            if (window_x_offset == 800)
            {
                window_x_offset=0;
                window_y_offset+=200;
            }
        }
    }

    int i = 0;
    for (auto bomber_object_iter : game_status_team->app->bomber_objects)
    {
        if (bomber_object_iter->get_team() == 0)
        {
            Resources::Font_small()->render(_("Teamless Heroes"), window_x_offset+35, window_y_offset+6, cbe::FontAlignment_0topleft);
            CB_FillRect(window_x_offset, window_y_offset+30+i*30, 380, 30, 140, 140, 140, 128);
            static_cast<GameObject*>(bomber_object_iter)->show(window_x_offset,window_y_offset+25+i*30, 0.5f);
            temp_string = bomber_object_iter->get_name();
            Resources::Font_small()->render(temp_string, window_x_offset+32, window_y_offset+35+i*30, cbe::FontAlignment_0topleft);
            for (int j=0 ; j<bomber_object_iter->get_points(); j++)
            {
                Resources::Gamestatus_tools()->put_screen(window_x_offset+180+j*20, window_y_offset+34+i*30,0.5f, 0.5f, 0  );
            }
            if (game_status_team->end_of_game && game_status_team->winner == bomber_object_iter)
            {
                float scalefactor = (float)(std::sin(Timer::get_time()/100.0f)+2.0f);
                Resources::Gamestatus_tools()->put_screen((int)(window_x_offset+200-20*scalefactor), (int)(window_y_offset+34+i*30-20*scalefactor),scalefactor,scalefactor, 0);
            }
            i++;
        }
    }
    if (!game_status_team->winner)
    {
        Resources::Font_big()->render(_("Draw Game"), 500, 40, cbe::FontAlignment_0topcenter);
    }
    else
    {
        if (game_status_team->winner->get_team() == 0)
        {
            std::string nstr = str(boost::format(_("%s won")) % game_status_team->winner->get_name());
            Resources::Font_big()->render(nstr, 500, 40, cbe::FontAlignment_0topcenter);
        }
        else
        {
            std::string nstr = str(boost::format(_("%s won")) % _(team_names[game_status_team->winner->get_team()-1]));
            Resources::Font_big()->render(nstr, 500, 40, cbe::FontAlignment_0topcenter);
        }
    }
    if (game_status_team->end_of_game)
    {
        Resources::Font_big()->render(_("the Match"), 500, 80, cbe::FontAlignment_0topcenter);
    }
    else
    {
        Resources::Font_small()->render(_("NEXT LEVEL"), 785, 10, cbe::FontAlignment_0topright);
        std::string temp_string = game_status_team->app->map->get_name();
        Resources::Font_small()->render(temp_string, 785, 125, cbe::FontAlignment_0topright);
        game_status_team->app->map->show_preview(790-119,30,0.18f);
    }

    CB_Flip();
}
