
#include "machine.h"
#include "tape.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct cell {
    enum Q q;
    enum Σ s;

    // Q X Σ -> (Q U halt) X Σ 
    struct T t;
};

struct cell table[] = {
    // q0
    {q0, $, {q0, $, R}},
    {q0, Σ0, {q0, Σ0, R}},
    {q0, Σ1, {q1, ΣY, R}}, 
    {q0, ΣX, {q0, ΣX, R}}, 
    {q0, ΣY, {q0, ΣY, R}}, 
    {q0, B, {q2, B, L}}, 


    // q1
    {q1, $, {q1, $, R}},
    {q1, Σ0, {q0, ΣX, R}},
    {q1, Σ1, {q1, Σ1, R}}, 
    {q1, ΣX, {q1, ΣX, R}}, 
    {q1, ΣY, {q1, ΣY, R}},
    {q1, B, {q3, B, L}},

    // q2
    {q2, $, {.H = accept, .Σ = $, .A = D}},
    {q2, Σ0, {q2, Σ0, L}},
    {q2, Σ1, {q1, ΣY, R}}, 
    {q2, ΣX, {q2, ΣX, L}}, 
    {q2, ΣY, {q2, ΣY, L}},
    {q2, B, {q2, B, L}},


    // q3
    {q3, $, {.H = reject, .Σ = $, .A = D}},
    {q3, Σ0, {q0, ΣX, R}},
    {q3, Σ1, {q3, Σ1, L}}, 
    {q3, ΣX, {q3, ΣX, L}}, 
    {q3, ΣY, {q3, ΣY, L}},
    {q3, B, {q3, B, L}},
};

struct T match_table(enum Q q, enum Σ s) {
    for (int i = 0; i < sizeof(table) / sizeof(struct cell); i++) {
        if (table[i].q == q && table[i].s == s) {
            return table[i].t;
        }
    }
    // If no match found, return a default transition
    return (struct T){.Q = q, .Σ = s, .A = D}; // Default to halt
}

struct T δ(enum Q q, enum Σ s) {  // Changed return type from void
    // Call the match_table function to get the transition
    struct T transition = match_table(q, s);
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


int main() {
    struct M m;
    m.q0 = q0;

    m.δ = δ;
    struct tape* t = tape_init((enum Σ[]){
        $,
        Σ0,
        Σ1,
        Σ1,
        Σ0,
        Σ0,
        Σ0,
    }, 7);
    


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