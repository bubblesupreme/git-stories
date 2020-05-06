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

#include "render.h"

GS_Status *renderLine(SDL_Renderer *renderer, const GS_CircularObject *obj1, const GS_CircularObject *obj2) {
  lineRGBA(renderer, obj1->center.x, obj1->center.y, obj2->center.x, obj2->center.y,
                   0, 0, 0, 255);
  return GS_Ok();
}

GS_Status *renderCircle(SDL_Renderer *renderer, const GS_CircularObject *obj) {
  filledCircleRGBA(renderer, obj->center.x, obj->center.y, obj->radius,
                   obj->color.r, obj->color.g, obj->color.b, obj->color.a);
  return GS_Ok();
}

GS_Status *GS_RenderFile(SDL_Renderer *renderer, const GS_File *file) {
  GS_RETURN_NOT_OK(renderCircle(renderer, &file->obj))
  return GS_Ok();
}

GS_Status *GS_RenderFolder(SDL_Renderer *renderer, const GS_Folder *folder) {
  for (size_t i = 0; i < folder->files_count; i++) {
    GS_RETURN_NOT_OK(renderLine(renderer, &folder->obj, &folder->files[i]->obj))
    GS_RETURN_NOT_OK(GS_RenderFile(renderer, folder->files[i]))
  }
  for (size_t i = 0; i < folder->folders_count; i++) {
    GS_RETURN_NOT_OK(renderLine(renderer, &folder->obj, &folder->folders[i]->obj))
    GS_RETURN_NOT_OK(GS_RenderFolder(renderer, folder->folders[i]))
  }
  GS_RETURN_NOT_OK(renderCircle(renderer, &folder->obj))
  return GS_Ok();
}
