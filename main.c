#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include "raycast.h"
#include "camera.h"
#include "render.h"
#include "input.h"

void sdl_exit();
int sdl_init();

SDL_Texture* load_texture(const char* path);

void init_textures();
void free_textures();

SDL_Texture* get_wall_texture(int id);
SDL_Texture* get_entity_texture(int id);

const int num_wall_textures = 2;
SDL_Texture* wall_textures[2];
SDL_Surface* wall_surfaces[2];
const int num_entity_textures = 1;
SDL_Texture* entity_textures[1];

SDL_Window* g_window;
SDL_Renderer* g_renderer;

int main() {
	atexit(sdl_exit);

	if (sdl_init() < 0)
		return sdl_error();

	init_textures();
	init_floor_texture_data();

	raycast_camera cam;

	cam.pos = (vec2) {1.5, 1.5};
	cam.z = 0;
	cam.dir = 0; 
	cam.fov = deg2rad(100);

	entity_array* objects = init_entity_array();

	int loop = 1;

	entity* player = init_entity(objects, 0, 2.5, 2.5);
	int cam_movement = 1;

	while (loop) {
		Uint64 start = SDL_GetTicks64();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			parse_event(e);
		}

		if (cam_movement) {
			cam_debug_movement(&cam);
			// fprintf(stderr, "(%d, %d) ", (int) cam.pos.x, (int) cam.pos.y); cam position is fine
		} else {
			cam.pos = player->pos;
		}

		SDL_RenderClear(g_renderer);

		render(cam, objects);

		SDL_RenderPresent(g_renderer);

		Uint64 time = SDL_GetTicks64() - start;
		int sleep_time = 1000/FPS - time;
		if (sleep_time >= 0) {
			SDL_Delay(sleep_time);
		} else {
			printf("lag! %d ms over\n", -sleep_time);
		}
	}

	exit(0);
}

int sdl_init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return sdl_error();

	g_window = SDL_CreateWindow("raycast", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (g_window == NULL)
		return sdl_error();
	
	g_renderer = SDL_CreateRenderer(g_window, 0, 0);
	if (g_renderer == NULL)
		return sdl_error();
	return 1;
}

void sdl_exit() {
	free_textures();
	if (g_renderer)
		SDL_DestroyRenderer(g_renderer);
	if (g_window)
		SDL_DestroyWindow(g_window);
	SDL_Quit();
}

SDL_Texture* load_texture(const char* path) {
	SDL_Surface* surface = SDL_LoadBMP(path);
	if (surface == NULL) {
		sdl_error();
		return NULL;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);
	SDL_FreeSurface(surface);
	if (texture == NULL)
		sdl_error();
	return texture;
}

void init_textures() {
	wall_textures[0] = load_texture("./textures/wall.bmp");
	wall_textures[1] = load_texture("./textures/floor.bmp");

	wall_surfaces[0] = SDL_LoadBMP("./textures/wall.bmp");
	wall_surfaces[1] = SDL_LoadBMP("./textures/floor.bmp");

	entity_textures[0] = load_texture("./textures/camera.bmp");
}

extern SDL_Texture* floor_to_draw;
extern int* floor_pixels;

extern int* map;

void free_textures() {
	for (int i = 0; i < num_wall_textures; i++) {
		SDL_DestroyTexture(wall_textures[i]);
		SDL_FreeSurface(wall_surfaces[i]);
	}
	for (int i = 0; i < num_entity_textures; i++)
		SDL_DestroyTexture(entity_textures[i]);
	if (floor_to_draw != NULL)
		SDL_DestroyTexture(floor_to_draw);
	free_floor_texture_data();
}

SDL_Texture* get_wall_texture(int id) {
	return wall_textures[id];
}

SDL_Texture* get_entity_texture(int id) {
	return entity_textures[id];
}

