#include <stdio.h>
#include <stdlib.h>
int striden(char* str1, char* str2)
{
    while(*str1 == *str2 && *str1 != '\0' && *str2 != '\0')
    {
        str1++;
        str2++;
    }   
    if (*str1 == '\0' && *str2 == '\0')
        return 0;
    else 
        return 1;
}

int main(int argc, char *argv[]) 
{
    FILE *file;
    int current_symb,lineNum = 1, first = 1, i;
    // Проверяем, был ли передан аргумент с именем файла
    if (argc < 2) 
    {
        printf("Не введены имена файлов\n");
        return 1;
    }
    // Проверяем, есть ли флаг "-n"
    int lineNumbering = 0;
    if (striden(argv[1], "-n") == 0) 
    {
        lineNumbering = 1;
        i = 2;
    }  
    else    
        i = 1;
    for (int j = i; j < argc; j++)
    {
        file = fopen(argv[j], "r");
        if (file == NULL) 
        {
            perror("Ошибка открытия файла");
            return 1;
        }
        // Читаем и выводим содержимое файла посимвольно
        while ((current_symb = fgetc(file)) != EOF) 
        {

            if (lineNumbering && lineNum == first)
                printf("%d: ", lineNum++);
            if (lineNumbering && current_symb == '\n') 
                printf("%c%d: ",current_symb, lineNum++);
            else    
                printf("%c",current_symb);
        }
        printf("\n");
        // Закрываем файл
        fclose(file);
    }
    return 0;
}