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
#include "MapSelector.h"
#include "Utils.h"

#include "GameConfig.h"
#include "Map.h"

MapSelector::MapSelector( ClanBomberApplication *_app )
{
    app = _app;

    map = new Map(app);

    if (Config::get_start_map() > map->get_map_count()-1)
    {
        Config::set_start_map( map->get_map_count() -1 );
    }
    current_map = Config::get_start_map();
    map_at_top = std::min( current_map-5, map->get_map_count()-11 );
    if (map_at_top < 0) map_at_top = 0;

    current_theme = Config::get_theme();

    map->load( current_map );

    list_width = 230;
    for (int i=0; i<map->get_map_count(); i++)
    {
        std::string s = map->map_list[i]->get_name();

        int width;
        Resources::Font_small()->getSize(s, &width, NULL);

        if (width+10 > list_width)
        {
            list_width = width+40;
        }
    }
}

MapSelector::~MapSelector()
{
    delete map;
}

void MapSelector::exec()
{
    draw();

    while (1)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_BACKSPACE:
                case SDL_SCANCODE_ESCAPE:
                    Config::set_start_map( current_map );
                    Config::save();
                    map->save_selection();
                    return;
                    break;
                case SDL_SCANCODE_SPACE:
                {
                    map->map_list[current_map]->toggle();
                    Resources::Menu_clear()->play();
                }
                break;
                case SDL_SCANCODE_A:
                {
                    for (int i=0; i<map->get_map_count(); i++)
                    {
                        map->map_list[i]->enable();
                    }
                    Resources::Menu_clear()->play();
                }
                break;
                case SDL_SCANCODE_S:
                {
                    for (int i=0; i<map->get_map_count(); i++)
                    {
                        map->map_list[i]->disable();
                    }
                    map->map_list[current_map]->enable();
                    Resources::Menu_clear()->play();
                }
                break;
                case SDL_SCANCODE_UP:
                    if (current_map > 0)
                    {
                        current_map--;
                        map_at_top = std::min( current_map-5, map->get_map_count()-11 );
                        if (map_at_top < 0)
                        {
                            map_at_top = 0;
                        }
                        Resources::Menu_break()->play();
                        map->load( current_map );
                    }
                    break;
                case SDL_SCANCODE_DOWN:
                    if (current_map < map->get_map_count()-1)
                    {
                        current_map++;
                        map_at_top = std::min( current_map-5, map->get_map_count()-11 );
                        if (map_at_top < 0)
                        {
                            map_at_top = 0;
                        }
                        Resources::Menu_break()->play();
                        map->load( current_map );
                    }
                    break;
                case SDL_SCANCODE_LEFT:
                    if (current_theme > 0)
                    {
                        current_theme--;
                        Resources::Menu_clear()->play();
                        Config::set_theme( current_theme );
                        map->load( current_map );
                    }
                    break;
                case SDL_SCANCODE_RIGHT:
                    if (current_theme < 5)
                    {
                        current_theme++;
                        Resources::Menu_clear()->play();
                        Config::set_theme( current_theme );
                        map->load( current_map );
                    }
                    break;
                }
            }
        }
        draw();
    }
}

void MapSelector::draw()
{
    Resources::Mapselector_background()->blit(0, 0);

    // show map name
    Resources::Font_big()->render(map->get_name(), 510, 50,
                                  cbe::FontAlignment_0topcenter);

    Resources::Font_small()->render(_("SPACE   TOGGLE CURRENT SELECTION"), 10,
                                    530, cbe::FontAlignment_0topleft);
    Resources::Font_small()->render(_("A   SELECT ALL MAPS"), 10, 550,
                                    cbe::FontAlignment_0topleft);
    Resources::Font_small()->render(_("S   SELECT CURRENT MAP ONLY"), 10, 570,
                                    cbe::FontAlignment_0topleft);

    // show author name
    std::string author(map->get_author());
    std::string byauthor = _("by ") + author;
    Resources::Font_small()->render(byauthor, 510, 90,
                                    cbe::FontAlignment_0topcenter);

    // highlight selected theme
    CB_FillRect(30 + current_theme *120, 423, 120, 30,
                255, 0, 0, 128);

    // show all themes
    const int margin = 30;
    Resources::Font_small()->render(_("Ice"), 60+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    Resources::Font_small()->render(_("Jungle"), 180+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    Resources::Font_small()->render(_("Jungle Arena"), 300+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    Resources::Font_small()->render(_("Strange Kitchen"),	420+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    Resources::Font_small()->render(_("Green Crypt"), 540+margin,	428,
                                    cbe::FontAlignment_0topcenter);
    Resources::Font_small()->render(_("Melting Metal"), 660+margin, 428,
                                    cbe::FontAlignment_0topcenter);

    // show small map preview
    map->show_preview(350, 125, 0.5f);

    // display max players info
    if (map->get_max_players() < Config::get_number_of_players())
    {
        Resources::Mapselector_not_available()->blit(402, 128);
        CB_FillRect(200, 494, 400, 27, 255, 0, 0, 128);
        std::string maxplayers = _("THIS MAP IS ONLY FOR UP TO ") + std::to_string(map->get_max_players()) + _(" PLAYERS");
        Resources::Font_small()->render(maxplayers, 400, 500,
                                        cbe::FontAlignment_0topcenter);
    }
    else
    {
        std::string maxplayers = _("THIS MAP IS ONLY FOR UP TO ") + std::to_string(map->get_max_players()) + _(" PLAYERS");
        Resources::Font_small()->render(maxplayers, 400, 500,
                                        cbe::FontAlignment_0topcenter);
    }

    // map list background
    const int ml_height = 116;
    CB_FillRect(15, ml_height, list_width, 391 - ml_height, 76, 76, 76, 128);

    // highlight selected map name
    CB_FillRect(15, ml_height + (current_map - map_at_top) * 25 ,
                list_width, 141 - ml_height,
                25, 80, 200, 128);

    // show up to ten map names
    for (int i=0; i < std::min(11, map->get_map_count()); i++)
    {
        // enabled ?
        if (map->map_list[i+map_at_top]->is_enabled())
        {
            CB_FillRect(15, ml_height + i * 25, list_width, 141 - ml_height,
                        200, 200, 0, 100);
        }

        // show name
        std::string s(map->map_list[i+map_at_top]->get_name());
        Resources::Font_small()->render(s, 25, 118 + i * 25,
                                        cbe::FontAlignment_0topleft);
    }

    // show scroll indicators
    if (map_at_top > 0)
    {
        Resources::Font_small()->render("+", 128, 90,
                                        cbe::FontAlignment_0topcenter);
    }
    if (map_at_top < map->get_map_count()-11)
    {
        Resources::Font_small()->render("-", 128, 391,
                                        cbe::FontAlignment_0topcenter);
    }

    CB_Flip();
}
