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

#ifndef SURFACESDL_H
#define SURFACESDL_H
#include "Surface.h"
#include <SDL2/SDL.h>

namespace cbe
{
  class SurfaceSDL : public Surface
  {
  public:
    SurfaceSDL(const char *filename);
    ~SurfaceSDL();
    void blit(int x, int y, uint8_t opacity = 255);
    void scaled_blit(int x, int y, float scale_x, float scale_y,
                     uint8_t opacity = 255);
  private:
    SDL_Surface *surface;
    SDL_Texture *texture;
  };
};
#endif
