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

#ifndef RESOURCES_H
#define RESOURCES_H

///#include <directfb.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "cbe/Surface.h"
#include "cbe/Font.h"
#include "cbe/AudioBuffer.h"
#include "cbe/Music.h"

#define NR_BOMBERSKINS 8

/**
 *@author Andreas Hundt, Denis Oliver Kropp
 */

class Resources {
public:
	Resources();
	~Resources();

	class Surface {
	public:
		Surface(const char *filename, int sprite_width, int sprite_height);
		~Surface();

		int get_height();

		void put_screen(int x, int y, int frame, Uint8 opacity = 255);
		void put_screen(int x, int y, float scale_x, float scale_y, int frame,
				Uint8 opacity = 255);

		void get_rect(int frame, SDL_Rect *ret_rect);

		SDL_Surface *surface;

	private:
		int sprite_width;
		int sprite_height;
		int frames_per_row;
	};

	static void init();

	static cbe::Font* Font_big();
	static cbe::Font* Font_small();

	static cbe::Surface* Titlescreen();

	static cbe::Surface* Intro_fl_logo();
	static cbe::AudioBuffer* Intro_typewriter();
	static cbe::AudioBuffer* Intro_winlevel();

	static Surface* Playersetup_teams();
	static Surface* Playersetup_controls();
	static Surface* Playersetup_teamlamps();
	static cbe::Surface* Playersetup_background();

	static cbe::Surface* Mapselector_background();
	static cbe::Surface* Mapselector_not_available();

	static Surface* Gamestatus_tools();
	static cbe::AudioBuffer* Gamestatus_klatsch();
	static cbe::Surface* Gamestatus_background();

	static cbe::AudioBuffer* Credits_forward();
	static cbe::AudioBuffer* Credits_rewind();
	static cbe::AudioBuffer* Credits_stop();

	static cbe::Surface* Credits_horst_evil();

	static Surface* Bombers(int nr);

	static Surface* Extras_extras();
	static cbe::AudioBuffer* Extras_wow();
	static cbe::AudioBuffer* Extras_joint();
	static cbe::AudioBuffer* Extras_horny();
	static cbe::AudioBuffer* Extras_schnief();

	static cbe::AudioBuffer* Menu_whoosh();
	static cbe::AudioBuffer* Menu_break();
	static cbe::AudioBuffer* Menu_clear();
	static cbe::AudioBuffer* Menu_back();

	static Surface* Observer_observer();
	static cbe::AudioBuffer* Observer_hurry_up();
	static cbe::AudioBuffer* Observer_time_over();
	static cbe::AudioBuffer* Observer_crunch();

	static cbe::AudioBuffer* Game_die();
	static cbe::AudioBuffer* Game_explode();
	static cbe::AudioBuffer* Game_putbomb();
	static cbe::AudioBuffer* Game_deepfall();
	static Surface* Game_maptiles();
	static Surface* Game_maptile_addons();
	static Surface* Game_bombs();
	static Surface* Game_explosion();
	static cbe::Surface* Game_cb_logo_small();

	static cbe::Surface* MapEditor_background();

	static cbe::AudioBuffer* Corpse_explode();
	static Surface* Corpse_part();

	static cbe::AudioBuffer* Splash(int nr);
	static cbe::Music* Music();

protected:

	static cbe::Font *fnt_big;
	static cbe::Font *fnt_small;

	static cbe::Surface *sur_titlescreen;

	static cbe::Surface *sur_fl_logo;
	static cbe::AudioBuffer *sfx_typewriter;
	static cbe::AudioBuffer *sfx_winlevel;

	static Surface *sur_ps_teams;
	static Surface *sur_ps_controls;
	static Surface *sur_ps_teamlamps;
	static cbe::Surface *sur_ps_background;

	static cbe::Surface *sur_ls_background;
	static cbe::Surface *sur_ls_not_available;

	static Surface *sur_gs_tools;
	static cbe::AudioBuffer *sfx_klatsch;
	static cbe::Surface *sur_gs_background;

	static cbe::AudioBuffer *sfx_forward;
	static cbe::AudioBuffer *sfx_rewind;
	static cbe::AudioBuffer *sfx_stop;

	static cbe::Surface *sur_horst_evil;

	static Surface *sur_bombers[NR_BOMBERSKINS];

	static Surface *sur_extras;
	static cbe::AudioBuffer *sfx_wow;
	static cbe::AudioBuffer *sfx_joint;
	static cbe::AudioBuffer *sfx_horny;
	static cbe::AudioBuffer *sfx_schnief;

	static cbe::AudioBuffer *sfx_whoosh;
	static cbe::AudioBuffer *sfx_break;
	static cbe::AudioBuffer *sfx_clear;
	static cbe::AudioBuffer *sfx_menu_back;

	static Surface *sur_observer;
	static cbe::AudioBuffer *sfx_hurry_up;
	static cbe::AudioBuffer *sfx_time_over;
	static cbe::AudioBuffer *sfx_crunch;

	static cbe::AudioBuffer *sfx_die;
	static cbe::AudioBuffer *sfx_explode;
	static cbe::AudioBuffer *sfx_putbomb;
	static cbe::AudioBuffer *sfx_deepfall;
	static Surface *sur_maptiles;
	static Surface *sur_maptile_addons;
	static Surface *sur_bombs;
	static Surface *sur_explosion;
	static cbe::Surface *sur_cb_logo_small;

	static cbe::Surface *sur_me_background;

	static cbe::AudioBuffer *sfx_corpse_explode;
	static Surface *sur_corpse_part;

	static cbe::AudioBuffer *sfx_splash[2];
	static cbe::Music *music;
};

#endif
