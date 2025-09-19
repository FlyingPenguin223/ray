#pragma once

#include "vec.h"

typedef struct raycast_camera {
    vec2 pos;
	float z;
    float dir;
    float fov;
} raycast_camera;
