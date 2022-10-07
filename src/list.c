#include "include/list.h"
#include "include/error.h"

#include <stdlib.h>

void ListPushBack(list_t* self, void* item) {
    self->size++;
    
    if (!self->items) {
        self->items = malloc(self->itemSize);
        if (!self->items)
            error_malloc(__func__);
    }
    else {
        self->items = realloc(self->items, self->size * self->itemSize);
        if (!self->items)
            error_realloc(__func__);
    }

    self->items[self->size - 1] = item;
}

void List(list_t* self, size_t itemSize) {
    self->items = NULL;
    self->size = 0;
    self->itemSize = itemSize;

    self->PushBack = ListPushBack;
}

list_t* newList(size_t itemSize) {
    list_t* self = (list_t*) malloc(sizeof(list_t));
    if (!self)
        error_calloc(__func__);

    List(self, itemSize);
    return self;
}
