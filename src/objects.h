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
#include "status.h"
#include <SDL.h>
#include <stdbool.h>

typedef struct {
  SDL_Color color;
  SDL_Point center;
  uint32_t radius;
} GS_CircularObject;

typedef struct {
  GS_CircularObject obj;
  char name[GS_MAX_NAME_SIZE];
  uint64_t lines;
} GS_File;

typedef struct GS_Folder_ {
  GS_CircularObject obj;
  char name[GS_MAX_NAME_SIZE];

  GS_File **files;
  size_t files_count;
  size_t files_capacity;

  struct GS_Folder_ **folders;
  size_t folders_count;
  size_t folders_capacity;
} GS_Folder;

bool GS_NameExists(GS_Folder *root, char *name);

GS_Status *GS_FindFile(GS_Folder *folder, char *name, GS_File **result);

GS_Status *GS_FindFolder(GS_Folder *root, char *name, GS_Folder **result);

GS_Status *GS_CreateFolder(char *name, GS_Folder *parent, GS_Folder **out);

GS_Status *GS_CreateFile(GS_Folder *folder, char *name, GS_File **out);

void GS_DestroyFolder(GS_Folder *folder);

GS_Status *GS_RemoveFile(GS_Folder *folder, char *filename);
