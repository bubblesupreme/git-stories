#pragma once
#include <errno.h>
#include <stdio.h>

#define GS_MAX_NAME_SIZE 128
#define GS_INITIAL_FOLDER_CAPACITY 16

#define GS_PANIC_ON_ERROR(expr)                                                \
  {                                                                            \
    errno_t M_err;                                                             \
    if ((M_err = expr) != 0) {                                                 \
      fprintf(stderr, "Panic at %s:%d. Message: %s", __FILE__, __LINE__,       \
              strerror(M_err));                                                \
      exit(-1);                                                                \
    }                                                                          \
  }

#define GS_NOT_NULL(expr)                                                      \
  {                                                                            \
    if (expr == NULL) {                                                        \
      fprintf(stderr, "NULL pointer at %s:%d", __FILE__, __LINE__);            \
      exit(-1);                                                                \
    }                                                                          \
  }
 