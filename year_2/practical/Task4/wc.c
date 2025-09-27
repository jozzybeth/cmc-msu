#include <stdio.h>

int main(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        printf("Не указаны имена файлов\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) 
    {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) 
        {
            printf("Не удалось открыть файл '%s'\n", argv[i]);
            continue;
        }
        int lines = 0, words = 0, bytes = 0, in_word = 0, current_char;
        while ((current_char = fgetc(file)) != EOF) 
        {
            bytes++;
            if (current_char == '\n')
                lines++;
            if (current_char == ' ' || current_char == '\n' || current_char == '\t' || current_char == '\r') 
                in_word = 0;
            else if (!in_word) 
            {
                words++;
                in_word = 1;
            }
        }
        printf("%s: Количество строк: %d Количество слов: %d  Количество символов: %d\n",argv[i], lines, words, bytes);
        fclose(file);
    }
    return 0;
}