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
#include <stdio.h>

#include "ClanBomber.h"
#include "Menu.h"
#include "Timer.h"

#include "GameConfig.h"

#include "Utils.h"

#include "config.h"

MenuItem::MenuItem(const std::string _text, int _id, int _parent)
{
    text = _text;
    id = _id;
    parent = _parent;
}

bool MenuItem::has_children()
{
    return children.size() > 0;
}

void MenuItem::add_child( MenuItem* child )
{
    children.push_back( child );
}

int MenuItem::get_id() const
{
    return id;
}

int MenuItem::get_parent() const
{
    return parent;
}

std::string MenuItem::get_text()
{
    return text;
}

void MenuItem::set_text(const std::string _text)
{
    text = _text;
}

MenuItem_Value::MenuItem_Value( const std::string _text, int _id, int _parent, int _min, int _max, int _value ) : MenuItem( _text, _id, _parent )
{
    value = _value;
    min = _min;
    max = _max;
    test_value();
}

int MenuItem_Value::get_value()
{
    return value;
}

int MenuItem_Value::get_min()
{
    return min;
}

int MenuItem_Value::get_max()
{
    return max;
}

void MenuItem_Value::set_min( int _min )
{
    min = _min;
    test_value();
}

void MenuItem_Value::set_max( int _max )
{
    max = _max;
    test_value();
}

void MenuItem_Value::set_value( int _value )
{
    value = _value;
    test_value();
}

void MenuItem_Value::inc_value()
{
    value++;
    test_value();
}

void MenuItem_Value::dec_value()
{
    value--;
    test_value();
}

void MenuItem_Value::test_value()
{
    if (value > max)
    {
        value = max;
    }
    if (value < min)
    {
        value = min;
    }
}

std::string *MenuItem_StringList::get_sel_string()
{
    return string_list[value];
}
//********************************+
MenuItem_StringList::MenuItem_StringList( const std::string _text, int _id, int _parent, std::list<std::string*> _string_list, int _value ) : MenuItem( _text, _id, _parent )
{
    set_strings(_string_list);
    value = _value;
    test_value();
}

std::vector<std::string*> MenuItem_StringList::get_strings()
{
    return string_list;
}

void MenuItem_StringList::set_value( int _value )
{
    value = _value;
    test_value();
}

int MenuItem_StringList::get_value()
{
    return value;
}

void MenuItem_StringList::set_strings( std::list<std::string*>  _string_list )
{
    for(const auto &iter : _string_list)
    {
    	string_list.push_back(iter);
    }
}

void MenuItem_StringList::inc_value()
{
    value++;
    test_value();
}

void MenuItem_StringList::dec_value()
{
    value--;
    test_value();
}

void MenuItem_StringList::test_value()
{
    if (value > max)
    {
        value = max;
    }
    if (value < min)
    {
        value = min;
    }
}

// ************************
Menu::Menu( const std::string& name, ClanBomberApplication* _app )
{
    app = _app;
    current_run_id = -1;
    current_selection = 1;

    items.push_back( new MenuItem(name, -1, -2) );

    left_netgame_setup = false;
}

Menu::~Menu()
{
    for (const auto &iter : items)
    {
        delete iter;
    }
}

