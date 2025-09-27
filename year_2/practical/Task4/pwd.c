#include <stdio.h>
#include <stdlib.h>

int main() {
    char* currentDir = getenv("PWD"); //Получаем значение переменной окружения PWD 
    if (currentDir != NULL) 
        printf("%s\n", currentDir);
    else 
    {
        printf("Невозможно получить путь.\n");
        return 1;
    }
    
    return 0;
}