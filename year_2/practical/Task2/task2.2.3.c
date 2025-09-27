#include <stdio.h>
int main() {
    double x, a, f = 0, f1 = 0, f2 = 0;
    int n;
    scanf("%d %lf", &n, &x);
    n++;
    while (scanf("%lf",&a) != EOF) {
        f1 = f1*x;
        f1 = f1 + f;
        f2 = f2*x + a/n;
        n--;
        f = f*x + a;
    }
    f2 = f2 * x;
    printf("f = %.4lf\nf' = %.4lf\nintegral(f) = %.4lf\n", f, f1, f2);
    return 0;    
    }

