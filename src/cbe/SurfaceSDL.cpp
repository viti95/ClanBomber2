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

#include "SurfaceSDL.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>

extern SDL_Surface *primary;

namespace cbe
{
  SurfaceSDL::SurfaceSDL(const char *filename)
  {
    surface = IMG_Load(filename);
  }

  SurfaceSDL::SurfaceSDL(SDL_Surface *extSurface, SDL_Rect *rect)
  {
    create(extSurface, rect);
  }

  SurfaceSDL::~SurfaceSDL()
  {
    SDL_FreeSurface(surface);
  }

  void SurfaceSDL::blit(int x, int y, uint8_t opacity)
  {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    SDL_SetAlpha(surface, SDL_SRCALPHA, opacity);
    SDL_BlitSurface(surface, NULL, primary, &rect);
  }
  void SurfaceSDL::scaled_blit(int x, int y, float scale_x, float scale_y, 
                               uint8_t opacity)
  {
    SDL_Surface *tmpSurface;
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;

    tmpSurface = zoomSurface(surface, scale_x, scale_y, 0);
    SDL_BlitSurface(tmpSurface, NULL, primary, &rect);
    SDL_FreeSurface(tmpSurface);
  }

  void SurfaceSDL::create(SDL_Surface *extSurface, SDL_Rect *rect)
  {
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_ASYNCBLIT,
                                   rect->w,
                                   rect->h,
                                   primary->format->BitsPerPixel,
                                   primary->format->Rmask,
                                   primary->format->Gmask,
                                   primary->format->Bmask,
                                   primary->format->Amask);

    SDL_PixelFormat *format = extSurface->format;
    Uint8 r ,g, b;
    r = ((format->colorkey & format->Rmask) >> format->Rshift) << format->Rloss;
    g = ((format->colorkey & format->Gmask) >> format->Gshift) << format->Gloss;
    b = ((format->colorkey & format->Bmask) >> format->Bshift) << format->Bloss;
    SDL_SetColorKey(surface, 
                    SDL_SRCCOLORKEY, 
                    SDL_MapRGB(surface->format, r, g, b));

    SDL_BlitSurface(extSurface, rect, surface, NULL);
  }

  Surface *CreateSurface(char *filename)
  {
    return new SurfaceSDL(filename);
  }

  Surface *CreateSurface(SDL_Surface *extSurface, SDL_Rect *rect)
  {
    return new SurfaceSDL(extSurface, rect);
  }
};
