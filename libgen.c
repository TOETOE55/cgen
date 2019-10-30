//
// Created by toe on 2019/10/29.
//

#include <stdlib.h>
#include <stdio.h>
#include "libgen.h"

struct _gen_t {
    int         tag;        // done or not
    ucontext_t  in_ctx;     // parent ctx
    ucontext_t  out_ctx;    // sub-routine ctx
    value       out;
    value       in;
    char        frame[];
};

value yield(gen_t gen, value v) {
    gen->tag = 1;
    gen->out = v;
    swapcontext(&gen->out_ctx, &gen->in_ctx);
    return gen->in;
}

int resume(gen_t gen, value* out, value in) {
    gen->tag = 0;
    gen->in = in;
    swapcontext(&gen->in_ctx, &gen->out_ctx);
    *out = gen->out;
    return gen->tag;
}

gen_t generator(void f(gen_t)) {
    gen_t gen = malloc(sizeof(struct _gen_t) + DEFAULT_STACK_SIZE);
    getcontext(&gen->out_ctx);
    gen->tag = 0;
    gen->out_ctx.uc_stack.ss_sp = gen->frame;
    gen->out_ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    gen->out_ctx.uc_stack.ss_flags = 0;
    gen->out_ctx.uc_link = &gen->in_ctx;
    makecontext(&gen->out_ctx, (void (*)(void)) f, 1, gen);
    return gen;
}
