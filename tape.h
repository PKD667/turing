#pragma once

#include "machine.h"
struct tape {
    enum Σ *tape;
    int head;

    int size;
};

struct tape* tape_create(int size);
struct tape* tape_realloc(struct tape *t, int new_size);
enum Σ tape_get(struct tape *t, int index);
struct tape* tape_init( enum Σ *data, int size);
void tape_set(struct tape *t, int index, enum Σ value);
void tape_print(struct tape *t);
void destroy_tape(struct tape *t);
