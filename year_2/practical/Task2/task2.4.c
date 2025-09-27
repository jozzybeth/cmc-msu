#include <stdio.h>

double str2double(char str[]) {
    double x = 0, x1 = 0.1;
    int x2 = 0,i = 0;
    char sign = 1;
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    if (str[0] == '+')
        i++;
    while ((str[i] != '\0') && (str[i] != '.') && (str[i] != 'E') && (str[i] != 'e') && (str[i] != 'F') && (str[i] != 'f')) 
        x = x*10 + str[i++] - '0';
    if (str[i] == '.') 
        while ((str[++i] != '\0') && (str[i] != 'E') && (str[i] != 'e') && (str[i] != 'F') && (str[i] != 'f')) {
            x = x + (str[i] - '0')*x1;
            x1 *= 0.1;
        }
    if ((str[i] == 'E') || (str[i] == 'e')) {
        switch (str[++i]) {
        case '-': 
            x1 = 0.1;
            i++;
            break;
        case '+':
            x1 = 10;
            i++; 
        default: 
            x1 = 10;      
        }
        while ((str[i] != '\0') && (str[i] != 'F') && (str[i] != 'f')) 
            x2 = x2*10 + str[i++] - '0';
        for (i = 1; i <= x2; i++)
            x *= x1;
    }
    x = x*sign;
    return x;
}
int main() {
    char str[200];
    double x;
    while (scanf("%s",str) != EOF) {
        x = str2double(str);
        printf("x = %.10g\n",x);
    }  
}

