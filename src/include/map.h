#pragma once

#include <stddef.h>

typedef struct mapNode_t mapNode_t;
struct mapNode_t {
    char* key;
    void* value;

    mapNode_t* left;
    mapNode_t* right;
};

typedef struct map_t map_t;
struct map_t {
    mapNode_t* root;
    
    mapNode_t* (*Set) (map_t* self, char* key, void* value);
    void* (*Get) (map_t* self, char* key);

    void (*Print) (map_t* self);
};

void Map(map_t* self);
map_t* newMap(void);