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

#include "objects.h"
#include "status.h"
#include "window_manager.h"
#include "config.pb-c.h"

static void initFolder(GS_Folder *root) {
  char name[] = "A";

  for (int i = 0; i < 30; i++) {
    GS_File *file;
    GS_WARN_NOT_OK(GS_CreateFile(root, name, &file))
    name[0] ++;
  }

  GS_Folder *f;
  GS_WARN_NOT_OK(GS_CreateFolder("kek", root, &f))
}

int main(int argc, char *argv[]) {

  FILE *fp;

	fp = fopen("/home/svetlana/git/go-github/Temp/output.gs", "r+");

    fseek(fp, 0, SEEK_END);
    size_t sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

	uint8_t *data = malloc(sz);

	fread(data, sizeof(uint8_t), sz, fp);

    fclose (fp);

    Config__OutConfig *config = config__out_config__unpack(NULL, sz, data);

    for(int i=0; i<config->n_commits; i++) {
        for(int j=0; j<config->commits[i]->n_newfiles; j++) {
            printf("%s\n", config->commits[i]->newfiles[j]);
        }
    }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }
  GS_WindowManager *window_manager;
  GS_PANIC_NOT_OK(GS_CreateWindowManager(1920, 1080, &window_manager))
  initFolder(window_manager->root);

  bool working = true;
  while (working) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        working = false;
      }
    }
    GS_WARN_NOT_OK(GS_UpdateWindowManager(window_manager))
  }

  GS_DestroyWindowManager(window_manager);
  SDL_Quit();
  return 0;
}
