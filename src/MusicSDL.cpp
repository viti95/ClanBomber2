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

#include "MusicSDL.h"

namespace cbe
{
  MusicSDL::MusicSDL(std::filesystem::path filename)
  {
    msc = Mix_LoadMUS(filename.string().c_str());
    if(msc == NULL) {
      printf("Cannot load music file");
      //TODO launch an exception
    }
  }

  MusicSDL::~MusicSDL()
  {
    Mix_FreeMusic(msc);
    msc=NULL;
  }

  bool MusicSDL::isPlaying()
  {
    if (msc == NULL) return false;

    return Mix_PlayingMusic();
  }

  void MusicSDL::playLoop()
  {
    if (msc == NULL) return;

    Mix_PlayMusic(msc, -1);
  }

  void MusicSDL::stop()
  {
    if (msc == NULL) return;

    Mix_HaltMusic();
  }
};
