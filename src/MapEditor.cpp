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

#include <unistd.h>

#include "ClanBomber.h"
#include "MapEditor.h"
#include "Utils.h"
#include <boost/format.hpp>

#include "GameConfig.h"
#include "Map.h"

MapEditor::MapEditor( ClanBomberApplication *_app )
{
    app = _app;

    map = new Map(app);

    current_map = 0;
    map_at_top = std::min( current_map-8, map->get_map_count()-16 );
    if (map_at_top < 0)
    {
        map_at_top = 0;
    }

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

    cur_x = 0;
    cur_y = 0;
    text_editor_mode = false;
}

MapEditor::~MapEditor()
{
    delete map;
}

void MapEditor::exec()
{
    draw_select_screen();

    while (1)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_BACKSPACE:
                case SDL_SCANCODE_ESCAPE:
                    return;
                    break;
                case SDL_SCANCODE_RETURN:
                    if (map->map_list[current_map]->is_writable())
                    {
                        Resources::Menu_clear()->play();
                        edit_map( current_map );
                    }
                    break;
                case SDL_SCANCODE_N:
                    Resources::Menu_clear()->play();
                    if ( new_map() )
                    {
                        edit_map( current_map );
                    }
                    break;
                case SDL_SCANCODE_D:
                    if (map->map_list[current_map]->is_writable())
                    {
                        Resources::Menu_clear()->play();
                        current_map = map->delete_entry( current_map );
                        map_at_top = std::min( current_map-8, map->get_map_count()-16 );
                    }
                    break;
                case SDL_SCANCODE_UP:
                    if (current_map > 0)
                    {
                        current_map--;
                        map_at_top = std::min( current_map-8, map->get_map_count()-16 );
                        if (map_at_top < 0)
                        {
                            map_at_top = 0;
                        }
                        Resources::Menu_break()->play();
                    }
                    break;
                case SDL_SCANCODE_DOWN:
                    if (current_map < map->get_map_count()-1)
                    {
                        current_map++;
                        map_at_top = std::min( current_map-8, map->get_map_count()-16 );
                        if (map_at_top < 0)
                        {
                            map_at_top = 0;
                        }
                        Resources::Menu_break()->play();
                    }
                    break;
                }
            }
            draw_select_screen();
        }
    }
}

