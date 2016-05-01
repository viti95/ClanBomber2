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

#ifndef FONT_H
#define FONT_H

#include <string>

namespace cbe
{
  /* Used to set font alignment
   * Similar to DFBSurfaceTextFlags for easier use in ClanBomber.
   * left, center, right indicates horizontal alignment
   * top, bottom indicates vertical alignment but if neither are provided
   * baseline is used as default.
   */
  enum FontAlignment {
    FontAlignment_0left   = 0,   //left, baseline
    FontAlignment_0center = 1,   //center, baseline
    FontAlignment_0right  = 2,   //right, baseline

    FontAlignment_0top    = 4,   //not used by itself, but same as topleft
    FontAlignment_0bottom = 8,   //not used by itself, but same as bottomleft

    //the 3 horizontal alignments with horizontal top alignment
    FontAlignment_0topleft      = FontAlignment_0top | FontAlignment_0left,
    FontAlignment_0topcenter    = FontAlignment_0top | FontAlignment_0center,
    FontAlignment_0topright     = FontAlignment_0top | FontAlignment_0right,

    //the 3 horizontal alignments with horizontal bottom alignment
    FontAlignment_0bottomleft   = FontAlignment_0bottom | FontAlignment_0left,
    FontAlignment_0bottomcenter = FontAlignment_0bottom | FontAlignment_0center,
    FontAlignment_0bottomright  = FontAlignment_0bottom | FontAlignment_0right
  };

  class Font
  {
  public:
    virtual ~Font() {};
    virtual void render(const std::string &text, int x, int y,
                        FontAlignment alignment = FontAlignment_0left) = 0;
    virtual void render(const std::wstring &text, int x, int y,
                        FontAlignment alignment = FontAlignment_0left) = 0;
    virtual void getSize(const std::string &text, int *w, int *h) = 0;
    virtual int getHeight() = 0;
  };
};
#endif
