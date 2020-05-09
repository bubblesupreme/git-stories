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

#include "vector.h"
#include <math.h>

void GS_VecSum(GS_Vec2 *lhs, GS_Vec2 *rhs, GS_Vec2 *res) {
  res->x = lhs->x + rhs->x;
  res->y = lhs->y + rhs->y;
}

void GS_VecDif(GS_Vec2 *lhs, GS_Vec2 *rhs, GS_Vec2 *res) {
  res->x = lhs->x - rhs->x;
  res->y = lhs->y - rhs->y;
}

void GS_VecScalarMult(GS_Vec2 *vec, double scalar, GS_Vec2 *res) {
  res->x = vec->x * scalar;
  res->y = vec->y * scalar;
}

void GS_VecScalarDiv(GS_Vec2 *vec, double scalar, GS_Vec2 *res) {
  GS_VecScalarMult(vec, 1.0 / scalar, res);
}

double GS_VecLen(GS_Vec2 *vec) {
  return sqrt(vec->x * vec->x + vec->y * vec->y);
}

void GS_VecNorm(GS_Vec2 *vec, GS_Vec2 *res) {
  double len = GS_VecLen(vec);
  GS_VecScalarDiv(vec, len, res);
}

GS_Vec2 GS_VecMake(double x, double y) {
  GS_Vec2 v;
  v.x = x;
  v.y = y;
  return v;
}
