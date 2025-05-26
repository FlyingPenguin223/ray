#pragma once

#include "../../raycast.h"

void init_keys();

void parse_event(SDL_Event e);

short is_key_down(enum SDL_Scancode code);
