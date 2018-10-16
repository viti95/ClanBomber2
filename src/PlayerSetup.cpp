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
#include "PlayerSetup.h"

#include "GameConfig.h"
#include "Timer.h"
#include "Controller.h"

#include <SDL2/SDL_ttf.h>
#include "Utils.h"

#include <sstream>

PlayerSetup::PlayerSetup(ClanBomberApplication* _app)
{
    app = _app;
    cur_row = 0;
    cur_col = 0;
}

PlayerSetup::~PlayerSetup()
{
}

void PlayerSetup::exec()
{
    draw();
    CB_Flip();

    ///DFBInputEvent

    SDL_Event event;

    ///keybuffer->Reset(keybuffer);

    while (1)
    {
        //keybuffer->WaitForEvent( keybuffer );
        SDL_PumpEvents();
        if(SDL_WaitEvent(&event))
        {
            if(event.type != SDL_KEYDOWN)
            {
                continue;
            }
        }

        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_BACKSPACE:
        case SDL_SCANCODE_ESCAPE:
            return;
            break;
        case SDL_SCANCODE_RETURN:
            Resources::Menu_clear()->play();
            handle_enter();
            Config::save();
            break;
        case SDL_SCANCODE_SPACE:
            if (Config::bomber[cur_row].is_enabled())
            {
                Config::bomber[cur_row].disable();
            }
            else
            {
                Config::bomber[cur_row].enable();
            }
            Config::save();
            Resources::Menu_clear()->play();
            break;
        case SDL_SCANCODE_H:
            Config::bomber[cur_row].set_highlight_maptile(!Config::bomber[cur_row].get_highlight_maptile());
            Config::save();
            Resources::Menu_clear()->play();
            break;
        case SDL_SCANCODE_DOWN:
            cur_row += (cur_row<7) ? 1 : 0;
            Resources::Menu_break()->play();
            break;
        case SDL_SCANCODE_UP:
            cur_row -= (cur_row>0) ? 1 : 0;
            Resources::Menu_break()->play();
            break;
        case SDL_SCANCODE_LEFT:
            cur_col -= (cur_col>0) ? 1 : 0;
            Resources::Menu_break()->play();
            break;
        case SDL_SCANCODE_RIGHT:
            cur_col += (cur_col<3) ? 1 : 0;
            Resources::Menu_break()->play();
            break;
        }

        draw();
        CB_Flip();
    }
}

