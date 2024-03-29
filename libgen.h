//
// Created by d5846 on 2019/10/29.
//

#ifndef GENERATOR_LIBGEN_H
#define GENERATOR_LIBGEN_H

#define DEFAULT_STACK_SIZE (1024*1024)

typedef long long value;
typedef struct _gen_t * gen_t;

gen_t generator(void f(gen_t, value));
void drop_gen(gen_t gen);

int resume(gen_t gen, value send, value* recv);
int resume_by(gen_t gen, value send);
int resume_on(gen_t gen, value* recv);
int resume_(gen_t gen);

#endif //GENERATOR_LIBGEN_H
