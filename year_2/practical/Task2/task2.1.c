#include <stdio.h>
int main() {
    double eps, x, xi, xj;
    scanf("%lf", &eps);
    while (scanf("%lf", &x) != EOF) {
        xj = 1;
        do {
            xi = xj;
            xj = (double)1/2 * (xi + x/xi);
        } while ((xi - xj) >= eps || (xj - xi) >= eps);
        printf("sqrt(%.10g) = %.10g\n", x, xj);
    } 
    return 0;    
    }

