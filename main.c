#include <stdio.h>
#include "libgen.h"

void fib(gen_t gen, value start) {
    value an_1 = 1, an_2 = 0;
    while (an_2 >= 0) {
        resume_by(gen, an_2);
        value tmp = an_1;
        an_1 = an_1 + an_2;
        an_2 = tmp;
    }
}

void fact(gen_t gen, value start) {
    value an = 1;
    for (int i = 1; an > 0; ++i) {
        resume_by(gen, an);
        an *= i;
    }
}


void foo(gen_t parent_gen, value start) {
    printf("start with %lld\n", start);
    value x;
    resume(parent_gen, 99, &x);
    printf("resume from main and get %lld\n", x);
    resume(parent_gen, 98, &x);
    printf("resume from main and get %lld\n", x);
    resume(parent_gen, 97, &x);
    printf("resume from main and get %lld\n", x);
    drop_gen(parent_gen);
}


int main() {

    gen_t fib_gen = generator(fib);
    gen_t fact_gen = generator(fact);
    value an, bn;
    for (int i = 0;
        resume_on(fib_gen, &an) &&
        resume_on(fact_gen, &bn); ++i) {
        printf("fib(%d) %lld\n", i, an);
        printf("fact(%d) %lld\n", i, bn);
    }

    drop_gen(fact_gen);
    drop_gen(fib_gen);

    value x = 100;
    gen_t foo_gen = generator(foo);
    for (int i = 0; resume(foo_gen, i, &x); ++i) {
        printf("resume from foo and get %lld\n", x);
    }

    return 0;
}
