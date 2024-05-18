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
  Vector3 pos;
  Vector3 vel;
  Vector3 force;
  Vector2 trail[TRAIL_LENGTH];
  size_t trail_pos;
  Color color;
} Body;

Vector2 Vector3ToVector2(Vector3 v) {
  return (Vector2) { v.x, v.y };
}

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
  float s = body.pos.z * 0.02;
  if (s < 1) s = 1;
  DrawCircle(body.pos.x + s/2, body.pos.y + s/2, s, body.color);
  draw_body_trail(body);
}

float distance(Body* a, Body* b) {
  return sqrt(
    pow((b->pos.x - a->pos.x), 2) + 
    pow((b->pos.y - a->pos.y), 2) + 
    pow((b->pos.z - a->pos.z), 2));
}

void apply_forces(Body* bodies, size_t n, float dt) {
  for(size_t i = 0; i < n; i++) {
    Body* b = &bodies[i];
    float m = b->mass;
    b->force = Vector3Scale(b->force, -G * m);

    Vector3 a = Vector3Scale(b->force, 1 / m);    
    b->vel = Vector3Add(
      b->vel,
      Vector3Scale(a, dt));

    Vector2 ct = b->trail[b->trail_pos];
    if ((int)ct.x != (int)b->pos.x || (int)ct.y != (int)b->pos.y) {
      b->trail[b->trail_pos] = Vector3ToVector2(b->pos);
      b->trail_pos = (b->trail_pos + 1) % TRAIL_LENGTH;
    }

    b->pos = Vector3Add(
      b->pos,
      Vector3Scale(b->vel, dt));
    b->force = Vector3Zero();
  }
}

void force(Body* s1, Body* s2) {
  float d = distance(s1, s2);
  float m = s1->mass * s2->mass * G;
  float ds = pow(fabs(d) + EPS, 3);
  Vector3 delta = Vector3Subtract(s1->pos, s2->pos);
  Vector3 delta_m = Vector3Scale(delta, m);
  Vector3 f = Vector3Scale(delta_m, 1 / ds);
  s1->force = Vector3Add(s1->force, f);
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
  b->pos = (Vector3) { x, y, z };
  b->mass = mass;
  b->color = c;
}

void init_bodies(Body* bodies) {
  float mass = 5000000000000;
  float cx = WIDTH / 2;
  float cy = HEIGHT / 2;
  float p = 80;
  srand(time(NULL));
  float r = (float)rand() / RAND_MAX;
  init_body(&bodies[0], cx - p, cy - p, 250, mass, MAROON);
  init_body(&bodies[1], cx + p, cy + p, 100, mass*r, GOLD);
  init_body(&bodies[2], cx + p, cy - p, 0, mass*r, SKYBLUE);
}

size_t camera_mode = N;
Vector2 camera_pos(Body* bodies, size_t n) {
  if (camera_mode != N) {
    return Vector3ToVector2(bodies[camera_mode].pos);
  }
  int max_x = 0;
  int min_x = INT_MAX;
  int max_y = 0;
  int min_y = INT_MAX;
  for (size_t i = 0; i < n; i++) {
    Body b = bodies[i];
    if (b.pos.x > max_x) max_x = b.pos.x;
    if (b.pos.x < min_x) min_x = b.pos.x;

    if (b.pos.y > max_y) max_y = b.pos.y;
    if (b.pos.y < min_y) min_y = b.pos.y;
  }
  return (Vector2) { min_x + (max_x - min_x) / 2, min_y + (max_y - min_y) / 2 };
}

void camera_cycle_mode() {
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
      camera_cycle_mode();
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
