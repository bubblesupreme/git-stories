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
#include <stdint.h>
#include <stdio.h>

#include "SDL_pixels.h"
#include "vector.h"

#define GS_MAX_NAME_SIZE 128
#define GS_INITIAL_FOLDER_CAPACITY 16
#define GS_FOLDER_RADIUS 32
#define GS_FILE_RADIUS 16
#define GS_FOLDER_MASS 32.0
#define GS_FILE_MASS 1.0
#define GS_TICS_PER_SECOND 30
// TODO: differentiate ticks and microtics
// we don't need ticks to influence phisics speed as it now
#define GS_MICROTICKS_PER_TICK 30
#define GS_COMMITS_INTERVAL 7
#define GS_SPEED_DAMPING 0.99

#define GS_FOLDER_CHARGE 4000
#define GS_FILE_CHARGE 120
#define GS_FOLDER_FOLDER_TENSION 20
#define GS_FOLDER_FILE_TENSION 20

#define GS_FILE_FOLDER_SPRING_LENTH 64
#define GS_FOLDER_FOLDER_SPRING_LENTH 256

#define GS_PANIC_ON_ERROR(expr)                                                \
  {                                                                            \
    int M_err = expr;                                                          \
    if (M_err != 0) {                                                          \
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

SDL_Color GS_MakeSDLColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

SDL_Color GS_MakeSDLColorRGB(uint8_t r, uint8_t g, uint8_t b);

SDL_Color GS_MakeSDLColorGrey(uint8_t contrast);

SDL_Color GS_CalculateColor(int32_t errors);

void GS_UpdateColor(SDL_Color *curColor, SDL_Color *targetColor, int8_t step);

uint8_t GS_UpdateColorComponent(uint8_t curComponent, uint8_t targetComponent,
                                int8_t step);

typedef struct GS_Status GS_Status;
GS_Status *GS_CheckArgc(int argc);

void GS_RandomCirclePoint(GS_Vec2 *center, int radius, GS_Vec2 *res);
