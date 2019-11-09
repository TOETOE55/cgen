//
// Created by toe on 2019/10/29.
//

#include <stdlib.h>
#include <ucontext.h>
#include "libgen.h"

struct _gen_t {
    int          state;
    void*        stack;
    value        send;
    gen_t        dual;
    ucontext_t   ctx;
};

gen_t generator(void f(gen_t, value)) {
    void bootstrap(gen_t, void (gen_t, value));
    void* stack = malloc(DEFAULT_STACK_SIZE);

    gen_t gen = malloc(sizeof(struct _gen_t));
    gen_t dual_gen = malloc(sizeof(struct _gen_t));

    gen->state = 1;
    gen->stack = stack;
    gen->dual = dual_gen;

    dual_gen->state = 1;
    dual_gen->stack = NULL;
    dual_gen->dual = gen;

    getcontext(&gen->ctx);
    gen->ctx.uc_stack.ss_sp = stack;
    gen->ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    gen->ctx.uc_stack.ss_flags = 0;
    gen->ctx.uc_link = &dual_gen->ctx;
    dual_gen->ctx.uc_link = NULL;
    makecontext(&gen->ctx, (void (*)(void)) bootstrap, 2, dual_gen, f);

    return gen;
}

int resume(gen_t gen, value send, value* recv) {
    if (!gen->state) { return 0; }
    gen->send = send;
    swapcontext(&gen->dual->ctx, &gen->ctx);
    *recv = gen->dual->send;
    return gen->state;
}

int resume_by(gen_t gen, value send) {
    if (!gen->state) { return 0; }
    gen->send = send;
    swapcontext(&gen->dual->ctx, &gen->ctx);
    return gen->state;
}

int resume_on(gen_t gen, value* recv) {
    if (!gen->state) { return 0; }
    swapcontext(&gen->dual->ctx, &gen->ctx);
    *recv = gen->dual->send;
    return gen->state;
}

int resume_(gen_t gen) {
    if (!gen->state) { return 0; }
    swapcontext(&gen->dual->ctx, &gen->ctx);
    return gen->state;
}

void drop_gen(gen_t gen) {
    if (gen->dual->ctx.uc_link) {
        gen->dual->ctx.uc_link = NULL;
        return;
    }

    free(gen->dual);
    gen->dual = NULL;

    free(gen->stack);
    gen->stack = NULL;

    free(gen);
}



void launch(gen_t gen, void f(gen_t, value)) {
    f(gen, gen->dual->send);

    if (!gen->dual->ctx.uc_link) {
        exit(0);
    }
    gen->dual->state = 0;
}