void Menu::redraw( bool options_menu_hack, int yoffset )
{
    MenuItem* current = get_item_by_id(current_run_id);

    int width;
    int fontheight;

    Resources::Font_big()->getSize(current->get_text(), &width, &fontheight);

    for(const auto &item_iter : current->children)
    {
        int w;

        Resources::Font_big()->getSize(item_iter->get_text(), &w, NULL);
        if (item_iter->get_type() == MenuItem::MT_VALUE)
        {
            w += 50;
        }
        if (item_iter->get_type() == MenuItem::MT_STRING)
        {
            int ww;
            Resources::Font_big()->getSize(item_iter->get_text(), &ww, NULL);
            w += ww;
        }
        if (item_iter->get_type() == MenuItem::MT_STRINGLIST)
        {
            int ww;
            Resources::Font_big()->getSize(*static_cast<MenuItem_StringList*>(item_iter)->get_sel_string(), &ww, NULL);
            w += ww;
        }
        width = std::max( w, width );
    }
    width += 30;

    int height = current->children.size()*(fontheight+10) + fontheight*2;

    int left_border = 400 - width/2;
    int right_border = 400 + width/2;

    int vert = yoffset + 300 - height / 2;

    if (!options_menu_hack)
    {
    	Resources::Titlescreen()->blit(0, 0);
    }

    CB_FillRect(left_border-30, vert-10, right_border+60-left_border, height+50, 0, 0, 0xFF, 135);
    Resources::Font_big()->render(current->get_text(), 400, vert, cbe::FontAlignment_0topcenter);

    vert += fontheight*2;

    int act_draw = 0;
    for(const auto &item_iter : current->children)
    {
        act_draw++;
        if (act_draw == current_selection)
        {
            CB_FillRect(left_border-30, vert-5, right_border+60-left_border, fontheight+10, 0, 30, 170, 170);
        }

        if (item_iter->get_type() == MenuItem::MT_VALUE)
        {
            if ((static_cast<MenuItem_Value*>(item_iter)->get_min() == 0) && (static_cast<MenuItem_Value*>(item_iter)->get_max() == 1))
            {
                Resources::Font_big()
                ->render(static_cast<MenuItem_Value*>(item_iter)->get_value()
                         ? _("Yes") : _("No"), right_border, vert,
                         cbe::FontAlignment_0topright);
            }
            else
            {
                char buf[8];
                sprintf( buf, "%d", static_cast<MenuItem_Value*>(item_iter)->get_value() );
                Resources::Font_big()->render(buf, right_border, vert,
                                              cbe::FontAlignment_0topright);
            }
            Resources::Font_big()->render((item_iter)->get_text(),
                                          left_border, vert,
                                          cbe::FontAlignment_0topleft);
        }
        else if ((item_iter)->get_type() == MenuItem::MT_STRINGLIST)
        {
            Resources::Font_big()
            ->render(*static_cast<MenuItem_StringList*>(item_iter)
                     ->get_sel_string(), right_border, vert,
                     cbe::FontAlignment_0topright);
            Resources::Font_big()->render((item_iter)->get_text(),
                                          left_border, vert,
                                          cbe::FontAlignment_0topleft);
        }
        else
        {
            Resources::Font_big()->render((item_iter)->get_text(), 400, vert,
                                          cbe::FontAlignment_0topcenter);
        }
        vert += fontheight+10;
    }

    Resources::Font_small()->render("V" PACKAGE_VERSION, right_border+20,
                                    vert+20, cbe::FontAlignment_0topright);

    CB_Flip();
}

