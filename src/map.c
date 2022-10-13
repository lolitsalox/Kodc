#include "include/map.h"
#include "include/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void MapNode(mapNode_t* self, char* key, void* value, mapNode_t* left, mapNode_t* right) {
    self->key = key;
    self->value = value;
    self->left = left;
    self->right = right;
}

static mapNode_t* newMapNode(char* key, void* value, mapNode_t* left, mapNode_t* right) {
    mapNode_t* self = (mapNode_t*) malloc(sizeof(mapNode_t));
    if (!self) error_malloc(__func__);

    MapNode(self, key, value, left, right);
    return self;
}

static mapNode_t* SetMapNode(mapNode_t** node_field, char* key, void* value) {
    if (!node_field) {
        // literally skill issue
        return NULL;
    }
    mapNode_t* node = *node_field;
    if (!node) {
        node = newMapNode(key, value, NULL, NULL);
        *node_field = node;
        return node;
    }
    int res = strcmp(key, node->key);
    if (res < 0) {
        return SetMapNode(&node->left, key, value);
    }
    if (res > 0) {
        return SetMapNode(&node->right, key, value);
    }
    free(node->value);
    node->value = value;
    return node;
}

static mapNode_t* Set(map_t* self, char* key, void* value) {
    return SetMapNode(&self->root, key, value);
}

static void* GetMapNode(mapNode_t* node, char* key) {
    if (!node) {
        return NULL;
    }
    int res = strcmp(key, node->key);
    if (res < 0) {
        if (node->left) {
            return GetMapNode(node->left, key);
        }
        return NULL;
    }
    if (res > 0) {
        if (node->right) {
            return GetMapNode(node->right, key);
        }
        return NULL;
    }
    return node->value;
}

static void* Get(map_t* self, char* key) {
    return GetMapNode(self->root, key);
}

static void PrintNode(mapNode_t* node, size_t indent) {
    if (!node) return;
    INDENT(indent);
    printf("%s\n", node->key);

    if (node->right) {
        INDENT(indent + 1);
        printf("right:\n");
        PrintNode(node->right, indent + 2);
    }

    if (node->left) {
        INDENT(indent + 1);
        printf("left:\n");
        PrintNode(node->left, indent + 2);
    }
}

static void Print(map_t* self) {
    PrintNode(self->root, 0);
}

void Map(map_t* self) {
    self->root = NULL;

    self->Set = Set;
    self->Get = Get;
    self->Print = Print;
}

map_t* newMap(void) {
    map_t* self = (map_t*) malloc(sizeof(map_t));
    if (!self) error_malloc(__func__);

    Map(self);
    return self;
}
