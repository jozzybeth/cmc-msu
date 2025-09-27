#include <stdio.h>
int main() {
    double x, a, f = 0, f1 = 0;
    scanf("%lf",&x);
    while (scanf("%lf", &a) != EOF) {
        f1 = f1*x;
        f1 = f1 + f;
        f = f*x + a;
    }
    printf("f = %.4lf\nf' = %.4lf\n", f, f1);
    return 0;    
    }

