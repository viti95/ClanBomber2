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

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <SDL2/SDL.h>

#include "config.h"
#include "Controller.h"
#include "Observer.h"
#include "GameConfig.h"
#include "Menu.h"
#include "Timer.h"
#include "PlayerSetup.h"
#include "Map.h"
#include "Credits.h"
#include "MapEditor.h"
#include "MapTile.h"
#include "Bomber.h"
#include "MapSelector.h"
#include "Bomb.h"
#include "Utils.h"
#include "UtilsGetHome.h"
#include "AudioSimple.h"
#include "AudioSimpleSDL.h"

#include <unistd.h>

ClanBomberApplication *app;

SDL_Renderer *renderer = NULL;
SDL_Window *gameWindow = NULL;
const Uint8 *keyboard = NULL;

boost::filesystem::path ClanBomberApplication::map_path;
boost::filesystem::path ClanBomberApplication::local_map_path;

static unsigned short next_object_id = 0;

/* initialize state to random bits */
static unsigned long state[16];
/* init should also reset this to 0 */
static unsigned int indx = 0;

/* return 32 bit random number */
unsigned long fast_random(void) {
	unsigned long a, b, c, d;
	a = state[indx];
	c = state[(indx + 13) & 15];
	b = a ^ c ^ (a << 16) ^ (c << 15);
	c = state[(indx + 9) & 15];
	c ^= (c >> 11);
	a = state[indx] = b ^ c;
	d = a ^ ((a << 5) & 0xDA442D24UL);
	indx = (indx + 15) & 15;
	a = state[indx];
	state[indx] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
	return state[indx];
}

bool game_object_compare(GameObject * go1, GameObject * go2) {
	return go1->get_z() < go2->get_z();
}

ClanBomberApplication::ClanBomberApplication() {
	observer = NULL;
	map = NULL;
	pause_game = false;

	// initialize fast_random
	for (int i = 0; i < 16; i++)
		state[i] = rand();
}

ClanBomberApplication::~ClanBomberApplication() {
	std::cout << _("(+) clanbomber: deallocating resources...") << std::endl;

	AS->close();
	TTF_Quit();
	SDL_Quit();

	if (map) {
		delete map;
		map = NULL;
	}

}

int ClanBomberApplication::init_SDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK)) {
		std::cerr << _("Cannot Initialise SDL!") << std::endl;
		SDL_Quit();
		return -1;
	}

	Uint32 fullscreen = (Config::get_fullscreen()) ? SDL_WINDOW_FULLSCREEN : 0;
	Uint32 renderMode = (Config::get_softwareRendering()) ? SDL_RENDERER_SOFTWARE : SDL_RENDERER_ACCELERATED;

	if (Config::get_fullscreen()) SDL_SetRelativeMouseMode(SDL_TRUE);

	keyboard = SDL_GetKeyboardState(NULL);
	
	#ifdef SDL_HINT_RENDER_BATCHING
	SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1"); // enable batch rendering, it's faster
	#endif
	
	#ifdef SDL_HINT_RENDER_SCALE_QUALITY
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	#endif

	gameWindow = SDL_CreateWindow(PACKAGE_STRING,
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          800, 600,
                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | fullscreen);

	renderer = SDL_CreateRenderer(gameWindow, -1, renderMode);

	SDL_RenderSetLogicalSize(renderer, 800, 600);

   	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	AS = new cbe::AudioSimpleSDL();
	if (AS == NULL) {
		std::cerr << _("Cannot initialise the requested audio plugin!")
				<< std::endl;
	}
	if (!AS->init()) {
		std::cerr << _("Cannot Initialise SDL audio!") << std::endl;
	}

	if (TTF_Init()) {
		std::cerr << _("Cannot Initialise SDL ttf!") << std::endl;
		TTF_Quit();
		AS->close();
		SDL_Quit();
		return -1;
	}

	return 0;
}

bool ClanBomberApplication::paused_game() {
	return pause_game;
}

void ClanBomberApplication::set_pause_game(bool paused) {
	pause_game = paused;
}

void ClanBomberApplication::reload_map() {
	if (map) {
		delete map;
	}
	map = new Map(this);
}

