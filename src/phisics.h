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
#include <stdbool.h>

#include "objects.h"
#include "status.h"
#include "utils.h"
#include "vector.h"

#define GS_INITIAL_BALANCER_CAPACITY 1024

typedef struct {
  size_t first;
  size_t second;
} GS_Pair;

typedef struct {
  GS_CircularObject **objects;
  GS_Vec2 *forces;
  bool *is_file;
  size_t objects_count;
  size_t objects_capacity;
  GS_Pair *connections;
  size_t connections_count;
  size_t connections_capacity;
} GS_Balancer;

GS_Status *GS_CreateBalancer(GS_Balancer **out);

void GS_ClearBalancer(GS_Balancer *balancer);

void GS_DestroyBalancer(GS_Balancer *balancer);

void GS_TraceObjects(GS_Balancer *balancer, GS_Folder *root);

void GS_Balance(GS_Balancer *balancer);
