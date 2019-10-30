//
// Created by d5846 on 2019/10/29.
//

#ifndef GENERATOR_LIBGEN_H
#define GENERATOR_LIBGEN_H

#include <ucontext.h>

#define DEFAULT_STACK_SIZE (1024*1024*2)

typedef long long value;
struct _gen_t;
typedef struct _gen_t * gen_t;

value yield(gen_t gen, value v);

int resume(gen_t gen, value* out, value in);

gen_t generator(void f(gen_t));

#endif //GENERATOR_LIBGEN_H
