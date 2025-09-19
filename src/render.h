#pragma once

#include "entity.h"
#include "camera.h"

void init_floor_texture_data();
void free_floor_texture_data();
void render(raycast_camera cam, entity_array* objects);
