//
// Created by toe on 2019/10/29.
//

#include <stdlib.h>
#include "libgen.h"

value yield(gen_t* gen, value v) {
    gen->tag = 1;
    gen->out = v;
    swapcontext(&gen->out_ctx, &gen->in_ctx);
    return gen->in;
}

int resume(gen_t* gen, value* out, value in) {
    gen->tag = 0;
    gen->in = in;
    swapcontext(&gen->in_ctx, &gen->out_ctx);
    *out = gen->out;
    return gen->tag;
}

gen_t generator(void f(gen_t*)) {
    void* frame = malloc(DEFAULT_STACK_SIZE);
    gen_t gen;
    gen.tag = 0;
    getcontext(&gen.out_ctx);
    gen.out_ctx.uc_stack.ss_sp = frame;
    gen.out_ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    gen.out_ctx.uc_stack.ss_flags = 0;
    gen.out_ctx.uc_link = &gen.in_ctx;
    makecontext(&gen.out_ctx, (void (*)(void)) f, 1, &gen);
    return gen;
}

void drop_gen(gen_t* gen) {
    free(gen->out_ctx.uc_stack.ss_sp);
}