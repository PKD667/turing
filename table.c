#include "machine.h"
#include "table.h"

#include <ctype.h>
#include <stdio.h>

struct T match_table(enum Q q, enum Σ s, struct table *table) {


    for (int i = 0; i < table->size; i++) {
        if (table->rows[i].q == q) {
            // match the right cell
            for (int j = 0; j < table->rows[i].size; j++) {
                if (table->rows[i].cells[j].s == s) {
                    return table->rows[i].cells[j].t;
                }
            }
        }
    }
    // If no match found, return a default transition
    printf("Error: No match found for state %d and symbol %d\n", q, s);
    return (struct T){.H = reject, .Σ = s, .A = D}; // Default to halt
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to find a row by state or create a new one
static int find_or_create_row(struct table *table, enum Q q) {
    for (int i = 0; i < table->size; i++) {
        if (table->rows[i].q == q) {
            return i;
        }
    }
    
    // Create new row
    table->size++;
    table->rows = realloc(table->rows, table->size * sizeof(struct row));
    if (!table->rows) {
        perror("Failed to allocate memory for table row");
        exit(1);
    }
    
    int idx = table->size - 1;
    table->rows[idx].q = q;
    table->rows[idx].cells = NULL;
    table->rows[idx].size = 0;
    
    return idx;
}

// Helper function to add a cell to a row
static void add_cell_to_row(struct row *row, enum Σ symbol, struct T transition) {
    row->size++;
    row->cells = realloc(row->cells, row->size * sizeof(struct cell));
    if (!row->cells) {
        perror("Failed to allocate memory for row cells");
        exit(1);
    }
    
    int idx = row->size - 1;
    row->cells[idx].s = symbol;
    row->cells[idx].t = transition;
}

// Parse direction character to enum
static enum A parse_direction(char dir) {
    switch (dir) {
        case 'L': return L;
        case 'R': return R;
        case 'D': return D;
        default:
            fprintf(stderr, "Invalid direction: %c\n", dir);
            return D; // Default to D
    }
}

// Parse halt state or regular state
static enum halt parse_halt_state(const char *state_str) {
    if (strcmp(state_str, "accept") == 0)
        return accept;
    if (strcmp(state_str, "reject") == 0)
        return reject;

    return -1; // not_halt
}

struct table *load_table(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open table file");
        return NULL;
    }
    
    // Initialize empty table
    struct table *table = malloc(sizeof(struct table));
    if (!table) {
        perror("Failed to allocate memory for table");
        fclose(file);
        return NULL;
    }
    table->rows = NULL;
    table->size = 0;
    
    char line[256];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;
        
        // Parse transition rule
        int current_state, current_symbol, new_symbol;
        char next_state_str[20], direction_char;
        
        int matched = sscanf(line, "%d %d %s %d %c", 
                            &current_state, &current_symbol, 
                            next_state_str, &new_symbol, &direction_char);
        
        if (matched != 5) {
            fprintf(stderr, "Error parsing line %d: %s\n", line_num, line);
            continue;
        }
        
        // Create transition
        struct T transition;

        bool not_halt = true;
        
        // Parse next_state (could be a number or "accept"/"reject")
        if (isdigit(next_state_str[0])) {
            transition.Q = atoi(next_state_str);
        } else {
            not_halt = false;
            transition.H = parse_halt_state(next_state_str);
            transition.Q = 0;  // This doesn't matter when H != not_halt
        }
        
        transition.Σ = (enum Σ)new_symbol;
        if (not_halt) transition.A = parse_direction(direction_char);
        
        // Add to table
        int row_idx = find_or_create_row(table, (enum Q)current_state);
        add_cell_to_row(&table->rows[row_idx], (enum Σ)current_symbol, transition);
    }
    
    fclose(file);
    return table;
}

int store_table(struct table *table, const char *filename) {
    if (!table) {
        fprintf(stderr, "Table is NULL\n");
        return -1;
    }
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return -1;
    }
    
    fprintf(file, "# Turing machine transition table\n");
    fprintf(file, "# current_state current_symbol next_state new_symbol direction\n\n");
    
    for (int i = 0; i < table->size; i++) {
        enum Q state = table->rows[i].q;
        
        for (int j = 0; j < table->rows[i].size; j++) {
            struct cell *cell = &table->rows[i].cells[j];
            
            // Convert direction enum to character
            char dir_char = 'D';
            switch (cell->t.A) {
                case L: dir_char = 'L'; break;
                case R: dir_char = 'R'; break;
                case D: dir_char = 'D'; break;
                default: dir_char = 'D'; break;
            }
            
            // Output based on whether it's a halt state
            if (cell->t.A != D) {
                fprintf(file, "%d %d %d %d %c\n", 
                       (int)state, (int)cell->s, 
                       (int)cell->t.Q, (int)cell->t.Σ, dir_char);
            } else {
                fprintf(file, "%d %d %s %d %c\n", 
                       (int)state, (int)cell->s, 
                       cell->t.H == accept ? "accept" : "reject", 
                       (int)cell->t.Σ, dir_char);
            }
        }
    }
    
    fclose(file);
    return 0;
}