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
#include "utils.h"

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

#define GS_PANIC_NOT_OK(expr)                                                  \
  {                                                                            \
    GS_Status *M_status = expr;                                                \
    if (M_status->code != GS_StatusCode_OK) {                                  \
      fprintf(stderr, "Panic at %s:%d. %s", __FILE__, __LINE__,                \
              M_status->message);                                              \
      GS_DestroyStatus(M_status);                                              \
      exit(-1);                                                                \
    }                                                                          \
  }

#define GS_WARN_NOT_OK(expr)                                                   \
  {                                                                            \
    GS_Status *M_status = expr;                                                \
    if (M_status->code != GS_StatusCode_OK) {                                  \
      fprintf(stderr, "Warning generated at %s:%d. %s", __FILE__, __LINE__,    \
              M_status->message);                                              \
      GS_DestroyStatus(M_status);                                              \
    }                                                                          \
  }

GS_Status *GS_Ok();

GS_Status *GS_FileNotFound(char *name);

GS_Status *GS_FolderNotFound(char *name);

GS_Status *GS_ObjectAlreadyExists(char *name);

void GS_DestroyStatus(GS_Status *status);