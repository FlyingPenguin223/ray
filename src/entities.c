#include "../raycast.h"
#include <SDL2/SDL_scancode.h>

void cam_debug_movement(raycast_camera* cam) {
    float spd = 0.1;
    if (is_key_down(SDL_SCANCODE_W)) {
        cam->pos.x += cos(cam->dir) * spd;
        cam->pos.y += sin(cam->dir) * spd;
    } else if (is_key_down(SDL_SCANCODE_S)) {
        cam->pos.x -= cos(cam->dir) * spd;
        cam->pos.y -= sin(cam->dir) * spd;
    }

    if (is_key_down(SDL_SCANCODE_D)) {
        cam->pos.x += cos(cam->dir + M_PI/2) * spd;
        cam->pos.y += sin(cam->dir + M_PI/2) * spd;
    } else if (is_key_down(SDL_SCANCODE_A)) {
        cam->pos.x += cos(cam->dir - M_PI/2) * spd;
        cam->pos.y += sin(cam->dir - M_PI/2) * spd;
    }

    if (is_key_down(SDL_SCANCODE_LEFT)) {   
        cam->dir -= 0.03;
    } else if (is_key_down(SDL_SCANCODE_RIGHT)) {
        cam->dir += 0.03;
    }

    cam->dir = fixmod(cam->dir, 2*M_PI);
}
