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

#include "window_manager.h"
#include <stdlib.h>

GS_Status *GS_CreateWindowManager(GS_WindowManager **out) {
  GS_Folder *root;
  GS_RETURN_NOT_OK(GS_CreateFolder("root", NULL, &root))
  GS_WindowManager *wm = malloc(sizeof(GS_WindowManager));
  GS_NOT_NULL(wm);
  wm->root = root;
  wm->window =
      SDL_CreateWindow("Git Stories", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL);
  GS_NOT_NULL(wm->window);
  wm->renderer = SDL_CreateRenderer(wm->window, -1, 0);
  GS_NOT_NULL(wm->renderer);
  return GS_Ok();
}
void GS_DestroyWindowManager(GS_WindowManager *wm) {
  SDL_DestroyWindow(wm->window);
  SDL_DestroyRenderer(wm->renderer);
  GS_DestroyFolder(wm->root);
  free(wm);
}
