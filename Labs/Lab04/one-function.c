#include <stdio.h>

int main()
{
    int a;
    int b;
    int *pp;

    a = 10;
    b = 20;

    printf("%d %d\n", a, b);

    pp = &a;
    *pp = 333;

    printf("%d %d\n", a, b);

    pp = &b;
    a = 444;
    b = 555;

    printf("%d %d\n", a, b);
    printf("%d\n", *pp);
    printf("%p\n", (void *)pp);
}
