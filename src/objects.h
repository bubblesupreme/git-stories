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
