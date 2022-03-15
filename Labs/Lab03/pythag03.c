#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double pythag(double x, double y)
{
     return sqrt(x * x + y * y);
}


int main(int argc, char *argv[])
{

    if (argc < 3) {
        printf("usage: %s <length> <length>\n", argv[0]);
        exit(1);
    }

    double a = atof(argv[1]);
    double b = atof(argv[2]);

    double c = pythag(a, b);

    printf("Right triangle with sides %.2f and %.2f has "
        "hypotenuse of length %.2f\n", a, b, c);
}
