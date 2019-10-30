//
// Created by d5846 on 2019/10/29.
//

#ifndef GENERATOR_LIBGEN_H
#define GENERATOR_LIBGEN_H

#include <ucontext.h>

#define DEFAULT_STACK_SIZE (1024*128)

typedef long long value;
typedef struct {
    int         tag;        // done or not
    ucontext_t  in_ctx;     // parent ctx
    ucontext_t  out_ctx;    // sub-routine ctx
    value       out;
    value       in;
} gen_t;

value yield(gen_t* gen, value v);

int resume(gen_t* gen, value* out, value in);

gen_t generator(void f(gen_t*));

void drop_gen(gen_t* gen);

#endif //GENERATOR_LIBGEN_H
