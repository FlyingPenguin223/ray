#include "SDL2/SDL.h"

#pragma once

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define FPS 60

#define sign(X) ( X > 0 ? 1 : X < 0 ? -1 : 0 )

#define min(a, b) ( a > b ? b : a )
#define max(a, b) ( a > b ? a : b )

#define deg2rad(X) (X * (M_PI / 180))
#define rad2deg(X) (X * (180 / M_PI))

typedef struct vec2 {
    float x;
    float y;
} vec2;

typedef struct raycast_camera {
    vec2 pos;
    float dir;
    float fov;
} raycast_camera;

float vec2rad(vec2 input);
float vec2deg(vec2 input);

vec2 rad2vec(float input);
vec2 deg2vec(float input);

float fixmod(float a, float b);

int sdl_error();

#include "src/include/entity.h"
#include "src/include/render.h"
#include "src/include/map.h"
#include "src/include/input.h"
#include "src/tiled2c.h"
