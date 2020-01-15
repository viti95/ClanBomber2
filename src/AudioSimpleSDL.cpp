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
#include "MusicSDL.h"

#include <SDL2/SDL_mixer.h>

#include "AudioBufferSDL.h"

namespace cbe
{
  bool AudioSimpleSDL::init()
  {
    if (!initialized) {
      if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096)) {
        Mix_CloseAudio();
        return false;
      }
    }
    initialized++;
    return true;
  }

  void AudioSimpleSDL::close()
  {
    if (initialized = 1) {
      Mix_CloseAudio();
    }
    if (initialized > 0) {
      initialized--;
    }
  }

  AudioSimpleSDL::AudioSimpleSDL()
  {
    initialized = 0;
  }

  AudioSimpleSDL::~AudioSimpleSDL()
  {
    Mix_CloseAudio();
  }

  AudioBuffer *AudioSimpleSDL::createBuffer(std::filesystem::path filename)
  {
    return new AudioBufferSDL(filename);
  }
  
  Music *AudioSimpleSDL::createMusic(std::filesystem::path filename)
  {
	return new MusicSDL(filename);
  }
};
