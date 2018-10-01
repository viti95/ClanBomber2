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

#ifndef FONTSDL_H
#define FONTSDL_H
#include "Font.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

namespace cbe
{
  class FontSDL : public Font
  {
  public:
    FontSDL(const char *ttfFile, unsigned int size);
    virtual ~FontSDL();
    void render(const std::string &text, int x, int y,
                FontAlignment alignment = FontAlignment_0left);
    void render(const std::wstring &text, int x, int y,
                FontAlignment alignment = FontAlignment_0left);
    void getSize(const std::string &text, int *w, int *h);
    int getHeight();
  private:
    //static unsigned int count;
    //void StartFontSystem();
    //void StopFontSystem();
    TTF_Font *font;
    static bool high_quality;
  };
};
#endif
