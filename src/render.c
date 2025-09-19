#include <SDL2/SDL.h>
#include <math.h>

#include "raycast.h"
#include "camera.h"
#include "entity.h"
#include "map.h"

extern SDL_Window* g_window;
extern SDL_Renderer* g_renderer;

extern SDL_Texture* get_wall_texture(int id);
extern SDL_Texture* get_entity_texture(int id);

typedef struct render_element {
	int type;
	float dist;
	float angle;
	float texture_offset;
	int id;
} render_element;

static render_element z_buffer[WINDOW_WIDTH + 100];
static int z_buffer_length = 0;

static float darkness_threshold = -1;

void shiftZBuffer(int startId) {
	for (int curId = z_buffer_length; curId > startId; curId--) {
		z_buffer[curId].id = z_buffer[curId-1].id;
		z_buffer[curId].angle = z_buffer[curId-1].angle;
		z_buffer[curId].dist = z_buffer[curId-1].dist;
		z_buffer[curId].type = z_buffer[curId-1].type;
		z_buffer[curId].texture_offset = z_buffer[curId-1].texture_offset;
	}
}

void insertRenderElement(int type, float dist, float angle, float texture_offset, int id) {
	int indexToInsert = z_buffer_length;
	for (int i = 0; i < z_buffer_length; i++) {
		if (dist > z_buffer[i].dist) {
			indexToInsert = i;
			break;
		}
	}

	shiftZBuffer(indexToInsert);
	z_buffer[indexToInsert].type = type;
	z_buffer[indexToInsert].dist = dist;
	z_buffer[indexToInsert].angle = angle;
	z_buffer[indexToInsert].id = id;
	z_buffer[indexToInsert].texture_offset = texture_offset;

	z_buffer_length++;
}

void draw_scaled_sprite(SDL_Texture* texture, float x, float y, float scale) {
	int texture_width;
	int texture_height;
	SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);
	SDL_Rect source = {0, 0, texture_width, texture_height};

	SDL_Rect dest = {
		x - scale/2,
		y - scale/2,
		scale,
		scale
	};

	SDL_RenderCopy(g_renderer, texture, &source, &dest);
}

SDL_Texture* floor_to_draw = NULL; //not static to be able to free, maybe change if I can figure it out
int* floor_pixels = NULL;

struct floor_texture_data {
	int* pixels;
	int w;
	int h;
};

static struct floor_texture_data floor_data[2];

extern int num_wall_textures;
extern SDL_Surface* wall_surfaces[];

void init_floor_texture_data() {
	for (int i = 0; i < num_wall_textures; i++) {
		SDL_Surface* surface = wall_surfaces[i];
		int* floor_pixels = malloc(surface->w * surface->h * sizeof(int));
		SDL_ConvertPixels(surface->w, surface->h, surface->format->format, surface->pixels, surface->pitch, SDL_PIXELFORMAT_RGBA8888, floor_pixels, surface->pitch);

		floor_data[i].pixels = floor_pixels;
		floor_data[i].w = surface->w;
		floor_data[i].h = surface->h;
	}
}

void free_floor_texture_data() {
	for (int i = 0; i < num_wall_textures; i++) {
		free(floor_data[i].pixels);
	}
}

