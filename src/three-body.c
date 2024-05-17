#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"

#include "raymath.h"

#define WIDTH 800
#define HEIGHT 600

#define BODY_SIZE 2
#define G 6.6743e-11
#define EPS 2

typedef struct {
  float mass;
  float x;
  float y;
  float z;
  float velX;
  float velY;
  float velZ;
  float forceX;
  float forceY;
  float forceZ;
} Body;

float distance(Body* a, Body* b) {
  return pow((a->x - b->x), 2) + pow((a->y - b->y), 2) + pow((a->z - b->z), 2);
}

void apply_forces(Body* stars, int n, float dt) {
  for(int i = 0; i < n; i++) {
    float starMass = stars[i].mass;
    stars[i].forceX = (-G * starMass) * stars[i].forceX;
    stars[i].forceY = (-G * starMass) * stars[i].forceY;
    stars[i].forceZ = (-G * starMass) * stars[i].forceZ;

    float a_x = stars[i].forceX / starMass;
    float a_y = stars[i].forceY / starMass;
    float a_z = stars[i].forceZ / starMass;
    
    stars[i].velX += dt * a_x;
    stars[i].velY += dt * a_y;
    stars[i].velZ += dt * a_z;

    stars[i].x += dt * stars[i].velX;
    stars[i].y += dt * stars[i].velY;
    stars[i].z += dt * stars[i].velZ;

    stars[i].forceX = 0.0;
    stars[i].forceY = 0.0;
    stars[i].forceZ = 0.0;
  }
}

void force(Body* s1, Body* s2) {
  float r = distance(s1, s2);
  if (r < EPS) {
    return;
  }
  float delta_x = s1->x - s2->x;
  float delta_y = s1->y - s2->y;
  float delta_z = s1->z - s2->z;

  float rx_hat = delta_x / sqrt(r); 
  float ry_hat = delta_y / sqrt(r);
  float rz_hat = delta_z / sqrt(r);
  float massByDist;
  float mass = s2->mass;
  massByDist  = (mass / r);
  s1->forceX +=  massByDist * rx_hat; 
  s1->forceY +=  massByDist * ry_hat; 
  s1->forceZ +=  massByDist * rz_hat; 
}

void sim(Body* bodies, int n, float dt) {
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      if (i != j) {
        force(&bodies[i], &bodies[j]);
        apply_forces(bodies, 3, dt);
      }
    }
  }
}

void draw_body(Body body) {
  DrawRectangle(body.x, body.y, BODY_SIZE, BODY_SIZE, RED);
}

int main(int argc, char** argv) {
  Body *bodies = (Body*)malloc(sizeof(Body)*3);
  float mass = 10000000000000000;
  bodies[0] = (Body) {
    .x = 200,
    .y = 200,
    .z = -250,
    .forceX = 0,
    .forceY = 0,
    .forceZ = 0,
    .mass = mass,
    .velX = 0,
    .velY = 0
  };
  bodies[1] = (Body) {
    .x = 200,
    .y = 250,
    .z = 250,
    .forceX = 0,
    .forceY = 0,
    .forceZ = 0,
    .mass = mass*0.8,
    .velX = 0,
    .velY = 0
  };
  bodies[2] = (Body) {
    .x = 275,
    .y = 225,
    .z = 0,
    .forceX = 0,
    .forceY = 0,
    .forceZ = 0,
    .mass = mass*0.7,
    .velX = 0,
    .velY = 0
  };

  InitWindow(WIDTH, HEIGHT, "Three-body");
  SetTargetFPS(60);

  while (!WindowShouldClose()) { 
    BeginDrawing();
    ClearBackground(BLACK);
    sim(bodies, 3, GetFrameTime());
    for(int i = 0; i < 3; i++) {
      draw_body(bodies[i]);
    }
    EndDrawing();
  }
  
  free(bodies);
}