#pragma once

#include <SDL2/SDL.h>

#include "vec.h"

typedef enum button_state {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_PRESSED,
} button_state;

#define NUM_MOUSE_BUTTONS 3

typedef enum mouse_button {
	MOUSE_BUTTON_LEFT = 1,
	MOUSE_BUTTON_MIDDLE = 2,
	MOUSE_BUTTON_RIGHT = 3,
} mouse_button;

typedef struct mouse_state {
	vec2 pos;
	button_state buttons[NUM_MOUSE_BUTTONS];
} mouse_state;

void init_keys();

void parse_event(SDL_Event e);

short key_down(enum SDL_Scancode code);
short key_pressed(enum SDL_Scancode code);

short mouse_down(mouse_button button);
short mouse_pressed(mouse_button button);
