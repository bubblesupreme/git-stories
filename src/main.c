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