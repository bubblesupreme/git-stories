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

#include <stdbool.h>
#include <stdlib.h>

#include "utils.h"

GS_Status *GS_CreateBalancer(GS_Balancer **out) {
  GS_Balancer *balancer = malloc(sizeof(GS_Balancer));
  GS_NOT_NULL(balancer)
  balancer->objects_count = 0;
  balancer->objects_capacity = GS_INITIAL_BALANCER_CAPACITY;
  balancer->connections_capacity = GS_INITIAL_BALANCER_CAPACITY;
  balancer->objects = malloc(sizeof(GS_Object *) * balancer->objects_capacity);
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
  // not really a coulomb force anymore, r^2 is made for better file allignment
  return q2 * q1 / r / r;
}

static double gukeForce(double k, double dl) { return -k * dl; }

static size_t traceObject(GS_Balancer *balancer, GS_Object *obj, bool is_file) {
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
    balancer->forces[i] = GS_VecMake(0, 0);
    GS_Vec2 pos = balancer->objects[i]->center;
    double q1 = balancer->is_file[i] ? GS_FILE_CHARGE : GS_FOLDER_CHARGE;
    for (int j = 0; j < balancer->objects_count; j++) {
      GS_Vec2 pos2 = balancer->objects[j]->center;
      GS_Vec2 res;
      GS_VecDif(&pos, &pos2, &res);
      double q2 = balancer->is_file[j] ? GS_FILE_CHARGE : GS_FOLDER_CHARGE;
      double len = GS_VecLen(&res);
      if (len == 0) {
        continue;
      }
      if (balancer->is_file[i] == balancer->is_file[j]) {
        // Ignore forces between files and folders to make better visualization
        double force = coulombForce(q1, q2, len);
        GS_VecNorm(&res, &res);
        GS_VecScalarMult(&res, force / 10, &res);
        GS_VecSum(&balancer->forces[i], &res, &balancer->forces[i]);
      }
    }
  }

  for (int i = 0; i < balancer->connections_count; i++) {
    size_t first = balancer->connections[i].first;
    size_t second = balancer->connections[i].second;
    double k = balancer->is_file[first] || balancer->is_file[second]
                   ? GS_FOLDER_FILE_TENSION
                   : GS_FOLDER_FOLDER_TENSION;
    GS_Vec2 pos1 = balancer->objects[first]->center;
    GS_Vec2 pos2 = balancer->objects[second]->center;
    GS_Vec2 res;
    GS_VecDif(&pos1, &pos2, &res);
    double len = GS_VecLen(&res);
    if (len == 0) {
      continue;
    }
    int spring_length =
        (!balancer->is_file[first] && !balancer->is_file[second])
            ? GS_FOLDER_FOLDER_SPRING_LENTH
            : GS_FILE_FOLDER_SPRING_LENTH;

    double force =
        gukeForce(k, len - (balancer->objects[first]->radius +
                            balancer->objects[second]->radius + spring_length));
    GS_VecNorm(&res, &res);
    GS_VecScalarMult(&res, force / 10, &res);
    if (balancer->is_file[first] || !balancer->is_file[second]) {
      GS_VecSum(&balancer->forces[first], &res, &balancer->forces[first]);
    }
    GS_VecScalarMult(&res, -1, &res);
    if (balancer->is_file[second] || !balancer->is_file[first]) {
      GS_VecSum(&balancer->forces[second], &res, &balancer->forces[second]);
    }
  }

  for (int i = 1; i < balancer->objects_count; i++) {
    GS_VecScalarMult(&balancer->objects[i]->speed, GS_SPEED_DAMPING,
                     &balancer->objects[i]->speed);
    GS_Vec2 acceleration = GS_VecMake(0.0, 0.0);
    GS_VecScalarDiv(&balancer->forces[i], balancer->objects[i]->mass,
                    &acceleration);
    GS_VecScalarDiv(&acceleration, GS_MICROTICKS_PER_TICK, &acceleration);
    GS_VecSum(&balancer->objects[i]->speed, &acceleration,
              &balancer->objects[i]->speed);
    GS_Vec2 speed = balancer->objects[i]->speed;

    GS_VecScalarDiv(&speed, GS_MICROTICKS_PER_TICK, &speed);
    GS_VecSum(&balancer->objects[i]->center, &speed,
              &balancer->objects[i]->center);
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
