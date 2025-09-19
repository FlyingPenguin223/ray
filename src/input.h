#pragma once

#include <SDL2/SDL.h>

void init_keys();

void parse_event(SDL_Event e);

short is_key_down(enum SDL_Scancode code);
