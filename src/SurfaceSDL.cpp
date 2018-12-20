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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

extern SDL_Renderer *renderer;

namespace cbe
{
  SurfaceSDL::SurfaceSDL(const char *filename)
  {
    surface = IMG_Load(filename);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
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
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_Rect orig;
    orig.x=0;
    orig.y=0;
    orig.w=surface->w;
    orig.h=surface->h;

    SDL_SetTextureAlphaMod(texture, opacity);
    SDL_RenderCopy(renderer, texture, &orig, &rect);
    SDL_SetTextureAlphaMod(texture, SDL_ALPHA_OPAQUE);
  }
  void SurfaceSDL::scaled_blit(int x, int y, float scale_x, float scale_y, uint8_t opacity)
  {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surface->w * scale_x;
    rect.h = surface->h * scale_y;

    SDL_Rect orig;
    orig.x=0;
    orig.y=0;
    orig.w=surface->w;
    orig.h=surface->h;

    SDL_SetTextureAlphaMod(texture, opacity);
    SDL_RenderCopy(renderer, texture, &orig, &rect);
    SDL_SetTextureAlphaMod(texture, SDL_ALPHA_OPAQUE);
  }
};
