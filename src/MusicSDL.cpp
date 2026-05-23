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
#include <cstdio>
#include <SDL3/SDL.h>

namespace cbe
{
  MusicSDL::MusicSDL(std::filesystem::path filename, MIX_Mixer *mixer)
      : audio(nullptr), mixer(mixer), track(nullptr)
  {
    audio = MIX_LoadAudio(mixer, filename.string().c_str(), false);
    if (!audio)
    {
      printf("Cannot load music file: %s\n", SDL_GetError());
    }
  }

  MusicSDL::~MusicSDL()
  {
    if (track)
      MIX_DestroyTrack(track);
    MIX_DestroyAudio(audio);
  }

  bool MusicSDL::isPlaying()
  {
    if (!track)
      return false;
    return MIX_TrackPlaying(track);
  }

  void MusicSDL::playLoop()
  {
    if (!audio || !mixer)
      return;

    if (track)
      MIX_DestroyTrack(track);

    track = MIX_CreateTrack(mixer);
    MIX_SetTrackAudio(track, audio);
    MIX_PlayTrack(track, 0);
  }

  void MusicSDL::stop()
  {
    if (track)
    {
      MIX_StopTrack(track, 0);
      MIX_DestroyTrack(track);
      track = nullptr;
    }
  }
}