int Menu::execute(bool options_menu_hack)
{
    bool must_redraw = true;
    SDL_Event event;

    while (1)
    {
        if (must_redraw)
        {
            redraw (options_menu_hack);
            must_redraw = false;
        }

        if (left_netgame_setup)
        {
            left_netgame_setup = false;
            restore_options_values();
            redraw (options_menu_hack);
        }

        if (SDL_WaitEvent(&event))
        {
            if ( event.type != SDL_KEYDOWN)
            {
                continue;
            }
        }

        MenuItem* current  = get_item_by_id(current_run_id);
        MenuItem* menu_sel = 0;

        switch (event.key.keysym.scancode)
        {
        case SDL_SCANCODE_DOWN:
            current_selection++;
            if (current_selection > current->children.size())
            {
                current_selection = 1;
            }

            must_redraw = true;
            Resources::Menu_break()->play();
            break;
        case SDL_SCANCODE_UP:
            current_selection--;
            if (current_selection < 1)
            {
                current_selection = current->children.size();
            }
            must_redraw = true;
            Resources::Menu_break()->play();
            break;
        case SDL_SCANCODE_RIGHT:
            menu_sel = current->children[current_selection-1];
            if (menu_sel->get_type() == MenuItem::MT_VALUE)
            {
                ((MenuItem_Value*)(menu_sel))->inc_value();
                Resources::Menu_break()->play();
                save_common_options (menu_sel->get_id(), options_menu_hack, false);
                return menu_sel->get_id();
            }
            if (menu_sel->get_type() == MenuItem::MT_STRINGLIST)
            {
                ((MenuItem_StringList*)(menu_sel))->inc_value();
                Resources::Menu_break()->play();
                save_common_options (menu_sel->get_id(), options_menu_hack, false);
                return menu_sel->get_id();
            }
            break;
        case SDL_SCANCODE_LEFT:
            menu_sel = current->children[current_selection-1];
            if (menu_sel->get_type() == MenuItem::MT_VALUE)
            {
                ((MenuItem_Value*)(menu_sel))->dec_value();
                Resources::Menu_break()->play();
                save_common_options (menu_sel->get_id(), options_menu_hack, false);
                return menu_sel->get_id();
            }
            if (menu_sel->get_type() == MenuItem::MT_STRINGLIST)
            {
                ((MenuItem_StringList*)(menu_sel))->dec_value();
                Resources::Menu_break()->play();
                save_common_options (menu_sel->get_id(), options_menu_hack, false);
                return menu_sel->get_id();
            }
            break;
        case SDL_SCANCODE_RETURN:
            if (current->children[current_selection-1]->has_children())
            {
                current_run_id = current->children[current_selection-1]->get_id();
                current_selection = 1;
                must_redraw = true;
                Resources::Menu_clear()->play();
            }
            else
            {
                return current->children[current_selection-1]->get_id();
            }
            break;
        case SDL_SCANCODE_ESCAPE:
            if (options_menu_hack && current->get_parent() < 0)
            {
                return -23;
            }

            if (current->get_id() != -1)
            {
                current_run_id = current->get_parent();
                current_selection = 1;
                must_redraw = true;
                Resources::Menu_clear()->play();
            }
            break;
        default:
            break;
        }
    }
    return -1;
}

void Menu::scroll_in()
{
    Timer timer;
    float i = -600;

    Resources::Font_big();
    Resources::Titlescreen(); // load resources, otherwise sound would be played too early

    Resources::Menu_whoosh()->play();

    while (i < 0)
    {
        redraw(false,(int)i);
        i += 600 * timer.time_elapsed(true);
    }
}

void Menu::scroll_out()
{
    Timer timer;
    float i = 0;

    Resources::Menu_whoosh()->play();

    while (i < 600)
    {
        redraw(false,(int)i);
        i += 600 * timer.time_elapsed(true);
    }
}

void Menu::add_item( const std::string& text, int id, int parent )
{
    MenuItem* new_item = new MenuItem( text, id, parent );

    get_item_by_id(parent)->add_child(new_item);
    items.push_back( new_item );
}

void Menu::add_value( const std::string& text, int id, int parent, int min, int max, int value )
{
    MenuItem_Value* new_item = new MenuItem_Value( text, id, parent, min, max, value );

    get_item_by_id(parent)->add_child(new_item);
    items.push_back( new_item );
}

void Menu::add_stringlist( const std::string& text, int id, int parent, std::list<std::string*> string_list, int cur_string )
{
    MenuItem_StringList* new_item = new MenuItem_StringList( text, id, parent, string_list, cur_string );

    get_item_by_id(parent)->add_child(new_item);
    items.push_back( new_item );
}

MenuItem* Menu::get_item_by_id( int id )
{
    for(const auto &item_iter : items)
    {
        if (item_iter->get_id() == id)
        {
            return item_iter;
        }
    }
    return NULL;
}

void Menu::go_to_game_menu(bool server)
{
    if (server)
    {
        current_selection = 2;
    }
    else
    {
        current_selection = 1;
    }
}

