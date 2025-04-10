#include "machine.h"
#include "tape.h"
#include "table.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



struct table table = {
    .rows = (struct row[]){
        {q0, (struct cell[]){ // Detect first symbol
            {Σ0, {.Q = q1, .Σ = B, .A = R}},
            {Σ1, {.Q = q2, .Σ = B, .A = R}},
            {B, {.H = accept, .Σ = B, .A = D}},
        }, .size = 3},
        {q1, (struct cell[]){ // Transport 0 right
            {Σ0, {.Q = q1, .Σ = Σ0, .A = R}},
            {Σ1, {.Q = q1, .Σ = Σ1, .A = R}},
            {B, {.Q = q3, .Σ = B, .A = L}},
        }, .size = 3},
        {q2, (struct cell[]){  // Transport 1
            {Σ0, {.Q = q2, .Σ = Σ0, .A = R}},
            {Σ1, {.Q = q2, .Σ = Σ1, .A = R}},
            {B, {.Q = q4, .Σ = B, .A = L}},
        }, .size = 3},
        {q3, (struct cell[]){ // check for 0
            {Σ0, {.Q = q5, .Σ = B, .A = L}},
            {Σ1, {.H = reject, .Σ = Σ1, .A = D}},
            {B, {.H = accept, .Σ = B, .A = D}},
        }, .size = 3},
        {q4, (struct cell[]){ // check for 1
            {Σ0, {.H = reject, .Σ = Σ0, .A = D}},
            {Σ1, {.Q = q5, .Σ = B, .A = L}},
            {B, {.H = accept, .Σ = B, .A = D}},
        }, .size = 3},
        {q5, (struct cell[]){ // Left transport
            {Σ0, {.Q = q5, .Σ = Σ0, .A = L}},
            {Σ1, {.Q = q5, .Σ = Σ1, .A = L}},
            {B, {.Q = q0, .Σ = B, .A = R}},
        }, .size = 3},
        
    },
    .size = 6,
};


struct T δ(enum Q q, enum Σ s) {  // Changed return type from void
    // Call the match_table function to get the transition
    struct T transition = match_table(q, s, &table);
    usleep(100);

    return transition;
}


enum halt run (struct M *m, struct tape *t) {
    enum Q state = m->q0;
    enum Σ current_symbol = tape_get(t, t->head);
    int step = 0;

    printf("Initial State: %d, Initial Symbol: %d, Head: %d\n", state, current_symbol, t->head);

    while (1) {

        tape_print(t);

        printf("Step %d: State: %d, Symbol: %d, Head: %d\n", step, state, current_symbol, t->head);
        struct T tr = m->δ(state, current_symbol);
        
        printf("New State: %d, New Symbol: %d, Action: %d\n", tr.Q, tr.Σ, tr.A);

        if (tr.A == D) {
            printf("Halting - State: %d, Symbol: %d\n", tr.H, tr.Σ);
            return tr.H;
        }

        // overwrite symbol
        tape_set(t, t->head, tr.Σ);
        printf("Tape updated at head %d: %d\n", t->head, tape_get(t, t->head));

        // Perform action based on tr.A
        // D is already handled above
        switch (tr.A) {
            case R:
                t->head++;
                printf("Action: Move Right -> Head now %d\n", t->head);
                break;
            case L:
                t->head--;
                printf("Action: Move Left -> Head now %d\n", t->head);
                break;
        }

        // Update current state and symbol
        state = tr.Q;
        printf("Updated State: %d, Updated Symbol: %d\n", state, current_symbol);
        current_symbol = tape_get(t, t->head);
        
        step++;
    }
}

#include <string.h>

int main() {
    struct M m;
    m.q0 = q0;

    m.δ = δ;

    store_table(&table, "table.txt");


    char* tape_str = "0110110"; // Example input
    int tape_size = strlen(tape_str); // Exclude null terminator
    struct tape *t = tape_create(tape_size);
    for (int i = 0; i < tape_size; i++) {
        if (tape_str[i] == '0') {
            t->tape[i] = Σ0;
        } else if (tape_str[i] == '1') {
            t->tape[i] = Σ1;
        } else {
            fprintf(stderr, "Invalid character in tape string: %c\n", tape_str[i]);
            free(t);
            return 1;
        }
    }

    tape_print(t);
    


    enum halt status = run(&m, t);
    printf("Final head position: %d\n", t->head);
    switch (status) {
        case accept:
            printf("Accepted\n");
            break;
        case reject:
            printf("Rejected\n");
            break;
        default:
            printf("Unknown status\n");
            break;
    }

    // print the tape
    printf("Final tape: ");
    for (int i = 0; i < t->size; i++) {
        if (t->tape[i] == B) {
            printf("B ");
        } else if (t->tape[i] == $) {
            printf("$ ");
        } else {
            printf("%d ", t->tape[i]);
        }
    }

    return 0;
}