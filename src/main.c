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

#include <SDL.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>
const int SECOND = 1000;

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    return -1;
  }
  SDL_Window *window =
      SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

  SDL_Renderer *rend = SDL_CreateRenderer(window, -1, 0);

  SDL_SetRenderDrawColor(rend, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(rend);
  SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
  filledCircleRGBA(rend, 100, 100, 100, 100, 100, 100, 100);
  SDL_RenderPresent(rend);

  int working = 1;
  while (working) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        working = 0;
        break;
      }
    }
  }

  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}