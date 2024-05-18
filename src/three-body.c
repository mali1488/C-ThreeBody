#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include "raylib.h"
#include "raymath.h"

#define WIDTH 800
#define HEIGHT 600

#define BODY_SIZE 2
#define G 6.67300e-11
#define EPS 10
#define N 3
#define TRAIL_LENGTH 1000

typedef struct {
  float mass;
  float x;
  float y;
  float z;
  float vx;
  float vy;
  float vz;
  float fx;
  float fy;
  float fz;
  Vector2 trail[TRAIL_LENGTH];
  size_t trail_pos;
  Color color;
} Body;

void draw_body_trail(Body body) {
  for (size_t i = 0; i < TRAIL_LENGTH - 1; i++) {
    size_t index = (body.trail_pos + i) % TRAIL_LENGTH;
    Vector2 c = body.trail[index];
    if (Vector2Equals(c, Vector2Zero())) {
      continue;
    }
    Vector2 cn = body.trail[(index + 1) % TRAIL_LENGTH];
    DrawLineBezier(c, cn, 1, body.color);
  }
}

void draw_body(Body body) {
  float s = body.z * 0.02;
  if (s < 1) s = 1;
  DrawCircle(body.x + s/2, body.y + s/2, s, body.color);
  draw_body_trail(body);
}

float distance(Body* a, Body* b) {
  return sqrt(pow((b->x - a->x), 2) + pow((b->y - a->y), 2) + pow((b->z - a->z), 2));
}

void apply_forces(Body* bodies, size_t n, float dt) {
  for(size_t i = 0; i < n; i++) {
    Body* b = &bodies[i];
    float m = b->mass;
    b->fy = (-G * m) * b->fy;
    b->fx = (-G * m) * b->fx;
    b->fz = (-G * m) * b->fz;

    float ax = b->fy / m;
    float ay = b->fx / m;
    float az = b->fz / m;
    
    b->vx += dt * ax;
    b->vy += dt * ay;
    b->vz += dt * az;

    Vector2 ct = b->trail[b->trail_pos];
    if ((int)ct.x != (int)b->x || (int)ct.y != (int)b->y) {
      b->trail[b->trail_pos] = (Vector2) {
        .x = b->x,
        .y = b->y
      };
      b->trail_pos = (b->trail_pos + 1) % TRAIL_LENGTH;
    }

    b->x += dt * b->vx;
    b->y += dt * b->vy;
    b->z += dt * b->vz;

    b->fy = 0.0;
    b->fx = 0.0;
    b->fz = 0.0;
  }
}

void force(Body* s1, Body* s2) {
  float d = distance(s1, s2);
  float delta_x = s1->x - s2->x;
  float delta_y = s1->y - s2->y;
  float delta_z = s1->z - s2->z;

  float m = s1->mass * s2->mass * G;
  float ds = pow(fabs(d) + EPS, 3);

  s1->fy += (m * delta_x) / ds;
  s1->fx += (m * delta_y) / ds;
  s1->fz += (m * delta_z) / ds;
}

void sim(Body* bodies, size_t n, float dt) {
  for(size_t i = 0; i < n; i++) {
    for(size_t j = 0; j < n; j++) {
      if (i == j) continue;
      force(&bodies[i], &bodies[j]);
      apply_forces(bodies, 3, dt);
    }
  }
}

void init_body(Body* b, float x, float y, float z, float mass, Color c) {
  *b = (Body) {0};
  b->x = x;
  b->y = y;
  b->z = z;
  b->mass = mass;
  b->color = c;
}

void init_bodies(Body* bodies) {
  float mass = 5000000000000;
  float cx = WIDTH / 2;
  float cy = HEIGHT / 2;
  float p = 80;
  init_body(&bodies[0], cx - p, cy - p, 250, mass, MAROON);
  init_body(&bodies[1], cx + p, cy + p, 100, mass*0.7, GOLD);
  init_body(&bodies[2], cx + p, cy - p, 0, mass*0.6, SKYBLUE);
}

size_t camera_mode = N;
Vector2 camera_pos(Body* bodies, size_t n) {
  if (camera_mode != N) {
    return (Vector2) { bodies[camera_mode].x, bodies[camera_mode].y };
  }
  int max_x = 0;
  int min_x = INT_MAX;
  int max_y = 0;
  int min_y = INT_MAX;
  for (size_t i = 0; i < n; i++) {
    Body b = bodies[i];
    if (b.x > max_x) max_x = b.x;
    if (b.x < min_x) min_x = b.x;

    if (b.y > max_y) max_y = b.y;
    if (b.y < min_y) min_y = b.y;
  }
  return (Vector2) { min_x + (max_x - min_x) / 2, min_y + (max_y - min_y) / 2 };
}

void camera_cycle() {
  camera_mode = (camera_mode + 1) % (N + 1);
}

int main(void) {
  Body bodies[N] = {0};
  init_bodies(bodies);
  
  InitWindow(WIDTH, HEIGHT, "Three-body");
  SetTargetFPS(60);
  Camera2D camera = { 0 };
  camera.offset = (Vector2){ WIDTH / 2, HEIGHT / 2 };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_R)) {
      init_bodies(bodies);
    }
    if (IsKeyPressed(KEY_M)) {
      camera_cycle();
    }

    BeginDrawing();
      camera.target = camera_pos(bodies, N);
      BeginMode2D(camera);
        ClearBackground(BLACK);
        sim(bodies, N, GetFrameTime());
        for(size_t i = 0; i < N; i++) {
          draw_body(bodies[i]);
        }
      EndMode2D();
    EndDrawing();
  }
}
