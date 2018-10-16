/*
 * This file is part of ClanBomber;
 * you can get it at "http://www.nongnu.org/clanbomber".
 *
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

#ifndef Utils_h
#define Utils_h

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

//required for ENABLE_NLS
#include "config.h"
#include "gettext.h"
#define _(String) gettext(String)
#define N_(String) String

void CB_RenderText(TTF_Font *font, const std::wstring &text, int x, int y);
void CB_RenderText(TTF_Font *font, const std::string &text, int x, int y);
void CB_RenderTextCenter(TTF_Font *font, const std::string &text, int x, int y);
void CB_RenderTextRight(TTF_Font *font, const std::string &text, int x, int y);
void CB_FillRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b);
void CB_FillRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b,
		Uint8 a);
void CB_WaitForKeypress();
void CB_BatchBlit(SDL_Texture *texture, SDL_Rect *srcRects, SDL_Rect *destRects,
		int num);
void CB_FillRects(SDL_Rect *rects, int num, Uint8 r, Uint8 g, Uint8 b, Uint8 a =
		0xFF);
int CB_EnterText(std::string &new_string);
void CB_Locale();
void CB_Flip();

#endif
