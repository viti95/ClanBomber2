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

#ifndef AUDIOSIMPLESDL_H
#define AUDIOSIMPLESDL_H

#include "AudioSimple.h"
#include "AudioBuffer.h"
#include "Music.h"

namespace cbe
{
  class AudioSimpleSDL : public AudioSimple
  {
  public:
    bool init();
    void close();
    AudioSimpleSDL();
    ~AudioSimpleSDL();
    AudioBuffer *createBuffer(std::filesystem::path filename);
    Music *createMusic(std::filesystem::path filename);
  private:
    unsigned int initialized;
  };
};

#endif