void render(raycast_camera cam, entity_array* objects) {
	if (floor_to_draw == NULL)
		floor_to_draw = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

	int half_width = WINDOW_WIDTH / 2;
	int half_height = WINDOW_HEIGHT / 2;

	float dv = half_width / tanf(cam.fov / 2);

	int* pixels = NULL;
	int pitch;
	if (SDL_LockTexture(floor_to_draw, NULL, (void**) &pixels, &pitch) < 0)
		sdl_error();

	for (int y = 0; y < half_height; y++) {
		float ray_dir_0 = cam.dir - cam.fov / 2;
		float ray_dir_1 = cam.dir + cam.fov / 2;
		float ray_dir_x0 = cosf(ray_dir_0);
		float ray_dir_y0 = sinf(ray_dir_0);
		float ray_dir_x1 = cosf(ray_dir_1);
		float ray_dir_y1 = sinf(ray_dir_1);

		int p = abs(y - half_height);
		float scaleconst = 0.75;
		float posZfloor = (half_width / (2.0*sin(cam.fov / 2))) + (cam.z * WINDOW_HEIGHT * scaleconst);
		float posZceil = (half_width / (2.0*sin(cam.fov / 2))) - (cam.z * WINDOW_HEIGHT * scaleconst);

		float rowDistFloor = posZfloor/p;
		float rowDistCeil = posZceil/p;
		// float rowDist = (half_width / (2.0*sin(cam.fov / 2))) / p; // https://stackoverflow.com/questions/73453144/why-is-the-floor-in-my-raycaster-seemingly-misaligned

		float floorStepX = rowDistFloor * (ray_dir_x1 - ray_dir_x0) / (float) WINDOW_WIDTH;
		float floorStepY = rowDistFloor * (ray_dir_y1 - ray_dir_y0) / (float) WINDOW_HEIGHT;

		float floorX = cam.pos.x + rowDistFloor * ray_dir_x0;
		float floorY = cam.pos.y + rowDistFloor * ray_dir_y0;

		float ceilStepX = rowDistCeil * (ray_dir_x1 - ray_dir_x0) / (float) WINDOW_WIDTH;
		float ceilStepY = rowDistCeil * (ray_dir_y1 - ray_dir_y0) / (float) WINDOW_HEIGHT;

		float ceilX = cam.pos.x + rowDistCeil * ray_dir_x0;
		float ceilY = cam.pos.y + rowDistCeil * ray_dir_y0;

		for (int x = 0; x < half_width * 2; x++) {
			float xTextureOffsetFloor = fabsf(floorX - (int) floorX); // fabsf'd once but dont matter ?
			float yTextureOffsetFloor = fabsf(floorY - (int) floorY);

			float xTextureOffsetCeil = fabsf(ceilX - (int) ceilX); // fabsf'd once but dont matter ?
			float yTextureOffsetCeil = fabsf(ceilY - (int) ceilY);

			int floor_texture_id = floor_at((int) floorX, (int) floorY);
			if (floor_texture_id < 0)
				floor_texture_id = 0; // continue is buggy for some reason i dont know
			int ceiling_texture_id = ceiling_at((int) ceilX, (int) ceilY);
			if (ceiling_texture_id < 0)
				ceiling_texture_id = 0; // continue is buggy for some reason i dont know
			struct floor_texture_data floor_thing = floor_data[floor_texture_id];
			struct floor_texture_data ceiling_thing = floor_data[ceiling_texture_id];

			int floor_textureXOff = (int) (xTextureOffsetFloor * floor_thing.w);
			int floor_textureYOff = (int) (yTextureOffsetFloor * floor_thing.h);
			int ceiling_textureXOff = (int) (xTextureOffsetCeil * ceiling_thing.w);
			int ceiling_textureYOff = (int) (yTextureOffsetCeil * ceiling_thing.h);

			Uint32 floor_pixel = floor_thing.pixels[floor_textureYOff * floor_thing.w + floor_textureXOff];
			Uint32 ceiling_pixel = ceiling_thing.pixels[ceiling_textureYOff * ceiling_thing.w + ceiling_textureXOff];

			// float dist = (floorX - cam.pos.x) * (floorX - cam.pos.x) + (floorY - cam.pos.y) * (floorY - cam.pos.y);
			// int darkness = darkness_threshold > 0 ? (dist * 255 / darkness_threshold) : 0;
			// Uint8 dark = 255 - min(darkness, 255); 
			//
			// Uint8 r, g, b;
			// r = (floor_pixel & 0xFF000000) >> (8*3);
			// g = (floor_pixel & 0x00FF0000) >> (8*2);
			// b = (floor_pixel & 0x0000FF00) >> (8*1);
			//
			// r = min((Uint8) (r * (dark/255.0)), 255);
			// g = min((Uint8) (g * (dark/255.0)), 255);
			// b = min((Uint8) (b * (dark/255.0)), 255); 
			//
			// floor_pixel = (r << (8*3)) + (g << (8*2)) + (b << (8*1)) + 0x000000FF;
			//
			// r = (ceiling_pixel & 0xFF000000) >> (8*3);
			// g = (ceiling_pixel & 0x00FF0000) >> (8*2);
			// b = (ceiling_pixel & 0x0000FF00) >> (8*1);
			//
			// r = min((Uint8) (r * (dark/255.0)), 255);
			// g = min((Uint8) (g * (dark/255.0)), 255);
			// b = min((Uint8) (b * (dark/255.0)), 255); 
			//
			// ceiling_pixel = (r << (8*3)) + (g << (8*2)) + (b << (8*1)) + 0x000000FF;

			pixels[y * WINDOW_WIDTH + x] = ceiling_pixel;
			pixels[(WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x] = floor_pixel;
			floorX += floorStepX / 1;
			floorY += floorStepY / 1.5;

			ceilX += ceilStepX / 1;
			ceilY += ceilStepY / 1.5;
		}
	}

	SDL_UnlockTexture(floor_to_draw);
	SDL_RenderCopy(g_renderer, floor_to_draw, 0, 0);

	z_buffer_length = 0;

	for (int i = -half_width; i < half_width; i++) {
		float angle = atan2(i, dv) + cam.dir;

		float rayvx = cos(angle);
		float rayvy = sin(angle);

		float dist = 0;

		float rx = cam.pos.x;
		float ry = cam.pos.y;

		int celx = (int) rx;
		int cely = (int) ry;

		int dirx = sign(rayvx);
		int diry = sign(rayvy);

		float dist4x = fabsf(1 / rayvx);
		float dist4y = fabsf(1 / rayvy);

		float dx = fabs(0.5 + 0.5 * dirx - fixmod(cam.pos.x, 1));
		float dy = fabs(0.5 + 0.5 * diry - fixmod(cam.pos.y, 1));

		float dist2x = fabsf(dx / rayvx);
		float dist2y = fabsf(dy / rayvy);

		int side = 0;

		do {
			if (dist2x < dist2y) {
				celx += dirx;
				dist += dist2x;
				dist2y -= dist2x;
				dist2x = dist4x;
				side = 1;
			} else {
				cely += diry;
				dist += dist2y;
				dist2x -= dist2y;
				dist2y = dist4y;
				side = 0;
			}

			if (wall_at(celx, cely) >= 0) {
				rx = dist * cos(angle) + cam.pos.x;
				ry = dist * sin(angle) + cam.pos.y;

				float texture_offset = side ? (dirx > 0 ? fixmod(ry, 1) : fixmod(1-ry, 1)): (diry < 0 ? fixmod(rx, 1) : fixmod(1-rx, 1));

				insertRenderElement(i + half_width, dist, angle, texture_offset, wall_at(celx, cely));
				break;
			}
		} while (dist < 128);
	}

	for (int i = 0; i < objects->length; i++) {
		entity* thing = get_entity(objects, i);

		float my_angle = atan2(thing->pos.y - cam.pos.y, thing->pos.x - cam.pos.x);
		float upper = cam.dir + M_PI/2;
		float lower = cam.dir - M_PI/2;

		if (fixmod(my_angle - lower, 2*M_PI) <= fixmod(upper - lower, 2*M_PI)) {
			vec2 offset = {thing->pos.x - cam.pos.x, thing->pos.y - cam.pos.y};
			float dist = sqrtf(offset.x * offset.x + offset.y * offset.y);
			insertRenderElement(-1, dist, my_angle - cam.dir, thing->z, thing->type);
		}
	}

	for (int i = 0; i < z_buffer_length; i++) {
		render_element cur = z_buffer[i];

		if (cur.type >= 0) { // wall
			float scaleconst = 1;
			float rx = cur.dist * cos(cur.angle) + cam.pos.x;
			float ry = cur.dist * sin(cur.angle) + cam.pos.y;
			float corrected_dist = cur.dist * cos(cur.angle - cam.dir);

			SDL_Texture* texture = get_wall_texture(cur.id);

			int texture_width;
			int texture_height;

			SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

			int darkness = darkness_threshold > 0 ? (cur.dist * cur.dist * 255 / darkness_threshold) : 0;
			Uint8 dark = 255 - min(darkness, 255);

			SDL_SetTextureColorMod(texture, dark, dark, dark);

			float z = cam.z;

			float udv = dv - WINDOW_HEIGHT * sinf(z);
			float bdv = dv + WINDOW_HEIGHT * sinf(z);

			float udist = (udv / 2.0) / corrected_dist;
			float bdist = (bdv / 2.0) / corrected_dist;

			float screen_y = WINDOW_HEIGHT/2.0 - udist;
			float screen_h = udist + bdist;

			SDL_Rect rect = {cur.texture_offset * texture_width, 0, 1, texture_height};
			// SDL_Rect dest = {cur.type, WINDOW_HEIGHT/2.0 - (dv/2) * scaleconst / corrected_dist, 1, dv * scaleconst / corrected_dist};
			SDL_Rect dest = {cur.type, screen_y, 1, screen_h};
			SDL_RenderCopy(g_renderer, texture, &rect, &dest);
		} else { // entity
			float dx = cur.dist * sin(cur.angle); // before correcting for fisheye
			float correctedDist = cur.dist * cos(cur.angle); // normalize for fisheye

			int darkness = darkness_threshold > 0 ? (cur.dist * cur.dist * 255 / darkness_threshold) : 0;
			Uint8 dark = 255 - min(darkness, 255);

			float z = cam.z - cur.texture_offset; // entity z

			float udv = dv - WINDOW_HEIGHT * sinf(z);
			float bdv = dv + WINDOW_HEIGHT * sinf(z);

			float udist = (udv / 2.0) / correctedDist;
			float bdist = (bdv / 2.0) / correctedDist;

			float screen_y = WINDOW_HEIGHT/2.0 - udist;
			float screen_h = udist + bdist;

			SDL_SetTextureColorMod(get_entity_texture(cur.id), dark, dark, dark);
			draw_scaled_sprite(get_entity_texture(cur.id), dx * dv / correctedDist + WINDOW_WIDTH/2.0, screen_y + screen_h / 2, dv / correctedDist);
		}
	}
}
