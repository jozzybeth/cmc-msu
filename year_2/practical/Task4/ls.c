#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

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

void listFiles(const char *path, int recursive, int group, int atributes)
{
    struct dirent *info;
    DIR *dc; /* декскриптор каталога */

    if ((dc = opendir(path)) == NULL) /* открытие каталога */
    { 
        perror("opendir"); /* если открыть каталог не удалось, то выводим в stdin ошибку */
        return;
    }

    while ((info = readdir(dc))) /* считывает очередную запись в каталоге */
    { 
        char fullpath[1024]; /* строка для хранения полного пути к файлу или каталогу. */
        struct stat statbuf; /* структура для хранения информации о файле или каталоге. */
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, info->d_name); /* формирование полного пути к файлу. */
        if (stat(fullpath, &statbuf) < 0) /* возвращает информацию о файле */
        { 
            perror("stat"); /* при ошибке, выводим в stdin */
            continue;
        }

        if (S_ISDIR(statbuf.st_mode) && striden(info->d_name, "..") == 0 && striden(info->d_name, ".") == 0 && recursive) 
            listFiles(fullpath, 1, group, atributes); /* текущая запись в каталоге подкаталог и флаг -R */
        if (striden(info->d_name, "..") == 0 && striden(info->d_name, ".") == 0) 
        {
            if (atributes) 
            {
                printf("%c", (S_ISDIR(statbuf.st_mode)) ? 'd' : '-');
                printf("%c%c%c%c%c%c%c%c%c ", 
                    (statbuf.st_mode & S_IRUSR) ? 'r' : '-',
                    (statbuf.st_mode & S_IWUSR) ? 'w' : '-',
                    (statbuf.st_mode & S_IXUSR) ? 'x' : '-',
                    (statbuf.st_mode & S_IRGRP) ? 'r' : '-',
                    (statbuf.st_mode & S_IWGRP) ? 'w' : '-',
                    (statbuf.st_mode & S_IXGRP) ? 'x' : '-',
                    (statbuf.st_mode & S_IROTH) ? 'r' : '-',
                    (statbuf.st_mode & S_IWOTH) ? 'w' : '-',
                    (statbuf.st_mode & S_IXOTH) ? 'x' : '-'
                );
                if (S_ISLNK(statbuf.st_mode))
                    printf("Символическая ссылка ");
                else if (S_ISREG(statbuf.st_mode))
                    printf("Обычный файл ");
                else if (S_ISDIR(statbuf.st_mode))
                    printf("Каталог ");
                else if (S_ISCHR(statbuf.st_mode))
                    printf("Устройство символьного типа ");
                else if (S_ISBLK(statbuf.st_mode))
                    printf("Устройство блочного типа ");
                else if (S_ISFIFO(statbuf.st_mode))
                    printf("FIFO ");
                else if (S_ISSOCK(statbuf.st_mode))
                    printf("Сокет ");
                struct passwd *pwd = getpwuid(statbuf.st_uid);
                if (pwd != NULL) 
                    printf("Имя владельца: %s ", pwd->pw_name);
                else 
                    printf("Идентификатор владельца: %d, ", statbuf.st_uid);
                printf("Размер файла: %ld байт ",statbuf.st_size);
            }
            if (group) 
            {
                struct group *grp = getgrgid(statbuf.st_gid);
                if (grp != NULL) 
                    printf("Имя группы: %s ", grp->gr_name);
                else 
                    printf("Идентификатор группы: %d", statbuf.st_gid);
        
            }

            printf("%s\n", info->d_name); /*  вывод имени файла или каталога на экран. */
        }
    }

    closedir(dc); /* закрывает декскриптор каталога */
}


int main(int argc, char ** argv)
{
    int i, recursive = 0, group = 0, atributes = 0;
    for (i = 1; i < argc; i++) 
    {
        if (striden(argv[i],"-R")) 
            recursive = 1;
        if (striden(argv[i],"-g")) 
            group = 1;
        if (striden(argv[i],"-l")) 
            atributes = 1;     
    }
    listFiles(".",recursive, group, atributes);

    return 0;
}