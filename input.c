#include <SDL2/SDL.h>

#include "input.h"

static button_state keys[SDL_NUM_SCANCODES];
static mouse_state mouse;

void init_keys() {
	for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
		keys[i] = 0;
	}
}

void init_mouse() {
	mouse.pos = (vec2) {0, 0};
	for (int i = 0; i < NUM_MOUSE_BUTTONS; i++) {
		mouse.buttons[i] = BUTTON_UP;
	}
}

void parse_event(SDL_Event e) {
	for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
		if (keys[i] == BUTTON_PRESSED)
			keys[i] = BUTTON_DOWN;
	}
	for (int i = 0; i < NUM_MOUSE_BUTTONS; i++) {
		if (mouse.buttons[i] == BUTTON_PRESSED)
			mouse.buttons[i] = BUTTON_DOWN;
	}
	switch(e.type) {
		case SDL_QUIT:
			exit(0);
			break;
		case SDL_KEYDOWN:
			keys[e.key.keysym.scancode] = BUTTON_PRESSED;
			break;
		case SDL_KEYUP:
			keys[e.key.keysym.scancode] = BUTTON_UP;
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse.buttons[e.button.button] = BUTTON_PRESSED;
			break;
		case SDL_MOUSEBUTTONUP:
			mouse.buttons[e.button.button] = BUTTON_UP;
			break;
		case SDL_MOUSEMOTION:
			mouse.pos.x = e.motion.x;
			mouse.pos.y = e.motion.y;
			break;
	}
}

short key_down(SDL_Scancode code) {
	return keys[code] != BUTTON_UP;
}

short key_pressed(SDL_Scancode code) {
	return keys[code] == BUTTON_PRESSED;
}

short mouse_down(mouse_button button) {
	return mouse.buttons[button] != BUTTON_UP;
}

short mouse_pressed(mouse_button button) {
	return mouse.buttons[button] == BUTTON_PRESSED;
}
