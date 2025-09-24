#pragma once

#include "entity.h"

void update_player(entity* this);

void(*updates[])(entity* this) = {
	update_player,
};
