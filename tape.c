#include "tape.h"
#include <stdlib.h>

struct tape* tape_create(int size) {
    struct tape *t = malloc(sizeof(struct tape));
    t->tape = malloc(size * sizeof(enum Σ));
    t->head = 0;

    // set everything to blank
    for (int i = 0; i < size; i++) {
        t->tape[i] = B;
    }

    // set the first symbol to $
    t->tape[0] = $;
    t->size = size;

    return t;
}

struct tape* tape_realloc(struct tape *t, int new_size) {
    // check if new size is greater than current size
    if (new_size > t->size) {
        t->tape = realloc(t->tape, new_size * sizeof(enum Σ));
        for (int i = t->size; i < new_size; i++) {
            t->tape[i] = B;
        }
        t->size = new_size;
    }

    return t;
}

enum Σ tape_get(struct tape *t, int index) {

    if (index < 0 || index >= t->size) {
        // reallocate tape if index is out of bounds
        t = tape_realloc(t, index + 1);
    }

    return t->tape[index];
}

void tape_set(struct tape *t, int index, enum Σ value) {

    // check if index is within bounds
    if (index < 0 || index >= t->size) {
        // reallocate tape if index is out of bounds
        t = tape_realloc(t, index + 1);
    }
    t->tape[index] = value;
}

struct tape* tape_init(enum Σ *data, int size) {
    struct tape *t = tape_create(size);


    // copy data to tape
    for (int i = 0; i < size; i++) {
        t->tape[i] = data[i];
    }
    return t;
}

#include <stdio.h>
void tape_print(struct tape *t) {
    for (int i = 0; i < t->size; i++) {
        if (t->tape[i] == B) {
            printf("B ");
        } else if (t->tape[i] == $) {
            printf("$ ");
        } else {
            printf("%d ", t->tape[i]);
        }
    }
    printf("\n");
}


void destroy_tape(struct tape *t) {
    free(t->tape);
    free(t);
}

