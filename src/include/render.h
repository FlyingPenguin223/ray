#pragma once

#include "../../raycast.h"

void init_floor_texture_data();
void free_floor_texture_data();
void render(raycast_camera cam, entity_array* objects);
