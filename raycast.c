#include <SDL2/SDL.h>
#include <math.h>

#include "raycast.h"
#include "vec.h"


float vec2rad(vec2 input) {
    return atan2(input.y, input.x);
}

float vec2deg(vec2 input) {
    return rad2deg(atan2(input.y, input.x));
}

vec2 rad2vec(float input) {
    return (vec2) {cos(input), sin(input)};
}

vec2 deg2vec(float input) {
    return (vec2) {cos(deg2rad(input)), sin(deg2rad(input))};
}

float fixmod(float a, float b) {
    float res = fmodf(a, b);
    if (res < 0)
        res += b;
    return res;
}

int sdl_error() {
    SDL_Log("%s", SDL_GetError());
    return -1;
}
