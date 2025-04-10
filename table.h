#pragma once

#include "machine.h"

struct cell {
    enum Σ s;

    // Q X Σ -> (Q U halt) X Σ 
    struct T t;
};

struct row {
    enum Q q;
    struct cell *cells;
    int size;
};


struct table {
    struct row *rows;
    int size;
};

struct T match_table(enum Q q, enum Σ s, struct table *table);

// Load a transition table from a file
struct table *load_table(const char *filename);

// Store a transition table to a file
int store_table(struct table *table, const char *filename);