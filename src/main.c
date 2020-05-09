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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "SDL_log.h"
#include "config.pb-c.h"
#include "status.h"
#include "time.h"
#include "utils.h"
#include "window_manager.h"

int main(int argc, char *argv[]) {

  GS_PANIC_NOT_OK(GS_CheckArgc(argc));

  // TODO: split logic to functions
  FILE *fp;
  fp = fopen(argv[1], "r+");

  if (!fp) {
    GS_PANIC_NOT_OK(GS_FileNotFound(argv[1]));
  }

  fseek(fp, 0, SEEK_END);
  size_t sz = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  uint8_t *data = malloc(sz);
  fread(data, sizeof(uint8_t), sz, fp);

  fclose(fp);

  Config__OutConfig *config = config__out_config__unpack(NULL, sz, data);
  free(data);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }
  GS_WindowManager *window_manager;
  GS_PANIC_NOT_OK(GS_CreateWindowManager(1920, 1080, &window_manager))

  time_t updateDelay = 5;
  uint8_t iCommit = 0;
  GS_WARN_NOT_OK(GS_UpdateObjects(window_manager, config->commits[iCommit]))
  time_t lastUpdateObj = time(NULL);
  bool working = true;
  while (working) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        working = false;
      }
    }
    if ((time(NULL) - lastUpdateObj) >= updateDelay &&
        iCommit < (config->n_commits - 1)) {
      GS_WARN_NOT_OK(
          GS_UpdateObjects(window_manager, config->commits[++iCommit]))
      lastUpdateObj = time(NULL);
    }

    GS_WARN_NOT_OK(GS_UpdateColors(window_manager))
    GS_WARN_NOT_OK(GS_UpdateWindowManager(window_manager))
  }
  config__out_config__free_unpacked(config, NULL);
  GS_DestroyWindowManager(window_manager);
  SDL_Quit();
  return 0;
}
