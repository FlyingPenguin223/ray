#include <SDL2/SDL.h>

#include "input.h"
#include "camera.h"
#include "raycast.h"

void cam_debug_movement(raycast_camera* cam) {
	float spd = 0.1;
	if (key_down(SDL_SCANCODE_W)) {
		cam->pos.x += cos(cam->dir) * spd;
		cam->pos.y += sin(cam->dir) * spd;
	} else if (key_down(SDL_SCANCODE_S)) {
		cam->pos.x -= cos(cam->dir) * spd;
		cam->pos.y -= sin(cam->dir) * spd;
	}

	if (key_down(SDL_SCANCODE_D)) {
		cam->pos.x += cos(cam->dir + M_PI/2) * spd;
		cam->pos.y += sin(cam->dir + M_PI/2) * spd;
	} else if (key_down(SDL_SCANCODE_A)) {
		cam->pos.x += cos(cam->dir - M_PI/2) * spd;
		cam->pos.y += sin(cam->dir - M_PI/2) * spd;
	}

	if (key_down(SDL_SCANCODE_LEFT)) {	
		cam->dir -= 0.03;
	} else if (key_down(SDL_SCANCODE_RIGHT)) {
		cam->dir += 0.03;
	}

	if (key_down(SDL_SCANCODE_UP)) {
		cam->z += 0.1;
	} else if (key_down(SDL_SCANCODE_DOWN)) {
		cam->z -= 0.1;
	}

	float max_z = 0.49;
	cam->z = cam->z > max_z ? max_z : cam->z < -max_z ? -max_z : cam->z;

	cam->dir = fixmod(cam->dir, 2*M_PI);
}