void MapEditor::draw_select_screen(bool flip)
{
    Resources::MapEditor_background()->blit(0, 0);

    const int ml_height = 116;
    // map list background
    CB_FillRect(45, ml_height, list_width, 400, 75, 75, 75, 128);

    // highlight selected map name
    CB_FillRect(45, ml_height + (current_map - map_at_top) * 25, list_width, 25,
                25, 100, 200, 128);

    // show up to twenty map names
    for (int i=0; i < std::min(16, map->get_map_count()); i++)
    {
        // writable ?
        if (!map->map_list[i+map_at_top]->is_writable())
        {
            CB_FillRect(45, ml_height + i * 25, list_width, 25, 255, 25, 25, 100);
        }

        // show name
        std::string s(map->map_list[i+map_at_top]->get_name());
        //s.to_upper();
        //primary->SetColor( primary, 0xFF, 0xFF, 0xFF, 0xFF );
        //primary->DrawString( primary, s, -1, 60, 118+i*25, DSTF_TOPLEFT );
        Resources::Font_small()->render(s, 60, 118 + i * 25,
                                        cbe::FontAlignment_0topleft);
    }

    // show scroll indicators
    if (map_at_top > 0)
    {
        //primary->DrawString( primary, "+", -1, 165, 85, DSTF_TOPCENTER );
        Resources::Font_big()->render("+", 165, 85, cbe::FontAlignment_0topcenter);
    }
    if (map_at_top < map->get_map_count()-16)
    {
        //primary->DrawString( primary, "-", -1, 165, 516, DSTF_TOPCENTER );
        Resources::Font_big()->render("-", 165, 516, cbe::FontAlignment_0topcenter);
    }

    //primary->DrawString( primary, "Select a map to edit and press Enter", -1, 520, 150, DSTF_TOPCENTER );
    Resources::Font_big()->render(_("Select a map to edit and press Enter"), 520,
                                  150, cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Maps marked red are readonly", -1, 520, 190, DSTF_TOPCENTER );
    Resources::Font_big()->render(_("Maps marked red are readonly"), 520, 190,
                                  cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Press N to create a new map", -1, 520, 250, DSTF_TOPCENTER );
    Resources::Font_big()->render(_("Press N to create a new map"), 520, 250,
                                  cbe::FontAlignment_0topcenter);
    //primary->DrawString( primary, "Press D to delete a map", -1, 520, 290, DSTF_TOPCENTER );
    Resources::Font_big()->render(_("Press D to delete a map"), 520, 290,
                                  cbe::FontAlignment_0topcenter);
    if(flip)
    {
        CB_Flip();
    }
}

bool MapEditor::new_map()
{
    std::string new_string;

    while (1)
    {
        draw_select_screen(false);

        CB_FillRect(200, 300, 400, 100, 0, 0, 0, 128);

        //primary->SetColor( primary, 0xFF, 0xFF, 0xFF, 0xFF );
        //primary->DrawString( primary, "Name:", -1, 230, 330, DSTF_TOPLEFT );
        std::string name = str(boost::format(_("Name: %1$s")) % new_string);
        Resources::Font_big()->render(name, 230, 330, cbe::FontAlignment_0topleft);

        //primary->SetFont( primary, Resources::Font_small() );
        //primary->DrawString( primary, "PRESS ESC TO ABORT", -1, 400, 380, DSTF_TOPCENTER );
        Resources::Font_small()->render(_("PRESS ESC TO ABORT"), 400, 380,
                                        cbe::FontAlignment_0topcenter);

        CB_Flip();

        switch(CB_EnterText( new_string ))
        {
        case 1:
            if (new_string.length())
            {
                current_map = map->new_entry( new_string );
                map_at_top = std::min( current_map-8, map->get_map_count()-16 );
                return true;
            }
            return false;
        case -1:
            return false;
        }
    }
    // impossible to reach this
}

void MapEditor::edit_map( int number )
{
    MapEntry *entry = map->map_list[number];
    map->load( number );

    draw_editor();
    CB_Flip();

    while (1)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    entry->write_back();
                    return;
                    break;
                case SDL_SCANCODE_F1: // help screen
                    show_help();
                    break;
                case SDL_SCANCODE_F2: // editor_mode
                    text_editor_mode = !text_editor_mode;
                    Resources::Menu_clear()->play();
                    break;
                default:
                    break;
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                bool maptile_set = true;

                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_BACKSPACE:
                    maptile_set = false;
                    Resources::Menu_clear()->play();
                    cur_x--;
                    clip_cursor();
                    entry->set_data(cur_x,cur_y,'-');
                    map->reload();
                    break;
                case SDL_SCANCODE_PAGEUP: // increase number of players
                    maptile_set = false;
                    Resources::Menu_clear()->play();
                    entry->set_max_players(entry->get_max_players()+1);
                    map->reload();
                    break;
                case SDL_SCANCODE_PAGEDOWN: // decrease number of players
                    maptile_set = false;
                    Resources::Menu_clear()->play();
                    entry->set_max_players(entry->get_max_players()-1);
                    map->reload();
                    break;
                case SDL_SCANCODE_SPACE: // none
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'-');
                    map->reload();
                    break;
                case SDL_SCANCODE_A: // author
                    maptile_set = false;
                    Resources::Menu_break()->play();
                    entry->set_author( get_new_author() );
                    break;
                case SDL_SCANCODE_G: // ground
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,' ');
                    map->reload();
                    break;
                case SDL_SCANCODE_W: // wall
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'*');
                    map->reload();
                    break;
                case SDL_SCANCODE_B: // box
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'+');
                    map->reload();
                    break;
                case SDL_SCANCODE_R: // random box
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'R');
                    map->reload();
                    break;
                case SDL_SCANCODE_H: // arrow left
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'<');
                    map->reload();
                    break;
                case SDL_SCANCODE_K: // arrow right
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'>');
                    map->reload();
                    break;
                case SDL_SCANCODE_U: // arrow up
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'^');
                    map->reload();
                    break;
                case SDL_SCANCODE_J: // arrow down
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'v');
                    map->reload();
                    break;
                case SDL_SCANCODE_I: // ice
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'S');
                    map->reload();
                    break;
                case SDL_SCANCODE_O: // bomb trap
                    Resources::Menu_clear()->play();
                    entry->set_data(cur_x,cur_y,'o');
                    map->reload();
                    break;
                case SDL_SCANCODE_1: // place first player
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 0);
                    map->reload();
                    break;
                case SDL_SCANCODE_2:
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 1);
                    map->reload();
                    break;
                case SDL_SCANCODE_3:
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 2);
                    map->reload();
                    break;
                case SDL_SCANCODE_4:
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 3);
                    map->reload();
                    break;
                case SDL_SCANCODE_5:
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 4);
                    map->reload();
                    break;
                case SDL_SCANCODE_6:
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 5);
                    map->reload();
                    break;
                case SDL_SCANCODE_7:
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 6);
                    map->reload();
                    break;
                case SDL_SCANCODE_8:
                    Resources::Menu_clear()->play();
                    entry->set_bomber_pos(cur_x,cur_y, 7);
                    map->reload();
                    break;
                case SDL_SCANCODE_LEFT:
                    maptile_set = false;
                    Resources::Menu_break()->play();
                    cur_x--;
                    break;
                case SDL_SCANCODE_RIGHT:
                    maptile_set = false;
                    Resources::Menu_break()->play();
                    cur_x++;
                    break;
                case SDL_SCANCODE_UP:
                    maptile_set = false;
                    Resources::Menu_break()->play();
                    cur_y--;
                    break;
                case SDL_SCANCODE_DOWN:
                    maptile_set = false;
                    Resources::Menu_break()->play();
                    cur_y++;
                    break;
                default:
                    maptile_set = false;
                }

                if (maptile_set && text_editor_mode)
                {
                    cur_x++;
                }

                clip_cursor();
            }

            draw_editor();

            CB_Flip();
        }
    }
}

