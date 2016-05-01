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

#include "FontSDL.h"
#include "Font.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

extern SDL_Surface *primary;

namespace cbe
{
  //unsigned int FontSDL::count = 0;
  bool FontSDL::high_quality = true;

  FontSDL::FontSDL(const char *ttfFile, unsigned int size)
  {
    //StartFontSystem();
    font = TTF_OpenFont(ttfFile, size);
  }

  FontSDL::~FontSDL()
  {
    TTF_CloseFont(font);
    //StopFontSystem();
  }

  void FontSDL::render(const std::string &text, int x, int y,
                       FontAlignment alignment)
  {
    SDL_Rect rect = {x, y, 0, 0};

    SDL_Color color = {0xd3, 0xd7, 0xcf};
    
    SDL_Surface *textSurface = NULL;
    if(high_quality) {
      textSurface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    } else {
      textSurface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
    }

    //Empty string used in TTF_RenderUTF8_Solid returns NULL
    if(textSurface == NULL) 
      return;
  
    //horizontal alignment
    if(alignment & FontAlignment_0center) {
      rect.x -= textSurface->w/2;
    } else if(alignment & FontAlignment_0right) {
      rect.x -= textSurface->w;
    } //else keep x value which is Left aligned

    //keep top current value
    if(alignment & FontAlignment_0top) {
      //keep y calue which is Top aligned
    } else if(alignment & FontAlignment_0bottom) {
      rect.y += textSurface->h;
    } else { //baseline
      rect.y -= TTF_FontAscent(font);
    }

    SDL_BlitSurface(textSurface, NULL, primary, &rect);

    SDL_FreeSurface(textSurface);
  }

  void FontSDL::render(const std::wstring &text, int x, int y,
                       FontAlignment alignment)
  {
    char *utf8text = SDL_iconv_string("UTF-8", "wchar_t", (char*) text.c_str(),
                                      (text.length() + 1) * sizeof(wchar_t));
    render(utf8text, x, y, alignment);
    SDL_free(utf8text);
  }
    
  void FontSDL::getSize(const std::string &text, int *w, int *h)
  {
    TTF_SizeUTF8(font, text.c_str(), w, h);
  }

  int FontSDL::getHeight()
  {
    return TTF_FontHeight(font);
  }

};
