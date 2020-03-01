#include "objects.h"
#include "utils.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

bool GS_NameExists(GS_Folder *root, char *name) {
// We can not create folder with the same name as file, so we don't need
// separate methods for folders and files
#define GS_CHECK_NAME_EXISTS(array)                                            \
  for (size_t i = 0; i < root->array##_count; i++) {                           \
    if (strncmp((*(root->array + i))->name, name, GS_MAX_NAME_SIZE) == 0) {    \
      return true;                                                             \
    }                                                                          \
  }
  GS_CHECK_NAME_EXISTS(files)
  GS_CHECK_NAME_EXISTS(folders)
  return false;
}

#define GS_FIND_INTERNAL(method, type, field)                                  \
  static GS_Status *method(GS_Folder *folder, char *name, type **result,       \
                           size_t *index) {                                    \
    for (size_t i = 0; i < folder->field##_count; i++) {                       \
      type *cur = folder->field[i];                                            \
      if (strncmp(cur->name, name, GS_MAX_NAME_SIZE) == 0) {                   \
        if (result)                                                            \
          *result = cur;                                                       \
        if (index)                                                             \
          *index = i;                                                          \
        return GS_Ok();                                                        \
      }                                                                        \
    }                                                                          \
    return type##NotFound(name);                                               \
  }

GS_FIND_INTERNAL(findFile, GS_File, files)
GS_FIND_INTERNAL(findFolder, GS_Folder, folders)

GS_Status *GS_FindFile(GS_Folder *folder, char *name, GS_File **result) {
  return findFile(folder, name, result, NULL);
}

GS_Status *GS_FindFolder(GS_Folder *root, char *name, GS_Folder **result) {
  return findFolder(root, name, result, NULL);
}

#define GS_ADD_TO_ARRAY_UNCHECKED(method, type, field)                         \
  static void method(GS_Folder *parent, type *obj) {                           \
    if (parent->field##_count == parent->field##_capacity) {                   \
      parent->field##_capacity *= 2;                                           \
      parent->field = realloc(parent->field, parent->field##_capacity);        \
      GS_NOT_NULL(parent->field);                                              \
    }                                                                          \
    parent->field[parent->field##_count++] = obj;                              \
  }

GS_ADD_TO_ARRAY_UNCHECKED(addFileToFolderUnchecked, GS_File, files)
GS_ADD_TO_ARRAY_UNCHECKED(addFolderToFolderUnchecked, GS_Folder, folders)

GS_Status *GS_CreateFolder(char *name, GS_Folder *parent, GS_Folder **out) {
  if (GS_NameExists(parent, name)) {
    return GS_ObjectAlreadyExists(name);
  }

  GS_Folder *result = malloc(sizeof(GS_Folder));
  GS_NOT_NULL(result)
  GS_PANIC_ON_ERROR(strcpy_s(result->name, GS_MAX_NAME_SIZE, name))

  result->files_count = 0;
  result->files_capacity = GS_INITIAL_FOLDER_CAPACITY;
  result->files = malloc(sizeof(GS_File *) * result->files_capacity);
  GS_NOT_NULL(result->files)

  result->folders_count = 0;
  result->folders_capacity = GS_INITIAL_FOLDER_CAPACITY;
  result->folders = malloc(sizeof(GS_File *) * result->files_capacity);
  GS_NOT_NULL(result->folders)

  if (parent) {
    addFolderToFolderUnchecked(parent, result);
  }
  *out = result;
  return GS_Ok();
}

GS_Status *GS_CreateFile(GS_Folder *folder, char *name, GS_File **out) {
  if (GS_NameExists(folder, name)) {
    return GS_ObjectAlreadyExists(name);
  }
  GS_File *file = malloc(sizeof(GS_File));
  GS_NOT_NULL(file)
  GS_PANIC_ON_ERROR(strcpy_s(file->name, GS_MAX_NAME_SIZE, name))
  file->lines = 0;
  addFileToFolderUnchecked(folder, file);
  *out = file;
  return GS_Ok();
}

static void destroyFile(GS_File *file) { free(file); }

void GS_DestroyFolder(GS_Folder *folder) {
  for (size_t i = 0; i < folder->files_count; i++) {
    destroyFile(folder->files[i]);
  }
  for (size_t i = 0; i < folder->folders_count; i++) {
    GS_DestroyFolder(folder->folders[i]);
  }
  free(folder->files);
  free(folder->folders);
  free(folder);
}

GS_Status *GS_RemoveFile(GS_Folder *folder, char *filename) {
  GS_File *file;
  size_t index;
  GS_RETURN_NOT_OK(findFile(folder, filename, &file, &index))
  for (size_t i = index; i < folder->files_count; i++) {
    folder->files[i] = folder->files[i + 1];
  }
  folder->files_count--;
  destroyFile(file);
}
