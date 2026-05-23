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

#include "AudioSimpleSDL.h"
#include "AudioBufferSDL.h"
#include "MusicSDL.h"
#include <SDL3/SDL.h>

namespace cbe
{
  bool AudioSimpleSDL::init()
  {
    if (!initialized)
    {
      if (!MIX_Init())
      {
        return false;
      }

      SDL_AudioSpec hint;
      SDL_zerop(&hint);
      hint.freq = 44100;
      hint.format = SDL_AUDIO_S16;
      hint.channels = 2;

      mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &hint);
      if (!mixer)
      {
        MIX_Quit();
        return false;
      }
    }
    initialized++;
    return true;
  }

  void AudioSimpleSDL::close()
  {
    if (initialized == 1)
    {
      MIX_DestroyMixer(mixer);
      MIX_Quit();
    }
    if (initialized > 0)
    {
      initialized--;
    }
  }

  AudioSimpleSDL::AudioSimpleSDL() : initialized(0), mixer(nullptr) {}

  AudioSimpleSDL::~AudioSimpleSDL()
  {
    close();
  }

  AudioBuffer *AudioSimpleSDL::createBuffer(std::filesystem::path filename)
  {
    return new AudioBufferSDL(filename, mixer);
  }

  Music *AudioSimpleSDL::createMusic(std::filesystem::path filename)
  {
    return new MusicSDL(filename, mixer);
  }
}