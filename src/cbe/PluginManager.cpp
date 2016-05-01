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

#include "PluginManager.h"

#include <iostream>
#include <string>

#include "Factories.hpp"
#include "SDL/FactoriesSDL.hpp"

namespace cbe
{
  void RegisterSDL(PluginManager *PM)
  {
    //Register SDL sound
    PM->registerPluginFactory(new AudioSimpleFactorySDL());
  }

  void PluginManager::registerPluginFactory(AudioSimpleFactory *ASF)
  {
    std::cout << "Pushing " << ASF->getName() << std::endl;
    ASFactories.push_back(ASF);
  }

  AudioSimple *PluginManager::createAudioSimple(const std::string &type)
  {
    std::list<AudioSimpleFactory*>::iterator ite;
    for(ite = ASFactories.begin(); ite != ASFactories.end(); ite++) {
      if((*ite)->getName() == type) {
        return (*ite)->create();
      }
    }
    return NULL;
  }

  void PluginManager::searchStatic()
  {
    std::cout << "Registering Plugins\n";
    RegisterSDL(this);
  }

  void PluginManager::searchDynamic()
  {

  }

  void PluginManager::search()
  {
    searchStatic();
    searchDynamic();
  }

  PluginManager::~PluginManager()
  {
    std::list<AudioSimpleFactory*>::iterator ite;
    ite = ASFactories.begin();
    while(ite != ASFactories.end()) {
      delete *ite;
    }
    ASFactories.clear();
  }
};
