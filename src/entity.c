#include <stdlib.h>

#include "entity.h"

entity_array* init_entity_array() {
    entity_array* ret = malloc(sizeof(entity_array));
    ret->length = 0;
    ret->capacity = 1;
    ret->array = malloc(ret->capacity * sizeof(entity*));

    return ret;
}

entity* init_entity(entity_array* array, int type, float x, float y) {
    entity* new_entity = malloc(sizeof(entity));

    new_entity->pos = (vec2) {x, y};
	new_entity->z = 0;
    new_entity->spd = (vec2) {0, 0};
    new_entity->type = type;
    new_entity->update = NULL;


    if (array->length >= array->capacity) {
        array->capacity *= 2;
        array->array = realloc(array->array, array->capacity * sizeof(entity*));
    }

    array->array[array->length] = new_entity;

    array->length++;

    return new_entity;
}

void kill_entity(entity_array* array, int id) {
    if (id < 0 || id >= array->length)
        return;

    free(array->array[id]);

    for (int i = id+1; i < array->length; i++) {
        array->array[i - 1] = array->array[i];
    }

    array->length--;
}

entity* get_entity(entity_array* array, int id) {
    if (id < 0 || id >= array->length)
        return NULL;

    return array->array[id];
}

void free_entity_array(entity_array* array) {
    for (int i = array->length-1; i >= 0; i--) {
        kill_entity(array, i);
    }
    free(array->array);
    free(array);
}