void MapEditor::draw_editor()
{
    Resources::MapEditor_background()->blit(0, 0);

    // show map
    map->show();
    map->show_random_boxes();
    map->show_start_positions();

    // show cursor
    CB_FillRect(60 + cur_x * 40, 40 + cur_y * 40, 40, 40, 150, 125, 25, 150);

    // show map name
    //primary->DrawString( primary, map->map_list[current_map]->get_name(), -1, 780, 3, DSTF_TOPRIGHT );
    Resources::Font_big()->render(map->map_list[current_map]->get_name(), 780, 3,
                                  cbe::FontAlignment_0topright);

    // huh, what's this? ;)
    //primary->DrawString( primary, "PRESS F1 FOR HELP", -1, 10, 3, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS F1 FOR HELP"), 20, 3,
                                    cbe::FontAlignment_0topleft);

    if (text_editor_mode)
    {
        //primary->DrawString( primary, "TEXT EDITOR MODE", -1, 10, 580, DSTF_TOPLEFT );
        Resources::Font_small()->render(_("TEXT EDITOR MODE"), 20, 580,
                                        cbe::FontAlignment_0topleft);
    }
    else
    {
        //primary->DrawString( primary, "NORMAL EDITOR MODE", -1, 10, 580, DSTF_TOPLEFT );
        Resources::Font_small()->render(_("NORMAL EDITOR MODE"), 20, 580,
                                        cbe::FontAlignment_0topleft);
    }

    //primary->DrawString( primary, CL_String("NUMBER OF PLAYERS   ") + map->map_list[current_map]->get_max_players(), -1, 780, 580, DSTF_TOPRIGHT );
    std::string numplayers = str(boost::format(_("NUMBER OF PLAYERS   %1$s"))
                                 % map->map_list[current_map]->get_max_players()
                                );
    Resources::Font_small()->render(numplayers, 780, 580,
                                    cbe::FontAlignment_0topright);
}

