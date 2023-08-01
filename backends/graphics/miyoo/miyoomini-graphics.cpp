/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "backends/graphics/miyoo/miyoomini-graphics.h"

void MiyooMiniGraphicsManager::initGraphicsSurface() {
    _hwScreen = nullptr;
    int maxWidth = 640;
    int maxHeight = 480;
    int desiredWidth = _videoMode.hardwareWidth > maxWidth ? maxWidth : _videoMode.hardwareWidth;
    int desiredHeight = _videoMode.hardwareHeight > maxHeight ? maxHeight : _videoMode.hardwareHeight;
    _realHwScreen = SDL_SetVideoMode(desiredWidth, desiredHeight, 32, SDL_HWSURFACE);
    if (!_realHwScreen)
        return;

    _hwScreen = SDL_CreateRGBSurface(SDL_HWSURFACE, _videoMode.hardwareWidth, _videoMode.hardwareHeight,
                     _realHwScreen->format->BitsPerPixel,
                     _realHwScreen->format->Rmask,
                     _realHwScreen->format->Gmask,
                     _realHwScreen->format->Bmask,
                     _realHwScreen->format->Amask);
    if (!_hwScreen) {
        SDL_FreeSurface(_realHwScreen);
        _realHwScreen = nullptr;
        return;
    }
    _isDoubleBuf = false;
}

void MiyooMiniGraphicsManager::updateScreen(SDL_Rect *dirtyRectList, int actualDirtyRects) {
    SDL_Surface *renderSurface = _hwScreen;
    
    if (_hwScreen->w > 640 || _hwScreen->h > 480) {
        SDL_Surface *tempSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, _realHwScreen->w, _realHwScreen->h,
                         _realHwScreen->format->BitsPerPixel,
                         _realHwScreen->format->Rmask,
                         _realHwScreen->format->Gmask,
                         _realHwScreen->format->Bmask,
                         _realHwScreen->format->Amask);

        if (!tempSurface)
            return;

        float xRatio = _hwScreen->w / (float)_realHwScreen->w;
        float yRatio = _hwScreen->h / (float)_realHwScreen->h;

        for (int y = 0; y < _realHwScreen->h; ++y) {
            int srcY = y * yRatio;
            uint32_t *tempRow = &((uint32_t*)tempSurface->pixels)[y * tempSurface->w];
            uint32_t *srcRow = &((uint32_t*)_hwScreen->pixels)[srcY * _hwScreen->w];

            for (int x = 0; x < _realHwScreen->w; ++x) {
                int srcX = x * xRatio;
                tempRow[x] = srcRow[srcX];
            }
        }

        renderSurface = tempSurface;
    }

    SDL_BlitSurface(renderSurface, nullptr, _realHwScreen, nullptr);
    SDL_UpdateRects(_realHwScreen, actualDirtyRects, _dirtyRectList);

    if (renderSurface != _hwScreen) {
        SDL_FreeSurface(renderSurface);
    }
}

void MiyooMiniGraphicsManager::unloadGFXMode() {
    if (_realHwScreen) {
        SDL_FreeSurface(_realHwScreen);
        _realHwScreen = nullptr;
    }
    if (_hwScreen) {
        SDL_FreeSurface(_hwScreen);
        _hwScreen = nullptr;
    }
    SurfaceSdlGraphicsManager::unloadGFXMode();
}

void MiyooMiniGraphicsManager::getDefaultResolution(uint &w, uint &h) {	
	w = 640;	
	h = 480;	
}
