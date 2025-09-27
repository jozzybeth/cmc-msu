#include <stdio.h>
#define N 63
int main () {
	char X[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9',' '};
	int min,i,j;
	printf("Task 1.2, Горина Елизавета 212\n4. Коды символов\n");
	for (i = 0; i < N; i++){
		min = i;
		for (j = i+1; j < N ; j++)
			if (X[j] < X[min]) min = j;
		j = X[i];
		X[i] = X[min]; 
		X[min] = j; 
	};
	printf("Упорядоченные символы в порядке возрастания их кодов:\n");
	for (i = 0; i < N; i++){
		printf("%c ", X[i]);
	};
	printf("\n");
}
