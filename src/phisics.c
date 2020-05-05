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
#include <math.h>
#include <stdbool.h>

#define K_FoFi 1
#define K_FoFo 1
#define Q_Fi 1
#define Q_Fo 10

static void vecSum(GS_Vec2 *lhs, GS_Vec2 *rhs, GS_Vec2 *res) {
  res->x = lhs->x + rhs->x;
  res->y = lhs->y + rhs->y;
}

static void vecDif(GS_Vec2 *lhs, GS_Vec2 *rhs, GS_Vec2 *res) {
  res->x = lhs->x - rhs->x;
  res->y = lhs->y - rhs->y;
}

static void vecScalarMult(GS_Vec2 *vec, double scalar, GS_Vec2 *res) {
  vec->x *= scalar;
  vec->y *= scalar;
}

static void vecScalarDiv(GS_Vec2 *vec, double scalar, GS_Vec2 *res) {
  vecScalarMult(vec, 1.0 / scalar, res);
}

static double vecLen(GS_Vec2 *vec) {
  return sqrt(vec->x * vec->x + vec->y * vec->y);
}

static void vecNorm(GS_Vec2 *vec, GS_Vec2 *res) {
  double len = vecLen(vec);
  vecScalarDiv(vec, len, res);
}

static GS_Vec2 vecMake(double x, double y) {
  GS_Vec2 v;
  v.x = x;
  v.y = y;
  return v;
}

GS_Status *GS_CreateBalancer(GS_Balancer **out) {
  GS_Balancer *balancer = malloc(sizeof(GS_Balancer));
  GS_NOT_NULL(balancer)
  balancer->objects_count = 0;
  balancer->objects_capacity = GS_INITIAL_BALANCER_CAPACITY;
  balancer->connections_capacity = GS_INITIAL_BALANCER_CAPACITY;
  balancer->objects =
      malloc(sizeof(GS_CircularObject *) * balancer->objects_capacity);
  GS_NOT_NULL(balancer->objects)
  balancer->forces = malloc(sizeof(GS_Vec2) * balancer->objects_capacity);
  GS_NOT_NULL(balancer->forces);
  balancer->is_file = malloc(sizeof(bool) * balancer->objects_capacity);
  GS_NOT_NULL(balancer->is_file);
  balancer->connections =
      malloc(sizeof(GS_Pair) * balancer->connections_capacity);
  GS_NOT_NULL(balancer->connections);
  *out = balancer;
  return GS_Ok();
}

static double coulombForce(double q1, double q2, double r) {
  return 8.99 * q1 / r;
}

static double gukeForce(double k, double dl) { return -k * dl; }

static size_t traceObject(GS_Balancer *balancer, GS_CircularObject *obj,
                          bool is_file) {
  if (balancer->objects_count == balancer->objects_capacity) {
    balancer->objects_capacity *= 2;
    balancer->objects = realloc(balancer->objects, balancer->objects_capacity);
    balancer->forces =
        realloc(balancer->forces, sizeof(GS_Vec2) * balancer->objects_capacity);
    balancer->is_file =
        realloc(balancer->is_file, sizeof(bool) * balancer->objects_capacity);
  }
  balancer->objects[balancer->objects_count] = obj;
  balancer->is_file[balancer->objects_count] = is_file;
  balancer->objects_count++;
  return balancer->objects_count - 1;
}

static void addConnection(GS_Balancer *balancer, size_t a, size_t b) {
  if (balancer->connections_count == balancer->connections_capacity) {
    balancer->connections_capacity *= 2;
    balancer->connections =
        realloc(balancer->connections, balancer->connections_capacity);
  }
  GS_Pair p;
  p.first = a;
  p.second = b;
  balancer->connections[balancer->connections_count++] = p;
}

static void traceObjectImpl(GS_Balancer *balancer, GS_Folder *root,
                            size_t root_ind) {
  int ind = traceObject(balancer, &root->obj, false);
  addConnection(balancer, ind, root_ind);
  for (int i = 0; i < root->files_count; i++) {
    addConnection(balancer, ind,
                  traceObject(balancer, &root->files[i]->obj, true));
  }
  for (int i = 0; i < root->folders_count; i++) {
    traceObjectImpl(balancer, root->folders[i], ind);
  }
}

void GS_TraceObjects(GS_Balancer *balancer, GS_Folder *root) {
  traceObjectImpl(balancer, root, 0);
}

void GS_Balance(GS_Balancer *balancer) {
  for (int i = 0; i < balancer->objects_count; i++) {
    balancer->forces[i] = vecMake(0, 0);
    GS_Vec2 pos = balancer->objects[i]->center;
    for (int j = 0; j < balancer->objects_count; j++) {
      GS_Vec2 pos2 = balancer->objects[j]->center;
      GS_Vec2 res;
      vecDif(&pos, &pos2, &res);
      double q1 = balancer->is_file[i] ? Q_Fi : Q_Fo;
      double q2 = balancer->is_file[j] ? Q_Fi : Q_Fo;
      double len = vecLen(&res);
      if (len == 0) {
        continue;
      }
      double force = coulombForce(q1, q2, len);
      vecNorm(&res, &res);
      vecScalarMult(&res, force / 10, &res);
      vecSum(&balancer->forces[i], &res, &balancer->forces[i]);
    }
  }

  for (int i = 0; i < balancer->connections_count; i++) {
    size_t first = balancer->connections[i].first;
    size_t second = balancer->connections[i].second;
    double k =
        balancer->is_file[first] || balancer->is_file[second] ? K_FoFi : K_FoFo;
    GS_Vec2 pos1 = balancer->objects[first]->center;
    GS_Vec2 pos2 = balancer->objects[second]->center;
    GS_Vec2 res;
    vecDif(&pos1, &pos2, &res);
    double len = vecLen(&res);
    if (len == 0) {
      continue;
    }
    double force = gukeForce(k, len - (balancer->objects[first]->radius + balancer->objects[second]->radius) * 4);
    vecNorm(&res, &res);
    vecScalarMult(&res, force / 10, &res);
    if (balancer->is_file[first] || !balancer->is_file[second]) {
      vecSum(&balancer->forces[first], &res, &balancer->forces[first]);
    }
    vecScalarMult(&res, -1, &res);
    if (balancer->is_file[second] || !balancer->is_file[first]) {
      vecSum(&balancer->forces[second], &res, &balancer->forces[second]);
    }
  }
  for (int i = 1; i < balancer->objects_count; i++) {
    balancer->objects[i]->center.x += balancer->forces[i].x;
    balancer->objects[i]->center.y += balancer->forces[i].y;
  }
}

void GS_ClearBalancer(GS_Balancer *balancer) {
  balancer->objects_count = 0;
  balancer->connections_count = 0;
}

void GS_DestroyBalancer(GS_Balancer *balancer) {
  free(balancer->objects);
  free(balancer->forces);
  free(balancer->connections);
  free(balancer->is_file);
  free(balancer);
}
