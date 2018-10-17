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

#include "Utils.h"
#include <SDL2/SDL_image.h>
#include "Resources.h"
#include <locale.h>

#include "config.h"
#include "gettext.h"

extern SDL_Renderer *renderer;

void CB_BlitSurface(SDL_Surface *sSurface, int x, int y)
{
    SDL_Texture *texture;
    SDL_CreateTextureFromSurface(renderer, sSurface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = sSurface->w;
    rect.h = sSurface->h;

    SDL_Rect orig = {0, 0, sSurface->w, sSurface->h};

    SDL_RenderCopy(renderer, texture, &orig, &rect);

    SDL_DestroyTexture(texture);
    //SDL_BlitSurface(sSurface, NULL, primary, &rect);
}

void CB_RenderText(TTF_Font *font, const std::wstring &text, int x, int y)
{
    SDL_Color color = { 0xFF, 0x00, 0x00};//Default color for text is white
    char *utf8text = SDL_iconv_string("", "wchar_t", (char*) text.c_str(), (text.length() + 1) * sizeof(wchar_t));
    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font, utf8text, color);
    CB_BlitSurface(textSurface, x, y);
    SDL_FreeSurface(textSurface);
    SDL_free(utf8text);
}

void CB_RenderText(TTF_Font *font, const char *text, int x, int y)
{
    SDL_Color color = { 0xFF, 0x00, 0x00};//Default color for text is white
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text, color);
    CB_BlitSurface(textSurface, x, y);
    SDL_FreeSurface(textSurface);
}

void CB_RenderText(TTF_Font *font, const std::string &text, int x, int y)
{
    CB_RenderText(font, text.c_str(), x, y);
}

void CB_RenderTextCenter(TTF_Font *font, const char *text, int x, int y)
{
    SDL_Color color = { 0x00, 0xFF, 0x00};//Default color for text is white
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text, color);
    if(textSurface == NULL) //nedded beacause when empty string TTF_RenderText_Solid returns NULL
        return;
    x -= textSurface->w/2;
    CB_BlitSurface(textSurface, x, y);
    SDL_FreeSurface(textSurface);
}

void CB_RenderTextCenter(TTF_Font *font, const std::string &text, int x, int y)
{
    CB_RenderTextCenter(font, text.c_str(), x, y);
}

void CB_RenderTextRight(TTF_Font *font, const char *text, int x, int y)
{
    SDL_Color color = { 0x00, 0x00, 0xFF};//Default color for text is white
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text, color);
    x -= textSurface->w;
    CB_BlitSurface(textSurface, x, y);
    SDL_FreeSurface(textSurface);
}

void CB_RenderTextRight(TTF_Font *font, const std::string &text, int x, int y)
{
    CB_RenderTextRight(font, text.c_str(), x, y);
}

void CB_FillRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b)
{
    if (w == 0 || h == 0) return;

    SDL_Surface *surface;
    surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_Rect orig = {0, 0, w, h};
    SDL_FillRect(surface, &orig, SDL_MapRGB(surface->format, r, g, b));
    SDL_Rect rect = {x, y, w, h};
    SDL_Texture *texture;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(renderer, texture, &orig, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    //SDL_FillRect(primary, &rect, SDL_MapRGB(primary->format, r, g, b) );
}

void CB_FillRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (w == 0 || h == 0 || a == 0) return;

    SDL_Surface *surface;
    surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_Rect orig = {0, 0, w, h};
    SDL_FillRect(surface, &orig, SDL_MapRGBA(surface->format, r, g, b, a));
    SDL_Rect rect = {x, y, w, h};
    SDL_Texture *texture;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_SetTextureAlphaMod(texture, a);
    SDL_RenderCopy(renderer, texture, &orig, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    /*SDL_Surface *tmpSurface = SDL_CreateRGBSurface(SDL_SRCALPHA,
                              w,
                              h,
                              16,
                              0, 0, 0,
                              0);
    SDL_SetAlpha(tmpSurface, SDL_SRCALPHA, a);
    SDL_FillRect(tmpSurface, NULL, SDL_MapRGB(tmpSurface->format, r, g, b) );
    CB_BlitSurface(tmpSurface, x, y);
    SDL_FreeSurface(tmpSurface);*/
}

void CB_WaitForKeypress()
{
    SDL_Event event;
    do
    {
        SDL_WaitEvent(&event);
    }
    while(event.type != SDL_KEYDOWN);
}

void CB_BatchBlit(SDL_Texture *texture, SDL_Rect *srcRects, SDL_Rect *destRects, int num)
{
    for(int i = 0; i < num; i++)
    {
        SDL_Rect orig = srcRects[i];
        SDL_Rect dest = destRects[i];

        SDL_RenderCopy(renderer, texture, &orig, &dest);
        //SDL_BlitSurface(src, &srcRects[i], primary, &destRects[i]);
    }
}

void CB_FillRects(SDL_Rect *rects, int num, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    for(int i = 0; i < num; i++)
    {
        if (a == 0xFF)
        {
            CB_FillRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h, r, g, b);
        }
        else
        {
            CB_FillRect(rects[i].x, rects[i].y, rects[i].w, rects[i].h, r, g, b, a);
        }
    }
}

int CB_EnterText(std::string &new_string)
{
    SDL_Event event;
    std::string str = new_string;
    int cursor = str.length();
    int cursorx = 0;

    while(SDL_WaitEvent(&event))
    {
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.scancode)
            {
            case SDL_SCANCODE_RETURN:
                new_string = str;
                return 1;
            case SDL_SCANCODE_ESCAPE:
                return -1;
            case SDL_SCANCODE_BACKSPACE:
                if(cursor > 0)
                {
                    str = str.substr(0, cursor - 1)+ str.substr(cursor, str.length());
                    cursor--;
                }
                break;
            case SDL_SCANCODE_DELETE:
                if(cursor < str.length())
                {
                    str = str.substr(0, cursor)+ str.substr(cursor + 1, str.length());
                }
                break;
            case SDL_SCANCODE_LEFT:
                if(cursor > 0)
                    cursor--;
                break;
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_HOME:
            case SDL_SCANCODE_PAGEUP:
                cursor = 0;
                break;
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_END:
            case SDL_SCANCODE_PAGEDOWN:
                cursor = str.length();
                break;
            case SDL_SCANCODE_RIGHT:
                if(cursor < str.length())
                    cursor++;
                break;
            default:
                
                if (event.key.keysym.scancode >= 4 && event.key.keysym.scancode <= 39){
                    char temp[2] = {0,0};
                    temp[0] = *(SDL_GetKeyName(event.key.keysym.sym));
                    str = str.substr(0, cursor) + temp + str.substr(cursor, str.length()-cursor);
                    cursor++;
                }

                break;
            }
        }
        CB_FillRect(0, 0, 800, 40, 70, 0 , 0);
        Resources::Font_big()->render(str, 0, 0, cbe::FontAlignment_0topleft);
        CB_FillRect(cursorx, 0, 2, Resources::Font_big()->getHeight(), 0xFF, 0xFF, 0xFF);
        CB_Flip();
    }

    //SDL_EnableUNICODE(0);
    //SDL_EnableKeyRepeat(0, 0);

    return 0;
}

void CB_Locale()
{
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, CB_LOCALEDIR);
    textdomain(PACKAGE);
}

void CB_Flip()
{
    //SDL_UpdateRect(primary, 0, 0, 0, 0);
    SDL_RenderPresent(renderer);
    //SDL_Flip(primary);
}