std::string MapEditor::get_new_author()
{
    std::string author(map->map_list[current_map]->get_author());

    while (1)
    {
        draw_editor();

        CB_FillRect(150, 300, 400, 100, 0, 0, 0, 200);

        //primary->SetColor( primary, 0xFF, 0xFF, 0xFF, 0xFF );
        //primary->DrawString( primary, "Author:", -1, 180, 330, DSTF_TOPLEFT );
        //primary->DrawString( primary, author, -1, 330, 330, DSTF_TOPLEFT );
        std::string pauthor = str(boost::format(_("Author: %1$s"))
                                  % map->map_list[current_map]->get_author());
        Resources::Font_big()->render(pauthor, 180, 300,
                                      cbe::FontAlignment_0topleft);

        //primary->SetFont( primary, Resources::Font_small() );
        //primary->DrawString( primary, "PRESS ESC TO ABORT", -1, 400, 380, DSTF_TOPCENTER );
        Resources::Font_small()->render(_("PRESS ESC TO ABORT"), 400, 380,
                                        cbe::FontAlignment_0topcenter);

        CB_Flip();

        switch(CB_EnterText(author))
        {
        case 1:
            if (author.length())
            {
                Resources::Menu_clear()->play();
                return author;
            }
        case -1:
            Resources::Menu_break()->play();
            return map->map_list[current_map]->get_author();
        }
    }
    // cannot reach this!
}

