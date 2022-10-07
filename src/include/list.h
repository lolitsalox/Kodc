#pragma once

#include <stddef.h>

typedef struct list_t list_t;

struct list_t {
    void** items;
    size_t size;
    size_t itemSize;

    void (*PushBack) (list_t* self, void* item);
};

void List(list_t* self, size_t itemSize);
list_t* newList(size_t itemSize);