unsigned short ClanBomberApplication::get_next_object_id() {
	if (next_object_id++ > 65534) {
		next_object_id = 23;
	}
	return next_object_id;
}

int ClanBomberApplication::main() {
	std::srand(std::time(NULL));

	if (init_SDL()) {
		return -1;
	}

	show_fps = false;
	play_music = false;
	map_path = CB_DATADIR
	"/maps";

	boost::filesystem::path configpath(GetConfigHome() / "clanbomber");
	if (!RecursiveDirCreation(configpath)) {
		boost::format fmt(_("Config directory (%1$s) cannot be created"));
		std::cout << fmt % configpath << std::endl;
	}

	Config::set_path(configpath);
	Config::set_filename("clanbomber.cfg");

	boost::filesystem::path mappath(GetDataHome() / "clanbomber" / "maps");
	if (!RecursiveDirCreation(mappath)) {
		boost::format fmt(_("Data directory (%1$s) cannot be created"));
		std::cout << fmt % mappath << std::endl;
	}
	local_map_path = mappath;

	Resources::init();
	Config::load();
	run_intro();

	map = NULL;
	observer = NULL;

	menu = new Menu(_("Main Menu"), this);

	menu->add_item(_("New Game"), MENU_GAME);
	menu->add_item(_("Start Local"), LOCALGAME_START, MENU_GAME);
	menu->add_item(_("Local Player Setup"), MENU_PLAYER_SETUP, MENU_GAME);
	menu->add_item(_("Map Selection"), CONFIG_MAP_SEL, MENU_GAME);
	menu->add_value(_("Random Bomber Positions"), CONFIG_RANDOM_POSITIONS,
			MENU_GAME, 0, 1, Config::get_random_positions());
	menu->add_value(_("Random Map Order"), CONFIG_RANDOM_MAP_ORDER, MENU_GAME,
			0, 1, Config::get_random_map_order());
	menu->add_value(_("Points to win"), CONFIG_POINTS, MENU_GAME, 1, 10,
			Config::get_points_to_win());
	menu->add_value(_("Round Time"), CONFIG_ROUND_TIME, MENU_GAME, 5, 300,
			Config::get_round_time());

	menu->add_item(_("Options"), MENU_OPTIONS);
	menu->add_item(_("Start/Max Extras"), MENU_EXTRA_VALUES, MENU_OPTIONS);
	menu->add_value(_("Start Bombs"), CONFIG_START_BOMBS, MENU_EXTRA_VALUES, 1,
			15, Config::get_start_bombs());
	menu->add_value(_("Start Power"), CONFIG_START_POWER, MENU_EXTRA_VALUES, 1,
			15, Config::get_start_power());
	menu->add_value(_("Start Skateboards"), CONFIG_START_SKATES,
			MENU_EXTRA_VALUES, 0, 10, Config::get_start_skateboards());
	menu->add_value(_("Start Kick"), CONFIG_START_KICK, MENU_EXTRA_VALUES, 0, 1,
			Config::get_start_kick());
	menu->add_value(_("Start Glove"), CONFIG_START_GLOVE, MENU_EXTRA_VALUES, 0,
			1, Config::get_start_glove());
	menu->add_value(_("Max. Bombs"), CONFIG_MAX_BOMBS, MENU_EXTRA_VALUES, 1, 15,
			Config::get_max_bombs());
	menu->add_value(_("Max. Power"), CONFIG_MAX_POWER, MENU_EXTRA_VALUES, 1, 15,
			Config::get_max_power());
	menu->add_value(_("Max. Skateboards"), CONFIG_MAX_SKATES, MENU_EXTRA_VALUES,
			0, 10, Config::get_max_skateboards());
	menu->add_item(_("Enable/Disable Extras"), MENU_EXTRA_ONOFF, MENU_OPTIONS);
	menu->add_value(_("Bombs"), CONFIG_BOMBS, MENU_EXTRA_ONOFF, 0, 1,
			Config::get_bombs());
	menu->add_value(_("Power"), CONFIG_POWER, MENU_EXTRA_ONOFF, 0, 1,
			Config::get_power());
	menu->add_value(_("Skateboard"), CONFIG_SKATES, MENU_EXTRA_ONOFF, 0, 1,
			Config::get_skateboards());
	menu->add_value(_("Kick"), CONFIG_KICK, MENU_EXTRA_ONOFF, 0, 1,
			Config::get_kick());
	menu->add_value(_("Glove"), CONFIG_GLOVE, MENU_EXTRA_ONOFF, 0, 1,
			Config::get_glove());
	menu->add_item(_("Enable/Disable Diseases"), MENU_DISEASE_ONOFF,
			MENU_OPTIONS);
	menu->add_value(_("Joint"), CONFIG_JOINT, MENU_DISEASE_ONOFF, 0, 1,
			Config::get_joint());
	menu->add_value(_("Viagra"), CONFIG_VIAGRA, MENU_DISEASE_ONOFF, 0, 1,
			Config::get_viagra());
	menu->add_value(_("Koks"), CONFIG_KOKS, MENU_DISEASE_ONOFF, 0, 1,
			Config::get_koks());
	menu->add_item(_("Bomb Timing and Speed"), MENU_TIMING, MENU_OPTIONS);
	menu->add_value(_("Bomb Countdown (1/10 s)"), CONFIG_BOMB_COUNTDOWN,
			MENU_TIMING, 0, 50, Config::get_bomb_countdown());
	menu->add_value(_("Bomb Chain Reaction Delay (1/100 s)"), CONFIG_BOMB_DELAY,
			MENU_TIMING, 0, 50, Config::get_bomb_delay());
	menu->add_value(_("Moving Bombs Speed (pixels per second)"),
			CONFIG_BOMB_SPEED, MENU_TIMING, 10, 500, Config::get_bomb_speed());
	menu->add_item(_("Graphic Options"), MENU_GRAPHICS, MENU_OPTIONS);
	menu->add_value(_("Kidz Mode"), CONFIG_KIDS_MODE, MENU_GRAPHICS, 0, 1,
			Config::get_kids_mode());
	menu->add_value(_("Corpse Parts"), CONFIG_CORPSE_PARTS, MENU_GRAPHICS, 0,
			100, Config::get_corpse_parts());
	menu->add_value(_("Shaky Explosions"), CONFIG_SHAKE, MENU_GRAPHICS, 0, 1,
			Config::get_shaky_explosions());
	menu->add_value(_("Random Bomber Positions"),
			CONFIG_RANDOM_POSITIONS_DUPLICATE, MENU_OPTIONS, 0, 1,
			Config::get_random_positions());
	menu->add_value(_("Random Map Order"), CONFIG_RANDOM_MAP_ORDER_DUPLICATE,
			MENU_OPTIONS, 0, 1, Config::get_random_map_order());
	menu->add_value(_("Points to win"), CONFIG_POINTS_DUPLICATE, MENU_OPTIONS,
			1, 10, Config::get_points_to_win());
	menu->add_value(_("Round Time"), CONFIG_ROUND_TIME_DUPLICATE, MENU_OPTIONS,
			5, 300, Config::get_round_time());

	menu->backup_options_values();
	menu->save_common_options(0, false, true);

	menu->add_item(_("Map Editor"), MENU_MAP_EDITOR);
	menu->add_item(_("Show Credits"), MENU_CREDITS);
	menu->add_item(_("Help Screen"), MENU_HELP);
	menu->add_item(_("Quit Game"), MENU_EXIT);

	menu->scroll_in();

	while (1) {
		int result;

		result = menu->execute(false);

		MenuItem *item = menu->get_item_by_id(result);
		switch (result) {
		case MENU_EXIT:
			menu->scroll_out();
			delete menu;
			return 0;
		case MENU_PLAYER_SETUP:
			menu->scroll_out();
			{
				PlayerSetup ps(this);
				ps.exec();
			}
			menu->scroll_in();
			break;
		case CONFIG_START_BOMBS:
			Config::set_start_bombs(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_START_POWER:
			Config::set_start_power(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_START_SKATES:
			Config::set_start_skateboards(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_START_KICK:
			Config::set_start_kick(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_START_GLOVE:
			Config::set_start_glove(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_BOMBS:
			Config::set_bombs(static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_POWER:
			Config::set_power(static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_SKATES:
			Config::set_skateboards(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_KICK:
			Config::set_kick(static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_GLOVE:
			Config::set_glove(static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_JOINT:
			Config::set_joint(static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_VIAGRA:
			Config::set_viagra(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_KOKS:
			Config::set_koks(static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_MAX_BOMBS:
			Config::set_max_bombs(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_MAX_POWER:
			Config::set_max_power(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_MAX_SKATES:
			Config::set_max_skateboards(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case MENU_CREDITS:
			menu->scroll_out();
			{
				Credits credits(this);
				credits.exec();
			}
			menu->scroll_in();
			break;
		case MENU_MAP_EDITOR:
			menu->scroll_out();
			{
				MapEditor me(this);
				me.exec();
			}
			menu->scroll_in();
			break;
		case MENU_HELP:
			menu->scroll_out();
			show_tutorial();
			menu->scroll_in();
			break;
		case CONFIG_MAP_SEL:
			menu->scroll_out();
			{
				MapSelector ms(this);
				ms.exec();
			}
			menu->scroll_in();
			break;
		case CONFIG_POINTS:
			Config::set_points_to_win(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_POINTS_DUPLICATE:
			Config::set_points_to_win(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_ROUND_TIME:
			if (static_cast<MenuItem_Value *>(item)->get_value() % 5 == 1) {
				static_cast<MenuItem_Value *>(item)->set_value(
						static_cast<MenuItem_Value *>(item)->get_value() + 4);
			} else if (static_cast<MenuItem_Value *>(item)->get_value() % 5
					== 4) {
				static_cast<MenuItem_Value *>(item)->set_value(
						static_cast<MenuItem_Value *>(item)->get_value() - 4);
			}
			Config::set_round_time(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_ROUND_TIME_DUPLICATE:
			if (static_cast<MenuItem_Value *>(item)->get_value() % 5 == 1) {
				static_cast<MenuItem_Value *>(item)->set_value(
						static_cast<MenuItem_Value *>(item)->get_value() + 4);
			} else if (static_cast<MenuItem_Value *>(item)->get_value() % 5
					== 4) {
				static_cast<MenuItem_Value *>(item)->set_value(
						static_cast<MenuItem_Value *>(item)->get_value() - 4);
			}
			Config::set_round_time(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_KIDS_MODE:
			Config::set_kids_mode(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_CORPSE_PARTS:
			Config::set_corpse_parts(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_SHAKE:
			Config::set_shaky_explosions(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_BOMB_COUNTDOWN:
			Config::set_bomb_countdown(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_BOMB_DELAY:
			Config::set_bomb_delay(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_BOMB_SPEED:
			if (static_cast<MenuItem_Value *>(item)->get_value() % 10 == 1) {
				static_cast<MenuItem_Value *>(item)->set_value(
						static_cast<MenuItem_Value *>(item)->get_value() + 9);
			} else if (static_cast<MenuItem_Value *>(item)->get_value() % 10
					== 9) {
				static_cast<MenuItem_Value *>(item)->set_value(
						static_cast<MenuItem_Value *>(item)->get_value() - 9);
			}
			Config::set_bomb_speed(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_RANDOM_POSITIONS:
			Config::set_random_positions(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_RANDOM_POSITIONS_DUPLICATE:
			Config::set_random_positions(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_RANDOM_MAP_ORDER:
			Config::set_random_map_order(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case CONFIG_RANDOM_MAP_ORDER_DUPLICATE:
			Config::set_random_map_order(
					static_cast<MenuItem_Value *>(item)->get_value());
			Config::save();
			break;
		case LOCALGAME_START:
			menu->scroll_out();
			if (Config::get_number_of_opponents() > 1) {
				if (init_game()) {
					run_game();
					deinit_game();
				}
			} else {
				menu->scroll_in();
			}
			break;
		default:
			std::cout << result << std::endl;
			break;
		}
	}
}

void ClanBomberApplication::run_game() {

	if(play_music && !Resources::Music()->isPlaying())
		Resources::Music()->playLoop();

	Timer::reset();

	bool escape = false;
	while (!escape) {

		Timer::time_elapsed(true);

		SDL_RenderClear(renderer);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_F1:
					show_fps = !show_fps;
					break;
				case SDL_SCANCODE_F2:
					play_music = !play_music;
					break;
				case SDL_SCANCODE_P:
				case SDL_SCANCODE_F3:
					pause_game = !pause_game;
					break;
				case SDL_SCANCODE_ESCAPE:
					escape = true;
					break;
				}
			}
		}

		if (play_music){
			if (!Resources::Music()->isPlaying()){
				Resources::Music()->playLoop();
			}
		}else{
			if (Resources::Music()->isPlaying()){
				Resources::Music()->stop();
			}
		}

		if (pause_game) {
			show_all();
			CB_FillRect(0, 0, 800, 600, 0, 0, 0, 123);
			Resources::Font_big()->render(_("- PAUSED -"), 400, 300,
					cbe::FontAlignment_0topcenter);
			SDL_RenderPresent(renderer);
			continue;
		}

		if (observer != NULL) {
			observer->act();
			if (observer->end_of_game_requested()) {
				break;
			}
		}

		delete_some();
		act_all();
		show_all();

		SDL_RenderPresent(renderer);
		frame_count++;

		frame_time += Timer::time_elapsed();
		if (frame_time > 2) {
			fps = (int) (frame_count / frame_time + 0.5f);
			frame_time = 0;
			frame_count = 0;
		}
	}

	if (Resources::Music()->isPlaying())
		Resources::Music()->stop();
}

Menu *
ClanBomberApplication::get_menu() {
	return app->menu;
}

void ClanBomberApplication::make_observer() {
	if (observer == NULL) {
		observer = new Observer(0, 0, this);
	}
}

void ClanBomberApplication::make_map() {
	if (map == NULL) {
		map = new Map(this);
	}
}

Observer *
ClanBomberApplication::get_observer() {
	return observer;
}

Map *
ClanBomberApplication::get_map() {
	return map;
}

void ClanBomberApplication::delete_all_game_objects() {
	objects.clear();
	bomber_objects.clear();
}

GameObject *
ClanBomberApplication::get_object_by_id(int object_id) {
	for (const auto &bomber_object_iter : bomber_objects) {
		if ((bomber_object_iter)->get_object_id() == object_id) {
			return bomber_object_iter;
		}
	}
	for (const auto &game_object_iter : objects) {
		if ((game_object_iter)->get_object_id() == object_id) {
			return game_object_iter;
		}
	}
	return NULL;
}

void ClanBomberApplication::act_all() {
	// Map acts first
	if (map != NULL) {
		map->act();
	}

	for (const auto & object_iter : objects) {
		object_iter->act();
	}

	for (const auto & bomber_object_iter : bomber_objects) {
		bomber_object_iter->act();
	}
}

void ClanBomberApplication::delete_some() {
	// Check if we should delete some

	for (auto object_iter = objects.begin(); object_iter != objects.end();) {
		if ((*object_iter)->delete_me) {
			delete *object_iter;
			object_iter = objects.erase(object_iter);
		} else {
			++object_iter;
		}
	}
}

void ClanBomberApplication::show_all() {
	GameObject *draw_list[objects.size() + bomber_objects.size()];
	int n = 0;
	int i;

	const int top_height = 40;
	const int margin = 60;

	if (show_fps) {
		std::string nstr = str(boost::format(_("%1$d fps")) % fps);
		Resources::Font_small()->render(nstr, 535, 4, cbe::FontAlignment_0topcenter);
	}

	for (const auto &object_iter : objects) {
		draw_list[n] = (object_iter);
		n++;
	}
	for (const auto &bomber_object_iter : bomber_objects) {
		draw_list[n] = (bomber_object_iter);
		n++;
	}

	std::sort(draw_list, draw_list + n, game_object_compare);

	if (map != NULL) {
		map->refresh_holes();
	}

	bool drawn_map = false;

	for (i = 0; i < n; i++) {
		if (map != NULL && draw_list[i]->get_z() >= Z_GROUND && drawn_map == false) {
			map->show();
			drawn_map = true;
		}
		if (draw_list[i] != NULL) {
			draw_list[i]->show();
		}
	}
	if (map != NULL && !drawn_map) {
		map->show();
	}
	if (observer != NULL) {
		observer->show();
	}
}

bool ClanBomberApplication::init_game() {
	frame_count = 0;
	frame_time = 0;
	fps = 0;

	// init map
	if (map == NULL) {
		map = new Map(this);
	}

	if (!map->any_valid_map()) {
		delete map;
		map = NULL;
		return false;
	}

	if (Config::get_random_map_order()) {
		map->load_random_valid();
	} else {
		if (Config::get_start_map() > map->get_map_count() - 1) {
			Config::set_start_map(map->get_map_count() - 1);
		}
		map->load_next_valid(Config::get_start_map());
	}
	// init GameObjects
	if (Config::get_random_positions()) {
		map->randomize_bomber_positions();
	}
	CL_Vector pos;
	int j = 0;
	for (int i = 0; i < 8; i++) {
		if (Config::bomber[i].is_enabled()) {
			pos = map->get_bomber_pos(j++);
			new Bomber((int) (pos.x * 40), (int) (pos.y * 40),
					static_cast<Bomber::COLOR>(Config::bomber[i].get_skin()),
					Controller::create(
							static_cast<Controller::CONTROLLER_TYPE>(Config::bomber[i].get_controller())),
					Config::bomber[i].get_name(), Config::bomber[i].get_team(),
					i, this);
			bomber_objects.back()->set_pos(350, 270);
			bomber_objects.back()->fly_to((int) (pos.x * 40),
					(int) (pos.y * 40), 300);
			bomber_objects.back()->controller->deactivate();
		}
	}
	// this is for removing teams which only one player is in
	int team_count[] = { 0, 0, 0, 0 };
	for (int team = 0; team < 4; team++) {
		for (const auto &bomber_object_iter : bomber_objects) {
			if ((bomber_object_iter)->get_team() - 1 == team) {
				team_count[team]++;
			}
		}
	}
	for (const auto &bomber_object_iter : bomber_objects) {
		if ((bomber_object_iter)->get_team() != 0) {
			if (team_count[(bomber_object_iter)->get_team() - 1] == 1) {
				(bomber_object_iter)->set_team(0);
			}
		}
	}

	observer = new Observer(0, 0, this);

	return true;
}

void ClanBomberApplication::deinit_game() {
	// delete all GameObjects
	for (const auto &object_iter : objects) {
		delete object_iter;
	}
	objects.clear();
	for (const auto &bomber_object_iter : bomber_objects) {
		delete bomber_object_iter;
	}
	bomber_objects.clear();
	if (observer) {
		delete observer;
		observer = NULL;
	}
}

void ClanBomberApplication::run_intro() {
	SDL_Scancode escape = SDL_SCANCODE_ESCAPE;
	float alpha = 0;
	std::string domination(_("A WORLD DOMINATION PROJECT"));
	std::string & domihack = domination;

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	sleep(2);

	Timer::reset();
	Resources::Intro_winlevel()->play();
	while (!keyboard[escape]) {

		SDL_PumpEvents();

		if (alpha < 255) {
			Resources::Intro_fl_logo()->blit(100, 250, alpha);
			alpha += Timer::time_elapsed(true) * 130.0f;
			SDL_RenderPresent(renderer);
		} else {
			Resources::Intro_fl_logo()->blit(100, 250);
			break;
		}
	}

	int stringwidth;

	Resources::Font_small()->getSize(domination, &stringwidth, NULL);

	if (!keyboard[escape]) {
		//usleep (500000);

		for (int domispell = 0; domispell <= domihack.length(); domispell++) {

			SDL_PumpEvents();
			if (keyboard[escape]) {
				break;
			}

			if (domination.substr(domispell, 1) != " ") {
				Resources::Intro_typewriter()->play();
			}
			Resources::Intro_fl_logo()->blit(100, 250);
			Resources::Font_small()->render(domihack.substr(0, domispell),
					400 - stringwidth / 2, 360, cbe::FontAlignment_0topleft);

			SDL_RenderPresent(renderer);

			usleep(fast_random() % 100000 + 80000);
		}

		Resources::Menu_back()->play();

	}
}

void ClanBomberApplication::show_tutorial() {
	int y = 25;
	Resources::Titlescreen()->blit(0, 0);
	CB_FillRect(0, 0, 800, 600, 0, 0, 0, 128);
	Resources::Font_big()->render(_("ClanBomber Extras"), 400, y,
			cbe::FontAlignment_0topcenter);
	y += 80;
	Resources::Extras_extras()->put_screen(15, y - 5, 0);
	Resources::Font_big()->render(_("Bomb:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("You can place an additional bomb"), 250, y,
			cbe::FontAlignment_0topleft);

	y += 50;
	Resources::Extras_extras()->put_screen(15, y - 5, 1);
	Resources::Font_big()->render(_("Power:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("Explosions grow one field in each "
			"direction"), 250, y, cbe::FontAlignment_0topleft);

	y += 50;
	Resources::Extras_extras()->put_screen(15, y - 5, 2);
	Resources::Font_big()->render(_("Skateboard:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("Let's you move faster"), 250, y,
			cbe::FontAlignment_0topleft);

	y += 50;
	Resources::Extras_extras()->put_screen(15, y - 5, 3);
	Resources::Font_big()->render(_("Kick:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("Kick bombs if you walk against one"), 250,
			y, cbe::FontAlignment_0topleft);

	y += 50;
	Resources::Extras_extras()->put_screen(15, y - 5, 4);
	Resources::Font_big()->render(_("Throw:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(
			_("Throw Bombs if you press the button twice"), 250, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("without moving"), 250, y + 40,
			cbe::FontAlignment_0topleft);

	Resources::Font_big()->render(_("Press any key"), 400, 520,
			cbe::FontAlignment_0topcenter);
	SDL_RenderPresent(renderer);

	CB_WaitForKeypress();

	y = 25;
	Resources::Titlescreen()->blit(0, 0);
	CB_FillRect(0, 0, 800, 600, 0, 0, 0, 128);

	Resources::Font_big()->render(_("ClanBomber Drugs"), 400, y,
			cbe::FontAlignment_0topcenter);

	y += 80;
	Resources::Extras_extras()->put_screen(15, y - 5, 5);
	Resources::Font_big()->render(_("Joint:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("Controller will be reversed"), 250, y,
			cbe::FontAlignment_0topleft);

	y += 50;
	Resources::Extras_extras()->put_screen(15, y - 5, 6);
	Resources::Font_big()->render(_("Viagra:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("Autofire, this can be very dangerous!"),
			250, y, cbe::FontAlignment_0topleft);

	y += 50;
	Resources::Extras_extras()->put_screen(15, y - 5, 7);
	Resources::Font_big()->render(_("Cocaine:"), 70, y,
			cbe::FontAlignment_0topleft);
	Resources::Font_big()->render(_("Let's you move very fast!! (too fast)"),
			250, y, cbe::FontAlignment_0topleft);

	Resources::Font_big()->render(_("Press any key"), 400, 520,
			cbe::FontAlignment_0topcenter);
	SDL_RenderPresent(renderer);

	CB_WaitForKeypress();
}

boost::filesystem::path ClanBomberApplication::get_map_path() {
	return map_path;
}

boost::filesystem::path ClanBomberApplication::get_local_map_path() {
	return local_map_path;
}

int main(int argc, char **argv) {

	CB_Locale();
	// parse command line options
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--help")) {
			std::cout
					<< _(
							"\n    usage: clanbomber2 [--software] [--fullscreen]\n\n");
			return 0;
		} else if (!strcmp(argv[i], "--fullscreen")) {
			Config::set_fullscreen(true);
		} else if (!strcmp(argv[i], "--software")) {
			Config::set_softwareRendering(true);
		} else {
			std::cout << _("Invalid argument") << std::endl;
		}
	}

	app = new ClanBomberApplication();
	app->main();
	delete app;

	return 0;
}
