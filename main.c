#include <stdio.h>
#include "libgen.h"

void fib(gen_t* gen) {
    value an_1 = 1, an_2 = 0;
    while (an_2 >= 0) {
        yield(gen, an_2);
        value tmp = an_1;
        an_1 = an_1 + an_2;
        an_2 = tmp;
    }
}

void fact(gen_t* gen) {
    value an = 1;
    for (int i = 1; an >= 0; ++i) {
        yield(gen, an);
        an *= i;
    }
}

int main() {
    gen_t fib_gen = generator(fib);
    gen_t fact_gen = generator(fact);
    value an, bn;
    for (int i = 0; resume(&fib_gen, &an, i) && resume(&fact_gen, &bn, i); ++i) {
        printf("fib(%d) %lld\n", i, an);
        printf("fact(%d) %lld\n", i, bn);
    }

    drop_gen(&fib_gen);
    drop_gen(&fact_gen);
    return 0;
}
