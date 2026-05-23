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

#include "AudioBufferSDL.h"
#include <cstdio>
#include <SDL3/SDL.h>

namespace cbe
{
  AudioBufferSDL::AudioBufferSDL(std::filesystem::path filename, MIX_Mixer *mixer)
      : audio(nullptr), mixer(mixer)
  {
    audio = MIX_LoadAudio(mixer, filename.string().c_str(), false);
    if (!audio)
    {
      printf("Cannot load sound file: %s\n", SDL_GetError());
    }
  }

  AudioBufferSDL::~AudioBufferSDL()
  {
    MIX_DestroyAudio(audio);
  }

  void AudioBufferSDL::play()
  {
    if (!audio || !mixer)
      return;

    MIX_Track *track = MIX_CreateTrack(mixer);
    if (!track)
      return;

    MIX_SetTrackAudio(track, audio);
    MIX_PlayTrack(track, 0);
  }
}