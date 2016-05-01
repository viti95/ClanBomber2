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
#include <boost/format.hpp>

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
        //s.to_upper();

        int width;
        ///Resources::Font_small()->GetStringWidth( Resources::Font_small(), (char*)s.get_string(), -1, &width);
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
                switch (event.key.keysym.sym)
                {
                case SDLK_BACKSPACE:
                case SDLK_ESCAPE:
                    Config::set_start_map( current_map );
                    Config::save();
                    map->save_selection();
                    return;
                    break;
                //case DIKS_OK:
                case SDLK_SPACE:
                {
                    map->map_list[current_map]->toggle();
                    Resources::Menu_clear()->play();
                }
                break;
                case SDLK_a:
                {
                    for (int i=0; i<map->get_map_count(); i++)
                    {
                        map->map_list[i]->enable();
                    }
                    Resources::Menu_clear()->play();
                }
                break;
                case SDLK_s:
                {
                    for (int i=0; i<map->get_map_count(); i++)
                    {
                        map->map_list[i]->disable();
                    }
                    map->map_list[current_map]->enable();
                    Resources::Menu_clear()->play();
                }
                break;
                case SDLK_UP:
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
                case SDLK_DOWN:
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
                case SDLK_LEFT:
                    if (current_theme > 0)
                    {
                        current_theme--;
                        Resources::Menu_clear()->play();
                        Config::set_theme( current_theme );
                        map->load( current_map );
                    }
                    break;
                case SDLK_RIGHT:
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
    //primary->SetColor( primary, 0x30, 0x80, 0xD0, 0xFF );
    //primary->DrawString( primary, map->get_name(), -1, 510, 50, DSTF_TOPCENTER );
    Resources::Font_big()->render(map->get_name(), 510, 50,
                                  cbe::FontAlignment_0topcenter);

    //primary->SetFont( primary, Resources::Font_small() );
    //primary->SetColor( primary, 0xFF, 0xFF, 0xFF, 0xFF );
    //primary->DrawString( primary, "SPACE   TOGGLE CURRENT SELECTION", -1, 10, 530, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("SPACE   TOGGLE CURRENT SELECTION"), 10,
                                    530, cbe::FontAlignment_0topleft);
    //primary->DrawString( primary, "A   SELECT ALL MAPS", -1, 10, 550, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("A   SELECT ALL MAPS"), 10, 550,
                                    cbe::FontAlignment_0topleft);
    //primary->DrawString( primary, "S   SELECT CURRENT MAP ONLY", -1, 10, 570, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("S   SELECT CURRENT MAP ONLY"), 10, 570,
                                    cbe::FontAlignment_0topleft);

    // show author name
    std::string author(map->get_author());
    //author.to_upper();
    std::string byauthor = str(boost::format(_("by %1$s")) % author);
    //primary->DrawString( primary, author, -1, 510, 90, DSTF_TOPCENTER );
    Resources::Font_small()->render(byauthor, 510, 90,
                                    cbe::FontAlignment_0topcenter);

    // highlight selected theme
    CB_FillRect(30 + current_theme *120, 423, 120, 30,
                255, 0, 0, 128);

    //primary->SetColor( primary, 0xFF, 0xFF, 0xFF, 0xFF );
    // show all themes
    const int margin = 30;
    //primary->DrawString( primary, "Ice", -1, 60+30, 428, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("Ice"), 60+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Jungle", -1, 180+30, 428, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("Jungle"), 180+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Jungle Arena", -1, 300+30, 428, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("Jungle Arena"), 300+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Strange Kitchen", -1, 420+30, 428, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("Strange Kitchen"),	420+margin, 428,
                                    cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Green Crypt", -1, 540+30, 428, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("Green Crypt"), 540+margin,	428,
                                    cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Melting Metal", -1, 660+30, 428, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("Melting Metal"), 660+margin, 428,
                                    cbe::FontAlignment_0topcenter);

    // show small map preview
    map->show_preview(350, 125, 0.5f);

    // display max players info
    if (map->get_max_players() < Config::get_number_of_players())
    {
        //primary->Blit( primary, Resources::Mapselector_not_available(), NULL, 402, 128 );
        Resources::Mapselector_not_available()->blit(402, 128);
        //primary->SetColor( primary, 255, 0, 0, 128 );
        //primary->FillRectangle( primary, 200, 494, 400, 27 );
        CB_FillRect(200, 494, 400, 27, 255, 0, 0, 128);
        //primary->SetColor( primary, 0xFF, 0xFF, 0xFF, 0xFF );
        //primary->DrawString( primary, CL_String("THIS MAP IS ONLY FOR UP TO ") + map->get_max_players() + " PLAYERS", -1, 400, 500, DSTF_TOPCENTER );
        std::string maxplayers = str(boost::format(_("THIS MAP IS ONLY FOR UP TO %1$d PLAYERS")) % map->get_max_players());
        Resources::Font_small()->render(maxplayers, 400, 500,
                                        cbe::FontAlignment_0topcenter);
    }
    else
    {
        //primary->DrawString( primary, CL_String("THIS MAP IS FOR UP TO ") + map->get_max_players() + " PLAYERS", -1, 400, 500, DSTF_TOPCENTER );
        std::string maxplayers = str(boost::format(_("THIS MAP IS FOR UP TO %1$d PLAYERS")) % map->get_max_players());
        Resources::Font_small()->render(maxplayers, 400, 500,
                                        cbe::FontAlignment_0topcenter);
    }

    // map list background
    const int ml_height = 116;
    //primary->SetColor( primary, 76, 76, 76, 128 );
    //primary->FillRectangle( primary, 15, 116, list_width, 391-116 );
    CB_FillRect(15, ml_height, list_width, 391 - ml_height, 76, 76, 76, 128);

    // highlight selected map name
    //primary->SetColor( primary, 25, 80, 200, 128 );
    //primary->FillRectangle( primary, 15, 116+(current_map-map_at_top)*25, list_width, 141-116 );
    CB_FillRect(15, ml_height + (current_map - map_at_top) * 25 ,
                list_width, 141 - ml_height,
                25, 80, 200, 128);

    // show up to ten map names
    for (int i=0; i < std::min(11, map->get_map_count()); i++)
    {
        // enabled ?
        if (map->map_list[i+map_at_top]->is_enabled())
        {
            //primary->SetColor( primary, 200, 200, 0, 100 );
            //primary->FillRectangle( primary, 15, 116+i*25, list_width, 141-116 );
            CB_FillRect(15, ml_height + i * 25, list_width, 141 - ml_height,
                        200, 200, 0, 100);
        }

        // show name
        std::string s(map->map_list[i+map_at_top]->get_name());
        //s.to_upper();
        //primary->SetColor( primary, 0xFF, 0xFF, 0xFF, 0xFF );
        //primary->DrawString( primary, s, -1, 25, 118+i*25, DSTF_TOPLEFT );
        Resources::Font_small()->render(s, 25, 118 + i * 25,
                                        cbe::FontAlignment_0topleft);
    }

    // show scroll indicators
    if (map_at_top > 0)
    {
        //primary->DrawString( primary, "+", -1, 128, 90, DSTF_TOPCENTER );
        Resources::Font_small()->render("+", 128, 90,
                                        cbe::FontAlignment_0topcenter);
    }
    if (map_at_top < map->get_map_count()-11)
    {
        //primary->DrawString( primary, "-", -1, 128, 391, DSTF_TOPCENTER );
        Resources::Font_small()->render("-", 128, 391,
                                        cbe::FontAlignment_0topcenter);
    }

    CB_Flip();
}
