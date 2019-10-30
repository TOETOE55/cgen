# cgen
a lightweight generator implemented by ucontext.h

## API

```c
// create a generator
gen_t generator(void f(gen_t));
// yield the current sub-routine
value yield(gen_t gen, value v);
// continue a generator
int resume(gen_t gen, value* out, value in);
```



## Example

create and drop a generator:

```c
void foo(gen_t gen) {
    printf("foo start\n");
}

int main() {
    gen_t foo_gen = generator(foo);
    free(foo);
    return 0;
}
```



yield and resume a generator:

```c
void foo(gen_t gen) {
    value v = 0;
    printf("foo start\n");
    v = yield(gen, 100);
    printf("foo is resumed by %lld\n", v);
    v = yield(gen, 99);
    printf("foo is resumed by %lld\n", v);
    v = yield(gen, 98);
    printf("foo is resumed by %lld\n", v);
    printf("foo end\n");
}

int main() {
    gen_t foo_gen = generator(foo);
    value v;
    for (int i = 0; resume(foo_gen, &v, i); ++i) {
        printf("yield %lld from foo\n", v);
    }
    free(foo_gen);
    return 0;
}

/* output
foo start
yield 100 from foo
foo is resumed by 1
yield 99 from foo
foo is resumed by 2
yield 98 from foo
foo is resumed by 3
foo end
*/
```



you can create many generators:

```C
void fib(gen_t gen) {
    value an_1 = 1, an_2 = 0;
    while (an_2 >= 0) {
        yield(gen, an_2);
        value tmp = an_1;
        an_1 = an_1 + an_2;
        an_2 = tmp;
    }
}

void fact(gen_t gen) {
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
    for (int i = 0; resume(fib_gen, &an, i) && resume(fact_gen, &bn, i); ++i) {
        printf("fib(%d) %lld\n", i, an);
        printf("fact(%d) %lld\n", i, bn);
    }

    free(fib_gen);
    free(fact_gen);
    
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



