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

#ifndef FACTORYMESS_HPP
#define FACTORYMESS_HPP

#include <string>

#include "AudioSimple.h"

namespace cbe
{
  //Teplate for the creation of interfaces for subsystems
  template<typename T>
  class PluginFactoryAbstract
  {
  public:
    virtual T *create() = 0;
  };

  //Teplate for the creation of implementations for subsystems
  //P = Plugin Implementation
  //PA = Plugin Abstract
  template<typename P, typename PA>
  class PluginFactoryImplementation : public PluginFactoryAbstract<PA>
  {
  public:
    PA *create()
    {
      return new P();
    }
  };

  typedef PluginFactoryAbstract<AudioSimple> AudioSimpleFactory;
};

#endif
