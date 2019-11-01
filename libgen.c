//
// Created by toe on 2019/10/29.
//

#include <stdlib.h>
#include <ucontext.h>
#include "libgen.h"

struct _gen_t {
    int          tag;
    ucontext_t*  ctx;
    void*        stack;
    value        send;
    gen_t        co;
};

gen_t generator(void f(gen_t)) {
    void launch(gen_t, void (gen_t));
    ucontext_t*  ctx = malloc(sizeof(ucontext_t));
    ucontext_t*  co_ctx = malloc(sizeof(ucontext_t));
    void* stack = malloc(DEFAULT_STACK_SIZE);

    gen_t gen = malloc(sizeof(struct _gen_t));
    gen_t co_gen = malloc(sizeof(struct _gen_t));
    gen->tag = 1;
    gen->ctx = ctx;
    gen->stack = NULL;
    gen->co = co_gen;

    co_gen->tag = 1;
    co_gen->ctx = co_ctx;
    co_gen->stack = stack;
    co_gen->co = gen;

    getcontext(co_ctx);
    co_ctx->uc_stack.ss_sp = stack;
    co_ctx->uc_stack.ss_size = DEFAULT_STACK_SIZE;
    co_ctx->uc_stack.ss_flags = 0;
    co_ctx->uc_link = ctx;
    ctx->uc_link = NULL;
    makecontext(co_ctx, (void (*)(void)) launch, 2, gen, f);
    return co_gen;
}

int resume(gen_t gen, value in, value* out) {
    if (!gen->tag) { return 0; }
    gen->send = in;
    swapcontext(gen->co->ctx, gen->ctx);
    *out = gen->co->send;
    return gen->tag;
}

int resume_by(gen_t gen, value in) {
    if (!gen->tag) { return 0; }
    gen->send = in;
    swapcontext(gen->co->ctx, gen->ctx);
    return gen->tag;
}

int resume_on(gen_t gen, value* out) {
    if (!gen->tag) { return 0; }
    swapcontext(gen->co->ctx, gen->ctx);
    *out = gen->co->send;
    return gen->tag;
}

int resume_(gen_t gen) {
    if (!gen->tag) { return 0; }
    swapcontext(gen->co->ctx, gen->ctx);
    return gen->tag;
}

void drop_gen(gen_t gen) {
    free(gen->ctx);
    free(gen->stack);
    gen->ctx = NULL;
    gen->stack = NULL;

    if (gen->co->ctx->uc_link) {
        gen->co->ctx->uc_link = NULL;
        return;
    }
    free(gen->co->ctx);
    free(gen->co);
    gen->co = NULL;

    free(gen);
}



void launch(gen_t gen, void f(gen_t)) {
    f(gen);
    // TODO any other error handler

    if (!gen->co->ctx->uc_link) {
        exit(0);
    }
    gen->co->tag = 0;
}