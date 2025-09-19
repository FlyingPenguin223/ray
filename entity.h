#pragma once

#include "vec.h"

typedef struct entity {
    int type;
    vec2 pos;
	float z;
    vec2 spd;
    void (*update)(struct entity* this);
} entity;

typedef struct entity_array {
    int length;
    int capacity;
    entity** array;
} entity_array;

entity_array* init_entity_array();

entity* init_entity(entity_array* array, int type, float x, float y);

void kill_entity(entity_array* array, int id);

entity* get_entity(entity_array* array, int id);

void free_entity_array(entity_array* array);
