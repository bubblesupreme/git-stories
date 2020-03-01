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

#include "phisics.h"
GS_Status *GS_CreateBalancer(GS_Balancer **out) {
  GS_Balancer *balancer = malloc(sizeof(GS_Balancer));
  GS_NOT_NULL(balancer)
  balancer->objects_count = 0;
  balancer->objects_capacity = GS_INITIAL_BALANCER_CAPACITY;
  balancer->objects =
      malloc(sizeof(GS_CircularObject *) * balancer->objects_capacity);
  GS_NOT_NULL(balancer->objects)
  balancer->directions = malloc(sizeof(GS_Vec2) * balancer->objects_capacity);
  GS_NOT_NULL(balancer->directions)
  *out = balancer;
  return GS_Ok();
}

void GS_ClearBalancer(GS_Balancer *balancer) { balancer->objects_count = 0; }

void GS_DestroyBalancer(GS_Balancer *balancer) {
  free(balancer->objects);
  free(balancer->directions);
  free(balancer);
}