void Menu::execute_options_menu_hack()
{
    int current_run_id_backup    = current_run_id;
    int current_selection_backup = current_selection;

    current_run_id    = 1;
    current_selection = 1;

    restore_options_values();

    while (true)
    {
        int result = execute (true);
        if (result == -23)
        {
            break;
        }

        bool save_config = true;

        MenuItem* item = get_item_by_id (result);
        switch (result)
        {
        case CONFIG_START_BOMBS:
                ((MenuItem_Value*)item)->set_value( Config::get_start_bombs() );
            break;
        case CONFIG_START_POWER:
                ((MenuItem_Value*)item)->set_value( Config::get_start_power() );
            break;
        case CONFIG_START_SKATES:
                ((MenuItem_Value*)item)->set_value( Config::get_start_skateboards() );
            break;
        case CONFIG_START_KICK:
                ((MenuItem_Value*)item)->set_value( Config::get_start_kick() );
            break;
        case CONFIG_START_GLOVE:
                ((MenuItem_Value*)item)->set_value( Config::get_start_glove() );
            break;
        case CONFIG_BOMBS:
                ((MenuItem_Value*)item)->set_value( Config::get_bombs() );
             break;
        case CONFIG_POWER:
                ((MenuItem_Value*)item)->set_value( Config::get_power() );
            break;
        case CONFIG_SKATES:
                ((MenuItem_Value*)item)->set_value( Config::get_skateboards() );
            break;
        case CONFIG_KICK:
                ((MenuItem_Value*)item)->set_value( Config::get_kick() );
            break;
        case CONFIG_GLOVE:
                ((MenuItem_Value*)item)->set_value( Config::get_glove() );
            break;
        case CONFIG_JOINT:
                ((MenuItem_Value*)item)->set_value( Config::get_joint() );
            break;
        case CONFIG_VIAGRA:
                ((MenuItem_Value*)item)->set_value( Config::get_viagra() );
            break;
        case CONFIG_KOKS:
                ((MenuItem_Value*)item)->set_value( Config::get_koks() );
            break;
        case CONFIG_MAX_BOMBS:
                ((MenuItem_Value*)item)->set_value( Config::get_max_bombs() );
            break;
        case CONFIG_MAX_POWER:
                ((MenuItem_Value*)item)->set_value( Config::get_max_power() );
            break;
        case CONFIG_MAX_SKATES:
                ((MenuItem_Value*)item)->set_value( Config::get_max_skateboards() );
            break;
        case CONFIG_KIDS_MODE:
            break;
        case CONFIG_CORPSE_PARTS:
            break;
        case CONFIG_SHAKE:
            break;
        case CONFIG_BOMB_COUNTDOWN:
                ((MenuItem_Value*)item)->set_value( Config::get_bomb_countdown() );
            break;
        case CONFIG_BOMB_DELAY:
                ((MenuItem_Value*)item)->set_value( Config::get_bomb_delay() );
            break;
        case CONFIG_BOMB_SPEED:
                ((MenuItem_Value*)item)->set_value( Config::get_bomb_speed() );
            break;
        case CONFIG_RANDOM_POSITIONS_DUPLICATE:
                ((MenuItem_Value*)item)->set_value( Config::get_random_positions() );
            break;
        case CONFIG_RANDOM_MAP_ORDER_DUPLICATE:
                ((MenuItem_Value*)item)->set_value( Config::get_random_map_order() );
            break;
        case CONFIG_POINTS_DUPLICATE:
                ((MenuItem_Value*)item)->set_value( Config::get_points_to_win() );
            break;
        case CONFIG_ROUND_TIME_DUPLICATE:
                ((MenuItem_Value*)item)->set_value( Config::get_round_time() );
            break;
        default:
            save_config = false;
            break;
        }

    }

    current_run_id    = current_run_id_backup;
    current_selection = current_selection_backup;
}