void PlayerSetup::draw(bool fick)
{
    int cg[5] = { 17, 95, 340, 500, 720 };

    Resources::Playersetup_background()->blit(0, 0);

    Resources::Font_small()->render(_("SKIN"), 55, 40,
                                    cbe::FontAlignment_0center);
    Resources::Font_small()->render(_("NAME"), 130, 40,
                                    cbe::FontAlignment_0center);
    Resources::Font_small()->render(_("TEAM"), 418, 40,
                                    cbe::FontAlignment_0center);
    Resources::Font_small()->render(_("Controller"), 612, 40,
                                    cbe::FontAlignment_0center);
    Resources::Font_small()->render(_("SPACE ENABLES OR DISABLES A PLAYER, "
                                      "H TOGGLES HIGHLIGHTING"), 12, 580,
                                    cbe::FontAlignment_0topleft);

    CB_FillRect(0, 70+cur_row*63, 800, 63, 55, 110, 220, 70);

    if (!fick)
    {
        CB_FillRect(cg[cur_col], 70+cur_row*63, cg[cur_col+1] - cg[cur_col], 63, 40, 40, 240, 120);
    }

    for (int i=0; i<8; i++)
    {
        if (Config::bomber[i].get_highlight_maptile())
        {
            CB_FillRect(34, 90+i*63, 40, 40, 0x00, 0x00, 0xFF, 50);
        }

        Resources::Bombers(Config::bomber[i].get_skin())->put_screen(35, 130+i*63 - Resources::Bombers(Config::bomber[i].get_skin())->get_height(), 0);

        cbe::Font *font = Resources::Font_small();
        if (!fick || cur_row != i)
        {
            font = Resources::Font_big();
        }
        font->render(Config::bomber[i].get_name(), 130, 90 + i*63,
                     cbe::FontAlignment_0topleft);

        Resources::Playersetup_teams()->put_screen( 360, 72 + i*63, Config::bomber[i].get_team() );

        switch ( Config::bomber[i].get_controller() )
        {
        case Controller::AI:
            Resources::Playersetup_controls()->put_screen( 550, 72 + i*63,0);
            Resources::Font_small()->render("DOK", 650,
                                            102 + i*63,
                                            cbe::FontAlignment_0topright);
            break;
        case Controller::AI_mass:
            Resources::Playersetup_controls()->put_screen( 550, 72 + i*63,0);
            Resources::Font_small()->render("MASS", 650,
                                            102 + i*63,
                                            cbe::FontAlignment_0topright);
            break;
        case Controller::KEYMAP_1:
        case Controller::KEYMAP_2:
        case Controller::KEYMAP_3:
            Resources::Playersetup_controls()->put_screen( 550, 72 + i*63,Config::bomber[i].get_controller()-Controller::KEYMAP_1+1);
            break;
        case Controller::RCMOUSE:
            Resources::Font_small()->render("RCMOUSE", 612, 110 + i*63,
                                            cbe::FontAlignment_0center);
            break;
        case Controller::JOYSTICK_1:
        case Controller::JOYSTICK_2:
        case Controller::JOYSTICK_3:
        case Controller::JOYSTICK_4:
        case Controller::JOYSTICK_5:
        case Controller::JOYSTICK_6:
        case Controller::JOYSTICK_7:
            Resources::Playersetup_controls()->put_screen( 550, 72 + i*63,4);
            std::stringstream number;
            number << Config::bomber[i].get_controller()-Controller::JOYSTICK_1+1;
            Resources::Font_small()->render(number.str(),
                                            620,
                                            70 + i*63,
                                            cbe::FontAlignment_0topleft);
            // TODO FIX THIS
            /*Resources::Font_small()->render(SDL_JoystickName(Config::bomber[i].get_controller() - Controller::JOYSTICK_1),
                                            500,
                                            132 + i*63);*/
            break;
        }

        if (!Config::bomber[i].is_enabled())
        {
            CB_FillRect(0, 70+i*63, 800, 63, 0, 0, 0, 170);
        }
    }
}

void PlayerSetup::handle_enter()
{
    switch (cur_col)
    {
    case 0:
        Config::bomber[cur_row].set_skin( Config::bomber[cur_row].get_skin()+1 );
        break;
    case 1:
        enter_name();
        Resources::Menu_clear()->play();
        break;
    case 2:
        Config::bomber[cur_row].set_team( Config::bomber[cur_row].get_team()+1 );
        break;
    case 3:
        Config::bomber[cur_row].set_controller( Config::bomber[cur_row].get_controller()+1 );
        break;
    }
}

void PlayerSetup::enter_name()
{
    float alpha = 0;

    std::string new_string = Config::bomber[cur_row].get_name();

    Timer::reset();

    while (1)
    {
        alpha += Timer::time_elapsed(true);
        while (alpha > 0.5f)
        {
            alpha -= 0.5f;
        }
        draw(true);
        CB_FillRect(95, 70+cur_row*63, 245, 63, 50, 220, 128, (alpha>0.25f) ? (Uint8)((0.5f-alpha)*3.0f*255) : (Uint8)(alpha*3.0f*255));
        Resources::Font_big()->render(new_string, 130, 90 + cur_row,
                                      cbe::FontAlignment_0topleft);

        CB_Flip();


        if (CB_EnterText(new_string))
        {
            Config::bomber[cur_row].set_name( new_string );
            return;
        }
    }
}
