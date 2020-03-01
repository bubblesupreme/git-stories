#pragma once
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  GS_StatusCode_OK = 0,
  GS_StatusCode_NotFound = 1,
  GS_StatusCode_AlreadyExists = 2,
} GS_StatusCode;

typedef struct {
  GS_StatusCode code;
  char *message;
} GS_Status;

#define GS_STATUS_MAX_SIZE 512
#define GS_STATUS_MAX_MESSAGE_SIZE GS_STATUS_MAX_SIZE - sizeof(GS_Status)

#define GS_RETURN_NOT_OK(expr)                                                 \
  {                                                                            \
    GS_Status *M_status = expr;                                                \
    if (M_status->code != GS_StatusCode_OK) {                                  \
      return M_status;                                                         \
    }                                                                          \
  }

GS_Status *GS_Ok() {
  static GS_Status ok;
  ok.code = GS_StatusCode_OK;
  ok.message = NULL;
  return &ok;
}

static GS_Status *allocStatus() {
  void *mem = malloc(sizeof(GS_Status) + GS_STATUS_MAX_SIZE);
  GS_NOT_NULL(mem);
  GS_Status *status = mem;
  status->message = mem + sizeof(GS_Status);
  return status;
}

GS_Status *GS_FileNotFound(char *name) {
  GS_Status *status = allocStatus();
  status->code = GS_StatusCode_NotFound;
  snprintf(status->message, GS_STATUS_MAX_MESSAGE_SIZE, "File not found: %s",
           name);
  return status;
}

GS_Status *GS_FolderNotFound(char *name) {
  GS_Status *status = allocStatus();
  status->code = GS_StatusCode_NotFound;
  snprintf(status->message, GS_STATUS_MAX_MESSAGE_SIZE, "Folder not found: %s",
           name);
  return status;
}

GS_Status *GS_ObjectAlreadyExists(char *name) {
  GS_Status *status = allocStatus();
  status->code = GS_StatusCode_AlreadyExists;
  snprintf(status->message, GS_STATUS_MAX_MESSAGE_SIZE,
           "Object named '%s' already exists", name);
  return status;
}

void GS_DestroyStatus(GS_Status *status) {
  if (status == GS_StatusCode_OK)
    return;
  // we allocate status message in same memory with status
  // so we don't need to deallocate it
  free(status);
}