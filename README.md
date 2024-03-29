# cgen
a lightweight generator implemented by ucontext.h

## API

```c
// create the generator
gen_t generator(void f(gen_t));
// delete the generator, 
// you can drop the parent generator 
// but the the program will exit 
// when the generator return
void drop_gen(gen_t gen);

// resume the generator, with the input or the output paramenter
int resume(gen_t gen, value send, value* out);
int resume_by(gen_t gen, value send);
int resume_on(gen_t gen, value* out);
int resume_(gen_t gen);
```



## Example

create and drop a generator:

```c
void foo(gen_t gen) {
    printf("foo start\n");
}

int main() {
    gen_t foo_gen = generator(foo);
    drop_gen(foo);
    return 0;
}
```



yield and resume a generator:

```c
void foo(gen_t parent_gen) {
    value x;
    resume(parent_gen, 99, &x);
    printf("resume from main and get %lld\n", x);
    resume(parent_gen, 98, &x);
    printf("resume from main and get %lld\n", x);
    resume(parent_gen, 97, &x);
    printf("resume from main and get %lld\n", x);
}


int main() {
    value x = 100;
    gen_t foo_gen = generator(foo);
    for (int i = 0; resume(foo_gen, i, &x); ++i) {
        printf("resume from foo and get %lld\n", x);
    }
    drop_gen(foo_gen);
    return 0;
}

/* output
resume from foo and get 99
resume from main and get 1
resume from foo and get 98
resume from main and get 2
resume from foo and get 97
resume from main and get 3
*/
```



you can create multiple generators:

```C
void fib(gen_t gen) {
    value an_1 = 1, an_2 = 0;
    while (an_2 >= 0) {
        resume_by(gen, an_2);
        value tmp = an_1;
        an_1 = an_1 + an_2;
        an_2 = tmp;
    }
}

void fact(gen_t gen) {
    value an = 1;
    for (int i = 1; an > 0; ++i) {
        resume_by(gen, an);
        an *= i;
    }
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
    return 0;
}

/* output
fib(0) 0
fact(0) 1
fib(1) 1
fact(1) 1
fib(2) 1
fact(2) 2
fib(3) 2
fact(3) 6
fib(4) 3
fact(4) 24
fib(5) 5
fact(5) 120
fib(6) 8
fact(6) 720
fib(7) 13
fact(7) 5040
fib(8) 21
fact(8) 40320
fib(9) 34
fact(9) 362880
fib(10) 55
fact(10) 3628800
fib(11) 89
fact(11) 39916800
fib(12) 144
fact(12) 479001600
fib(13) 233
fact(13) 6227020800
fib(14) 377
fact(14) 87178291200
fib(15) 610
fact(15) 1307674368000
fib(16) 987
fact(16) 20922789888000
fib(17) 1597
fact(17) 355687428096000
fib(18) 2584
fact(18) 6402373705728000
fib(19) 4181
fact(19) 121645100408832000
fib(20) 6765
fact(20) 2432902008176640000
*/
```