void Menu::backup_options_values()
{
    for(const auto &item_iter : items)
    {
        int id = (item_iter)->get_id();
        MenuItem_Value *val = static_cast<MenuItem_Value*>(item_iter);
        switch (id)
        {
        case CONFIG_START_BOMBS:
            options_values[I_START_BOMBS] = val;
            break;
        case CONFIG_START_POWER:
            options_values[I_START_POWER] = val;
            break;
        case CONFIG_START_SKATES:
            options_values[I_START_SKATES] = val;
            break;
        case CONFIG_START_KICK:
            options_values[I_START_KICK] = val;
            break;
        case CONFIG_START_GLOVE:
            options_values[I_START_GLOVE] = val;
            break;
        case CONFIG_BOMBS:
            options_values[I_BOMBS] = val;
            break;
        case CONFIG_POWER:
            options_values[I_POWER] = val;
            break;
        case CONFIG_SKATES:
            options_values[I_SKATES] = val;
            break;
        case CONFIG_KICK:
            options_values[I_KICK] = val;
            break;
        case CONFIG_GLOVE:
            options_values[I_GLOVE] = val;
            break;
        case CONFIG_JOINT:
            options_values[I_JOINT] = val;
            break;
        case CONFIG_VIAGRA:
            options_values[I_VIAGRA] = val;
            break;
        case CONFIG_KOKS:
            options_values[I_KOKS] = val;
            break;
        case CONFIG_MAX_BOMBS:
            options_values[I_MAX_BOMBS] = val;
            break;
        case CONFIG_MAX_POWER:
            options_values[I_MAX_POWER] = val;
            break;
        case CONFIG_MAX_SKATES:
            options_values[I_MAX_SKATES] = val;
            break;
        case CONFIG_KIDS_MODE:
            options_values[I_KIDS_MODE] = val;
            break;
        case CONFIG_CORPSE_PARTS:
            options_values[I_CORPSE_PARTS] = val;
            break;
        case CONFIG_SHAKE:
            options_values[I_SHAKE] = val;
            break;
        case CONFIG_BOMB_COUNTDOWN:
            options_values[I_BOMB_COUNTDOWN] = val;
            break;
        case CONFIG_BOMB_DELAY:
            options_values[I_BOMB_DELAY] = val;
            break;
        case CONFIG_BOMB_SPEED:
            options_values[I_BOMB_SPEED] = val;
            break;
        case CONFIG_RANDOM_POSITIONS_DUPLICATE:
            options_values[I_RANDOM_POS] = val;
            break;
        case CONFIG_RANDOM_MAP_ORDER_DUPLICATE:
            options_values[I_RANDOM_MAP] = val;
            break;
        case CONFIG_POINTS_DUPLICATE:
            options_values[I_POINTS] = val;
            break;
        case CONFIG_ROUND_TIME_DUPLICATE:
            options_values[I_ROUND_TIME] = val;
            break;
        case CONFIG_RANDOM_POSITIONS:
            options_values[I_RANDOM_POS_NG] = val;
            break;
        case CONFIG_RANDOM_MAP_ORDER:
            options_values[I_RANDOM_MAP_NG] = val;
            break;
        case CONFIG_POINTS:
            options_values[I_POINTS_NG] = val;
            break;
        case CONFIG_ROUND_TIME:
            options_values[I_ROUND_TIME_NG] = val;
            break;
        default:
            break;
        }
    }
}

