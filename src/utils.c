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

#include "utils.h"

#include <math.h>
#include <stdlib.h>

#include "status.h"

SDL_Color GS_MakeSDLColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  SDL_Color result;
  result.r = r;
  result.g = g;
  result.b = b;
  result.a = a;
  return result;
}

SDL_Color GS_MakeSDLColorRGB(uint8_t r, uint8_t g, uint8_t b) {
  return GS_MakeSDLColorRGBA(r, g, b, 255);
}

SDL_Color GS_MakeSDLColorGrey(uint8_t contrast) {
  return GS_MakeSDLColorRGB(contrast, contrast, contrast);
}

SDL_Color GS_CalculateColor(int32_t errors) {
  uint8_t r = 255;
  uint8_t g = 255;
  if (errors > 50) {
    g = g / 100 * errors;
  }
  if (errors < 50) {
    r = r / 100 * errors;
  }
  return GS_MakeSDLColorRGB(r, g, 0);
}

void GS_UpdateColor(SDL_Color *curColor, SDL_Color *targetColor, int8_t step) {
  curColor->r = GS_UpdateColorComponent(curColor->r, targetColor->r, step);
  curColor->g = GS_UpdateColorComponent(curColor->g, targetColor->g, step);
  curColor->b = GS_UpdateColorComponent(curColor->b, targetColor->b, step);
}

uint8_t GS_UpdateColorComponent(uint8_t curComponent, uint8_t targetComponent,
                                int8_t step) {
  if (curComponent == targetComponent) {
    return targetComponent;
  }
  uint8_t res;
  int diff = targetComponent - curComponent;
  if (diff < 0) {
    step = -step;
  }
  if (abs(diff) > abs(step)) {
    res = (curComponent + step) > 0 ? curComponent + step : 0;
  } else {
    res = targetComponent;
  }
  return res;
}

GS_Status *GS_CheckArgc(int argc) {
  int correctArgc = 2;
  if (argc != correctArgc) {
    return GS_IncorrectArgc(argc, correctArgc);
  }
  return GS_Ok();
}

void GS_RandomCirclePoint(GS_Vec2 *center, int radius, GS_Vec2 *res) {
  // x*x + y*y = R*R
  float x = rand() % radius;
  if (rand() % 2) {
    x = -x;
  }
  float y = sqrt(radius * radius - x * x);
  if (rand() % 2) {
    y = -y;
  }
  res->x = center->x + x;
  res->y = center->y + y;
}