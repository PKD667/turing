#pragma once

#define bool char
#define true 1
#define false 0

// allow unicode
enum A{
    R,
    L,
    D
};

enum Q {
    q0,
    q1,
    q2,
    q3,
};

enum Σ {
    Σ0,
    Σ1,

    ΣX,
    ΣY,

    B, // blank
    $  // start
};

enum halt {
    accept,
    reject
};

struct T {
    union {
        enum Q Q;
        enum halt H;
    };

    enum Σ Σ;
    enum A A;
};




struct M {
    enum Q q0;


    struct T(*δ)(enum Q q, enum Σ s);
};