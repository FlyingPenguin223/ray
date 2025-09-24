#include "entity.h"
#include "input.h"
#include "raycast.h"
#include <SDL2/SDL_scancode.h>

void update_player(entity* this) {
	float spd = 0.1;
	if (key_down(SDL_SCANCODE_W)) {
		this->pos.x += cos(this->dir) * spd;
		this->pos.y += sin(this->dir) * spd;
	} else if (key_down(SDL_SCANCODE_S)) {
		this->pos.x -= cos(this->dir) * spd;
		this->pos.y -= sin(this->dir) * spd;
	}

	if (key_down(SDL_SCANCODE_D)) {
		this->pos.x += cos(this->dir + M_PI/2) * spd;
		this->pos.y += sin(this->dir + M_PI/2) * spd;
	} else if (key_down(SDL_SCANCODE_A)) {
		this->pos.x += cos(this->dir - M_PI/2) * spd;
		this->pos.y += sin(this->dir - M_PI/2) * spd;
	}

	if (key_down(SDL_SCANCODE_LEFT)) {	
		this->dir -= 0.03;
	} else if (key_down(SDL_SCANCODE_RIGHT)) {
		this->dir += 0.03;
	}

	if (key_down(SDL_SCANCODE_SPACE)) {
		this->z += 0.1;
	} else {
		this->z -= 0.1;
	}

	float max_z = 0.49;
	this->z = this->z > max_z ? max_z : this->z < -max_z ? -max_z : this->z;

	this->dir = fixmod(this->dir, 2*M_PI);
}
