//
// Created by toe on 2019/10/29.
//

#include <stdlib.h>
#include <ucontext.h>
#include "libgen.h"

struct _gen_t {
    int          tag;
    ucontext_t*  ctx;
    ucontext_t*  co_ctx;
    void*        stack;
    value*       in;
    value*       out;
    gen_t         co;
};

gen_t generator(void f(gen_t)) {
    void launch(gen_t, void (gen_t));
    ucontext_t*  ctx = malloc(sizeof(ucontext_t));
    ucontext_t*  co_ctx = malloc(sizeof(ucontext_t));
    void* stack = malloc(DEFAULT_STACK_SIZE);

    value* in_pool = malloc(sizeof(value));
    value* out_pool = malloc(sizeof(value));

    gen_t gen = malloc(sizeof(struct _gen_t));
    gen_t co_gen = malloc(sizeof(struct _gen_t));
    gen->tag = 0;
    gen->ctx = ctx;
    gen->co_ctx = co_ctx;
    gen->stack = stack;
    gen->in = in_pool;
    gen->out = out_pool;
    gen->co = co_gen;

    co_gen->tag = 0;
    co_gen->ctx = co_ctx;
    co_gen->co_ctx = ctx;
    co_gen->stack = stack;
    co_gen->in = out_pool;
    co_gen->out = in_pool;
    co_gen->co = gen;

    getcontext(co_ctx);
    co_ctx->uc_stack.ss_sp = stack;
    co_ctx->uc_stack.ss_size = DEFAULT_STACK_SIZE;
    co_ctx->uc_stack.ss_flags = 0;
    co_ctx->uc_link = ctx;
    ctx->uc_link = NULL;
    makecontext(co_ctx, (void (*)(void)) launch, 2, co_gen, f);
    return gen;
}

int resume(gen_t gen, value in, value* out) {
    if (gen->co->tag) { return 0; }
    gen->tag = 0;
    gen->co->tag = 1;
    *(gen->in) = in;
    swapcontext(gen->ctx, gen->co_ctx);
    *out = *gen->out;
    return gen->tag;
}

int resume_by(gen_t gen, value in) {
    if (gen->co->tag) { return 0; }
    gen->tag = 0;
    gen->co->tag = 1;
    *(gen->in) = in;
    swapcontext(gen->ctx, gen->co_ctx);
    return gen->tag;
}

int resume_on(gen_t gen, value* out) {
    if (gen->co->tag) { return 0; }
    gen->tag = 0;
    gen->co->tag = 1;
    swapcontext(gen->ctx, gen->co_ctx);
    *out = *gen->out;
    return gen->tag;
}

int resume_(gen_t gen) {
    if (gen->co->tag) { return 0; }
    gen->tag = 0;
    gen->co->tag = 1;
    swapcontext(gen->ctx, gen->co_ctx);
    return gen->tag;
}

void drop_gen(gen_t gen) {
    free(gen->in);
    free(gen->out);
    free(gen->co);
    free(gen->co_ctx);
    gen->out = NULL;
    gen->in = NULL;
    gen->co = NULL;
    gen->co_ctx = NULL;
    if (gen->ctx->uc_link) {
        gen->ctx->uc_link = NULL;
        return;
    }
    free(gen->ctx);
    free(gen->stack);
    gen->ctx = NULL;
    gen->stack = NULL;
    free(gen);
}



void launch(gen_t gen, void f(gen_t)) {
    f(gen);
    // TODO any other error handler
    if (!gen->ctx->uc_link) {
        exit(0);
    }
}