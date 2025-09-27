#include <stdio.h>

unsigned int fibrecur(unsigned int k) {
    if (k == 0) return 0;
    if (k == 1) return 1;
    return fibrecur(k-2) + fibrecur(k-1);
}

unsigned int fibiter(unsigned int k) {
    if (k == 0) return 0;
    if (k == 1) return 1;
    unsigned int f, f1 = 0, f2 = 1, i;
    for (i = 2; i <= k; i++) {
        f = f1 + f2;
        f1 = f2;
        f2 = f;
    }
    return f;
}

int main() {
    unsigned int x;
    while (scanf("%d",&x) != EOF) {
        printf("iterative = %d\n",fibiter(x));
        printf("recursively = %d\n",fibrecur(x));
    }  
}

