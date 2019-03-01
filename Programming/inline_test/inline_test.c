
#include <stdio.h>

//void test_inline_func1(int , int ) __attribute__((always_inline));


__attribute__((always_inline)) void inline test_inline_func1(int a, int b) {
    printf ("a=%d and b=%d\n", a, b);
}

__attribute__((always_inline)) int inline test_inline_func2(int x) {
    return x*x;
}

int main() {

    int tmp;

    test_inline_func1(2,4);
    tmp = test_inline_func2(5);

    printf("square val=%d\n", tmp);

    return 0;
}



