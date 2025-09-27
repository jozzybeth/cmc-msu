#include <stdio.h>
int main() {
    double x, a, f = 0;
    scanf("%lf", &x);
    while (scanf("%lf", &a) != EOF)
        f = f*x + a;
    printf("f = %.4lf\n", f);
    return 0;    
}