void Menu::restore_options_values()
{
    options_values[I_START_BOMBS]->set_value (Config::get_start_bombs());
    options_values[I_START_POWER]->set_value (Config::get_start_power());
    options_values[I_START_SKATES]->set_value (Config::get_start_skateboards());
    options_values[I_START_KICK]->set_value (Config::get_start_kick());
    options_values[I_START_GLOVE]->set_value (Config::get_start_glove());
    options_values[I_BOMBS]->set_value (Config::get_bombs());
    options_values[I_POWER]->set_value (Config::get_power());
    options_values[I_SKATES]->set_value (Config::get_skateboards());
    options_values[I_KICK]->set_value (Config::get_kick());
    options_values[I_GLOVE]->set_value (Config::get_glove());
    options_values[I_JOINT]->set_value (Config::get_joint());
    options_values[I_VIAGRA]->set_value (Config::get_viagra());
    options_values[I_KOKS]->set_value (Config::get_koks());
    options_values[I_MAX_BOMBS]->set_value (Config::get_max_bombs());
    options_values[I_MAX_POWER]->set_value (Config::get_max_power());
    options_values[I_MAX_SKATES]->set_value (Config::get_max_skateboards());
    //options_values[I_KIDS_MODE]->set_value (0);
    //options_values[I_CORPSE_PARTS]->set_value (0);
    //options_values[I_SHAKE]->set_value (0);
    options_values[I_BOMB_COUNTDOWN]->set_value (Config::get_bomb_countdown());
    options_values[I_BOMB_DELAY]->set_value (Config::get_bomb_delay());
    options_values[I_BOMB_SPEED]->set_value (Config::get_bomb_speed());
    options_values[I_RANDOM_POS]->set_value (Config::get_random_positions());
    options_values[I_RANDOM_MAP]->set_value (Config::get_random_map_order());
    options_values[I_POINTS]->set_value (Config::get_points_to_win());
    options_values[I_ROUND_TIME]->set_value (Config::get_round_time());
    options_values[I_RANDOM_POS_NG]->set_value (Config::get_random_positions());
    options_values[I_RANDOM_MAP_NG]->set_value (Config::get_random_map_order());
    options_values[I_POINTS_NG]->set_value (Config::get_points_to_win());
    options_values[I_ROUND_TIME_NG]->set_value (Config::get_round_time());
}

void Menu::save_common_options (int id, bool options_menu_hack, bool force_save)
{
    bool save_config = true;

    if (id == CONFIG_RANDOM_POSITIONS)
    {
        options_values[I_RANDOM_POS]->set_value (options_values[I_RANDOM_POS_NG]->get_value());
    }
    else if (id == CONFIG_RANDOM_POSITIONS_DUPLICATE)
    {
        options_values[I_RANDOM_POS_NG]->set_value (options_values[I_RANDOM_POS]->get_value());
    }
    else if (id == CONFIG_RANDOM_MAP_ORDER)
    {
        options_values[I_RANDOM_MAP]->set_value (options_values[I_RANDOM_MAP_NG]->get_value());
    }
    else if (id == CONFIG_RANDOM_MAP_ORDER_DUPLICATE)
    {
        options_values[I_RANDOM_MAP_NG]->set_value (options_values[I_RANDOM_MAP]->get_value());
    }
    else if (id == CONFIG_POINTS)
    {
        options_values[I_POINTS]->set_value (options_values[I_POINTS_NG]->get_value());
    }
    else if (id == CONFIG_POINTS_DUPLICATE)
    {
        options_values[I_POINTS_NG]->set_value (options_values[I_POINTS]->get_value());
    }
    else if (id == CONFIG_ROUND_TIME)
    {
        options_values[I_ROUND_TIME]->set_value (options_values[I_ROUND_TIME_NG]->get_value());
    }
    else if (id == CONFIG_ROUND_TIME_DUPLICATE)
    {
        options_values[I_ROUND_TIME_NG]->set_value (options_values[I_ROUND_TIME]->get_value());
    }
    else
    {
        save_config = false;
    }

    if (force_save || save_config)
    {
        Config::save();
        if (options_menu_hack)
        {
            Config::set_filename ("clanbomber.cfg");
        }
        else
        {
            Config::set_filename ("clanbomber_net.cfg");
        }
        Config::load();
        Config::set_random_positions (options_values[I_RANDOM_POS]->get_value());
        Config::set_random_map_order (options_values[I_RANDOM_MAP]->get_value());
        Config::set_points_to_win (options_values[I_POINTS]->get_value());
        Config::set_round_time (options_values[I_ROUND_TIME]->get_value());
        Config::save();
        if (options_menu_hack)
        {
            Config::set_filename ("clanbomber_net.cfg");
        }
        else
        {
            Config::set_filename ("clanbomber.cfg");
        }
        Config::load();
    }
}

void Menu::set_left_netgame_setup()
{
    left_netgame_setup = true;
}
