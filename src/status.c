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

#include "status.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

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

GS_Status *GS_IncorrectArgc(int received, int correct) {
  GS_Status *status = allocStatus();
  status->code = GS_StatusCode_IncorrectArgc;
  snprintf(status->message, GS_STATUS_MAX_MESSAGE_SIZE,
           "Takes %d arguments, but %d were received", correct, received);
  return status;
}

void GS_DestroyStatus(GS_Status *status) {
  if (status == GS_StatusCode_OK)
    return;
  // we allocate status message in same memory with status
  // so we don't need to deallocate it
  free(status);
}