void MapEditor::show_help()
{
    Resources::MapEditor_background()->blit(0, 0);

    Resources::Game_maptiles()->put_screen( 40, 70, Config::get_theme()*4 + 0 );
    //primary->DrawString( primary, "PRESS G FOR A GROUND TILE", -1, 110, 78, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS G FOR A GROUND TILE"), 110, 78,
                                    cbe::FontAlignment_0topleft);

    Resources::Game_maptiles()->put_screen( 40, 110, Config::get_theme()*4 + 1 );
    //primary->DrawString( primary, "PRESS W FOR A WALL", -1, 110, 118, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS W FOR A WALL"), 110, 118,
                                    cbe::FontAlignment_0topleft);

    Resources::Game_maptiles()->put_screen( 40, 150, Config::get_theme()*4 + 2 );
    //primary->DrawString( primary, "PRESS B FOR A BOX", -1, 110, 158, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS B FOR A BOX"), 110, 158,
                                    cbe::FontAlignment_0topleft);

    Resources::Game_maptiles()->put_screen( 40, 190, Config::get_theme()*4 + 2 );
    Resources::Game_maptile_addons()->put_screen( 40, 190, 5 );
    //primary->DrawString( primary, "PRESS R FOR A RANDOM TILE", -1, 110, 198, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS R FOR A RANDOM TILE"), 110, 198,
                                    cbe::FontAlignment_0topleft);

    Resources::Game_maptiles()->put_screen( 40, 230, Config::get_theme()*4 + 0 );
    Resources::Game_maptile_addons()->put_screen( 40, 230, 7 );
    //primary->DrawString( primary, "PRESS O FOR A BOMB TRAP", -1, 110, 238, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS O FOR A BOMB TRAP"),	110, 238,
                                    cbe::FontAlignment_0topleft);

    Resources::Game_maptiles()->put_screen( 40, 270, Config::get_theme()*4 + 0 );
    Resources::Game_maptile_addons()->put_screen( 40, 270, 0 );
    //primary->DrawString( primary, "PRESS I FOR AN ICE TILE", -1, 110, 278, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS I FOR AN ICE TILE"),	110, 278,
                                    cbe::FontAlignment_0topleft);

    Resources::Game_maptiles()->put_screen( 40, 320, Config::get_theme()*4 + 0 );
    Resources::Game_maptile_addons()->put_screen( 40, 320, 6 );
    //primary->DrawString( primary, "PRESS NUMBER KEYS FOR BOMBER POSITIONS", -1, 110, 328, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS NUMBER KEYS FOR BOMBER POSITIONS"),
                                    110, 328, cbe::FontAlignment_0topleft);

    // show arrows
    //primary->DrawString( primary, "THE FOLLOWING KEYS PRODUCE ARROWS", -1, 40, 398, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("THE FOLLOWING KEYS PRODUCE ARROWS"), 40,
                                    398, cbe::FontAlignment_0topleft);

    Resources::Game_maptiles()->put_screen( 150, 460, Config::get_theme()*4 + 0 );
    Resources::Game_maptile_addons()->put_screen( 150, 460, 3 );
    //primary->DrawString( primary, "U", -1, 170, 428, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("U"), 170, 428,
                                    cbe::FontAlignment_0topcenter);

    Resources::Game_maptiles()->put_screen( 110, 500, Config::get_theme()*4 + 0 );
    Resources::Game_maptile_addons()->put_screen( 110, 500, 2 );
    //primary->DrawString( primary, "J", -1, 98, 510, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("J"), 98, 510,
                                    cbe::FontAlignment_0topcenter);

    Resources::Game_maptiles()->put_screen( 150, 500, Config::get_theme()*4 + 0 );
    Resources::Game_maptile_addons()->put_screen( 150, 500, 1 );
    //primary->DrawString( primary, "K", -1, 170, 550, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("K"), 170, 550,
                                    cbe::FontAlignment_0topcenter);

    Resources::Game_maptiles()->put_screen( 190, 500, Config::get_theme()*4 + 0 );
    Resources::Game_maptile_addons()->put_screen( 190, 500, 4 );
    //primary->DrawString( primary, "L", -1, 242, 510, DSTF_TOPCENTER );
    Resources::Font_small()->render(_("L"), 242, 510,
                                    cbe::FontAlignment_0topcenter);

    // misc stuff
    //primary->DrawString( primary, "PRESS SPACE BAR FOR A HOLE", -1, 380, 448, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS SPACE BAR FOR A HOLE"), 380, 448,
                                    cbe::FontAlignment_0topleft);
    //primary->DrawString( primary, "PAGE UP AND DOWN SET NR OF PLAYERS", -1, 380, 468, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PAGE UP AND DOWN SET NR OF PLAYERS"), 380,
                                    468, cbe::FontAlignment_0topleft);
    //primary->DrawString( primary, "PRESS A FOR CHANGING THE AUTHOR", -1, 380, 488, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESS A FOR CHANGING THE AUTHOR"),	380,
                                    488, cbe::FontAlignment_0topleft);
    //primary->DrawString( primary, "PRESSING F2 SWITCHES EDITOR MODE", -1, 380, 508, DSTF_TOPLEFT );
    Resources::Font_small()->render(_("PRESSING F2 SWITCHES EDITOR MODE"), 380,
                                    508, cbe::FontAlignment_0topleft);

    CB_Flip();

    // wait for the "any key"
    CB_WaitForKeypress();
}

void MapEditor::clip_cursor()
{
    if (text_editor_mode)
    {
        if (cur_x <0)
        {
            cur_x = MAP_WIDTH-1;
            cur_y--;
        }
        if (cur_x > MAP_WIDTH-1)
        {
            cur_x=0;
            cur_y++;
        }
    }
    else
    {
        if (cur_x <0)
        {
            cur_x = 0;
        }
        if (cur_x > MAP_WIDTH-1)
        {
            cur_x = MAP_WIDTH-1;
        }
    }
    if (cur_y <0)
    {
        cur_y = 0;
    }
    if (cur_y > MAP_HEIGHT-1)
    {
        cur_y = MAP_HEIGHT-1;
    }
}
