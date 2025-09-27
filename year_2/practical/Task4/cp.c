#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

int striden(char* str1, char* str2)
{
    while(*str1 == *str2 && *str1 != '\0' && *str2 != '\0')
    {
        str1++;
        str2++;
    }   
    if (*str1 == '\0' && *str2 == '\0')
        return 1;
    else 
        return 0;
}

void copy_permissions(const char *src, const char *dest) 
{
    struct stat statbuf;
    if (stat(src, &statbuf) == 0) 
        chmod(dest, statbuf.st_mode);
}

void copy_file(const char *src, const char *dest) 
{
    FILE *source = fopen(src, "r");
    FILE *destination = fopen(dest, "w");
    char ch;
    while ((ch = fgetc(source)) != EOF)
        fputc(ch, destination);
    fclose(source);
    fclose(destination);
    copy_permissions(src, dest);
}

void copy_directory(const char *src, const char *dest) 
{
    struct stat st;
    if (stat(src, &st) == 0) 
    {
        mkdir(dest, st.st_mode); //создаем папку с именем dest, если она есть, то она не пересоздается
        DIR *dir = opendir(src); //открытие каталога, получение дескриптора
        struct dirent *info;
        while ((info = readdir(dir)) != NULL) //чтение очередной записи в каталоге
        {
            if (striden(info->d_name, ".") == 0 && striden(info->d_name, "..") == 0) //если этот каталог не текущий и не родительский
            {
                char src_path[1024];
                char dest_path[1024];
                snprintf(src_path, sizeof(src_path), "%s/%s", src, info->d_name); //путь к файлу/каталогу через папку источник
                snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, info->d_name); //путь к файлу/каталогу через папку получатель
                struct stat st_sub;
                if (stat(src_path, &st_sub) == 0) 
                    if (S_ISDIR(st_sub.st_mode)) //
                        copy_directory(src_path, dest_path); //если текущая запись каталог, то рекурсивно идем дальше
                    else 
                        copy_file(src_path, dest_path); //если текущая запись файл, то записываем его в получателя
            }
        }
        closedir(dir);
        copy_permissions(src, dest); //копируем права доступа
    }
}

int main(int argc, char ** argv) 
{
    struct stat stat_src, stat_dest;
    if (argc != 3) 
    {
        perror("Использование: cp source_file/directory dest_file/directory\n");
        return 1;
    }
    if (stat(argv[1], &stat_src) == -1) 
    {
        perror("Ошибка при получении информации о файле/директории");
        return 1;
    }
    if (striden(argv[1],argv[2]))
    {
        printf("%s и %s одинаковые файлы\n",argv[1],argv[2]);
        return 0;
    }    
    if (stat(argv[2], &stat_dest) != -1) 
    {
        if (S_ISDIR(stat_src.st_mode) && S_ISDIR(stat_dest.st_mode)) 
            copy_directory(argv[1], argv[2]);
        else if (!(S_ISDIR(stat_src.st_mode)) && !(S_ISDIR(stat_dest.st_mode)))
            copy_file(argv[1], argv[2]);
        else 
            perror("Нельзя копировать файл в директорию или директорию в файл\n");
            return 1;
    } 
    else if (errno == ENOENT &&  S_ISDIR(stat_src.st_mode)) //ENOENT записывается в errno, если файла/каталога dest не существует
        copy_directory(argv[1], argv[2]);
    else if (errno == ENOENT &&  !(S_ISDIR(stat_src.st_mode)))
        copy_file(argv[1], argv[2]);
    else 
    {
        perror("Ошибка при получении информации о файле/директории");
        return 1;
    }
    return 0;
}

