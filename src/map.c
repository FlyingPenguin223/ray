#include "../raycast.h"

extern const Tiled2cMap testmap;

int wall_at(int x, int y) {
    int i = x + y * testmap.width;
    if (i < 0 || i >= testmap.width * testmap.height)
        return -1;
    return testmap.layers[2].tilelayer.data[i] - 1;
}

int floor_at(int x, int y) {
    int i = x + y * testmap.width;
    if (i < 0 || i >= testmap.width * testmap.height)
        return -1;
    return testmap.layers[0].tilelayer.data[i] - 1;
}

int ceiling_at(int x, int y) {
    int i = x + y * testmap.width;
    if (i < 0 || i >= testmap.width * testmap.height)
        return -1;
    return testmap.layers[1].tilelayer.data[i] - 1;
}

/*
void load_map(const char* path) {
    if (map != NULL)
        free(map);

    FILE* f = fopen(path, "r");
    char str[128];
    int result;
    int map_index = 0;

    fscanf(f, "%d %d", &map_width, &map_height);

    map = malloc((map_width * map_height) * sizeof(int));

    do {
        result = fscanf(f, "%127[^,\n]", str);

        if (result == 0) {
            result = fscanf(f, "%*c");
        } else {
            map[map_index] = atoi(str);
            map_index++;
        }
    } while (result != EOF && map_index <= map_width * map_height);
} */
