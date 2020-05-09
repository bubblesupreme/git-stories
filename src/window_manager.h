// Copyright © 2020 Svetlana Emelianova
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "config.pb-c.h"
#include "objects.h"
#include "phisics.h"
#include "status.h"

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  GS_Folder *root;
  GS_Balancer *balancer;
  SDL_Color currentColor;
  SDL_Color targetColor;
} GS_WindowManager;

GS_Status *GS_CreateWindowManager(int w, int h, GS_WindowManager **out);

void GS_DestroyWindowManager(GS_WindowManager *wm);

GS_Status *GS_UpdateWindowManager(GS_WindowManager *wm);

GS_Status *GS_UpdateColors(GS_WindowManager *wm);

GS_Status *GS_UpdateObjects(GS_WindowManager *wm, Config__CommitInfo *commit);
