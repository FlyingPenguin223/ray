#include <SDL2/SDL.h>

static short keys_down[SDL_NUM_SCANCODES];

void init_keys() {
    for (int i = 0; i < 0x80; i++) {
        keys_down[i] = 0;
    }
}

void parse_event(SDL_Event e) {
    switch(e.type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
            keys_down[e.key.keysym.scancode] = 1;
            break;
        case SDL_KEYUP:
            keys_down[e.key.keysym.scancode] = 0;
            break;
    }
}

short is_key_down(enum SDL_Scancode code) {
    return keys_down[code];
}
