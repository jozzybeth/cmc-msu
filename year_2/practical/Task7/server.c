#define MAX_QUEUE 5
#define MEM_INC_SIZE 8

#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <poll.h>
#include <signal.h>

#define MAX_BUF 256
#define MAX_NAME 21
#define MAX_MESSAGE 256+20+3

int sock_serv, max_clients;
char **all_nick, **temp_all_nick;
struct pollfd *fd_event, *exit_serv, *temp_fd_event; 

void Handler () 
{
    printf("\n###Завершение работы сервера\n");
    shutdown(sock_serv, SHUT_RDWR);
    close(sock_serv);
    free(fd_event);
    for (int i = 0; i < max_clients; i++)
        free(all_nick[i]);
    free(all_nick);
    free(exit_serv);
    exit(0);
}

void all_message(int cur_num, char *buf)
{
    int i;
    for (i = 1; i <= max_clients ; i++)
    {
        if (i == cur_num)
            continue;
        if (fd_event[i].fd != 0 && fd_event[i].fd != -1 && (strlen(all_nick[i - 1]) != 0))
        {
            if (write(fd_event[i].fd, buf, strlen(buf))  == -1)
            {
                perror("\nerror write\n");
                kill(getpid(),SIGINT);
            }
        }
    }
}

void null_array(char *arr, int max)
{
    int i;
    for (i = 0; i < max; i++)
        arr[i] = '\0';
}

void quit_from_server(int num, int *clients)
{
    printf("\n###Клиент %s отсоединён\n", all_nick[num-1]);
    fflush(stdout);
    close(fd_event[num].fd);
    fd_event[num].fd = -1;
    fd_event[num].revents = 0;
    null_array(all_nick[num - 1], MAX_NAME);
    (*clients)--;
    printf("###На сервере осталось: %d клиентов\n",*clients);
    fflush(stdout);
}

void delete_space(char* str) {
    // Удаление пробелов из начала строки
    int len = strlen(str);
    int leadingSpaces = 0;
    while (leadingSpaces < len && str[leadingSpaces] == ' ') 
        leadingSpaces++;

    // Удаление пробелов из конца строки
    int trailingSpaces = 0;
    while (leadingSpaces + trailingSpaces < len && str[len - trailingSpaces - 1] == ' ')
        trailingSpaces++;

    // Копирование без пробелов в новую строку
    for (int i = 0; i < len - leadingSpaces - trailingSpaces; i++) 
        str[i] = str[i + leadingSpaces];

    str[len - leadingSpaces - trailingSpaces] = '\0';
}

int main (int argc, char ** argv)
{
    if (argc < 2)
    {
        perror("\n##Вы не ввели номер порта\n");
        exit(1);
    }
    signal(SIGINT, Handler);
    signal(SIGTSTP, Handler);
    signal(SIGTERM, Handler);
    char buffer[7];
    sock_serv = socket(AF_INET, SOCK_STREAM, 0); //получаем фд сокета для ожидания запросов
    int port, clients, events, temp_socket, i, cur_num, online;
    ssize_t n_read;
    char buf [MAX_BUF];
    char message [MAX_MESSAGE];
    char info_buf [MAX_BUF + MAX_BUF];
    struct sockaddr_in addr;
    if (sock_serv == -1)
    {
        perror("\nerror socket\n");
        exit(1);
    }
    addr.sin_family = AF_INET;
    port = atoi(argv[1]); //номер порта из аргумента командной строки, сокет будет слушать на этом порту
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock_serv, (struct sockaddr *) &addr, sizeof(addr)) == -1) //связали сокет с адресом
    {
        perror("\nerror bind\n");
        exit(1);
    }   
    if (listen(sock_serv,MAX_QUEUE) == -1) //перевели сокет в состояние ожидания соединений
    {
        perror("\nerror listen\n");
        exit(1);
    }
    max_clients = MEM_INC_SIZE;
    clients = 0; //текущее количество клиентов
    fd_event = malloc(sizeof(struct pollfd)*(max_clients + 1));
    exit_serv = malloc(sizeof(struct pollfd));
    exit_serv[0].fd = 0;
    exit_serv[0].events = POLLIN;
    exit_serv[0].revents = 0;
    all_nick = (char **) malloc(sizeof(char*) * max_clients);
    fd_event[0].fd = sock_serv; //ожидается входящее соединение
    fd_event[0].events = POLLIN | POLLERR | POLLOUT | POLLPRI; //запрошенные события
    for (i = 0; i < max_clients; i++)
    {
        all_nick[i] = malloc(MAX_NAME*sizeof(char));
        null_array(all_nick[i], MAX_NAME);
        fd_event[i+1].fd = 0;
        fd_event[i+1].revents = 0;
    }
    fd_event[0].revents = 0;
    printf("\nНачало работы сервера, доступна команда \\exit для завршения работы сервера\n\n");
    fflush(stdout);
    while(1) 
    {
        fd_event[0].revents = 0; //возвращаемые события
        events = poll(fd_event,clients + 1, 100); //ожидает пока какой-то фд будет готов к чтению/записи
        poll(exit_serv,1,100);
        if (exit_serv[0].revents)
        {
            null_array(buffer,7);
            ssize_t ex;
            ex = read(0, buffer, sizeof(buffer));
            buffer[ex - 1] = '\0';
            delete_space(buffer);
            if (ex == 7)
            {   
                printf("\nНекорректная команда\n");
                fflush(stdout);
                char c[1];
                ssize_t n;
                n = read(0,c,1);
                while (c[0] != EOF && c[0] != '\n' && n != 0) 
                    n = read(0,c,1);
            }
            else if (strcmp("\\exit",buffer))
            {
                printf("Некорректная команда\n");
                fflush(stdout);
            }
            else
                kill(getpid(),SIGINT);
            exit_serv[0].revents = 0;
        }
        if (events == -1)
        {
            perror("\nerror poll\n");
            kill(getpid(),SIGINT);
        }
        if (events == 0) //событий не произошло
            continue;   
        if (fd_event[0].revents) //произошло ожидаемое событие
        {
            temp_socket = accept(sock_serv, NULL, NULL); //сокет для обмена информацией
            if (temp_socket == -1)
            {
                perror("\nerror accept\n");
                kill(getpid(),SIGINT);
            }
            clients++;
            if (clients > max_clients) //клиентов больше чем массив
            {
                max_clients += MEM_INC_SIZE;
                temp_fd_event = fd_event;
                temp_all_nick = all_nick;
                all_nick = realloc(all_nick,sizeof(char*)*max_clients);
                fd_event = realloc(fd_event, sizeof(struct pollfd) * (max_clients + 1));
                if(fd_event == NULL || all_nick == NULL)
                {
                    perror("\nerror realloc\n");
                    kill(getpid(),SIGINT);
                }
                for (i = clients; i <= max_clients; i++)
                {
                    all_nick [i - 1] = realloc(all_nick[i - 1], sizeof(char)*MAX_NAME);
                    fd_event[i].fd = 0;
                    fd_event[i].revents = 0;
                }
            }
            for (i = 1; i <= clients; i++) //получили свободный номер для размещения структуры
                if (fd_event[i].fd == 0 || fd_event[i].fd == -1)
                {
                    cur_num = i;
                    break;
                }
            fd_event[cur_num].fd = temp_socket; //внесли в массив нового клиента 
            fd_event[cur_num].events = POLLIN | POLLHUP | POLLERR; //из сокета клиента можно прочитать/разрыв/ошибка
            fd_event[cur_num].revents = 0;
            fd_event[0].revents = 0;
            printf("\n###Новый клиент! Количество клиентов на сервере: %d\n",clients);
            fflush(stdout);
            null_array(buf,MAX_BUF);
            strcpy(buf,"###Добро пожаловать на сервер! Введите ваш ник < 20 символов: \0");
            if (write (fd_event[cur_num].fd, buf, strlen(buf)) == -1) 
            {
                perror("\nerror write\n");
                quit_from_server(i, &clients);
            }
        }
        for (i = 1; i <= max_clients; i++)
        {
            if (fd_event[i].revents & POLLHUP) //клиент отсоединился
            {
                null_array(buf, MAX_BUF);
                strcpy(buf, "\n###Отсоединился: \0");
                strcat(buf,all_nick[i - 1]);
                all_message(i,buf);
                quit_from_server(i, &clients);
            }
            else if (fd_event[i].revents & POLLERR) //ошибка
            {
                printf("\n###Отключение клиента: %s из-за ошибки\n",all_nick[i - 1]);
                fflush(stdout);
                null_array(buf, MAX_BUF);
                strcpy(buf,"Ошибка\0");
                if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                {
                    perror("\nerror write\n");
                    quit_from_server(i, &clients);
                }
                null_array(buf, MAX_BUF);
                strcpy(buf, "\n###Отсоединился: \0");
                strcat(buf,all_nick[i - 1]);
                all_message(i,buf);
                quit_from_server(i, &clients);
            }
            else if (fd_event[i].revents & POLLIN) //есть что прочитать у клиента
            {
                if (strlen(all_nick[i - 1]) == 0) //не ввели ник
                {
                    null_array(buf,MAX_BUF);
                    if ((n_read = read (fd_event[i].fd, buf, MAX_BUF)) == -1) //записали имя входящего в массив
                    {
                        perror("\nerror read name\n");
                        quit_from_server(i, &clients);
                    }
                    else if (n_read == 0) //клиент отключился
                    {
                        quit_from_server(i, &clients);
                    }
                    else if (n_read == 1) //пустое сообщение
                    {
                        null_array(buf,MAX_BUF);
                        strcpy(buf,"\n###Вы ввели пустой ник! Введите другой: \0");
                        if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                        {
                            perror("\nerror write\n");
                            quit_from_server(i, &clients);
                        }

                    }
                    else if (n_read == MAX_NAME)
                    {  
                        char c[1];
                        ssize_t n;
                        n = read(fd_event[i].fd,c,1);
                        while (c[0] != EOF && c[0] != '\n' && n != 0) 
                            n = read(fd_event[i].fd,c,1);
                        null_array(buf, MAX_BUF);
                        strcpy(buf,"\n###Вы превысили допустимый размер ника, введите ник < 20 символов: \0");
                        if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                        {
                            perror("\nerror write\n");
                            quit_from_server(i, &clients);
                        }
                    }
                    else if (n_read > 0)
                    {  
                        buf[n_read-1] = '\0';
                        delete_space(buf);
                        strcpy(all_nick[i-1],buf);
                        null_array(buf, MAX_BUF);
                        strcpy(buf, "\n###На сервер зашел: \0");
                        strcat(buf,all_nick[i-1]);
                        strcat(buf,"\n\n\0");
                        printf("%s", buf);
                        fflush(stdout);
                        all_message(i, buf);
                        null_array(info_buf, MAX_BUF + MAX_BUF);
                        strcpy(info_buf,"\nВы вошли в чат!\nПравила чата:\n"
                            "1. Вводите сообщения < 256 символов\n"
                            "2. \\users - список всех пользователей в чате\n"
                            "3. \\quit <сообщение> - выход с прощальным сообщением\n\n\0");
                        if (write (fd_event[i].fd, info_buf, strlen(info_buf)) == -1) 
                        {
                            perror("\nerror write\n");
                            quit_from_server(i, &clients);
                        }
                    }
                }
                else //ник уже прочитан можно читать сообщения
                {
                    null_array(buf,MAX_BUF);
                    n_read = read(fd_event[i].fd, buf, MAX_BUF);
                    if (n_read == -1) 
                    {
                        perror("\nerror read from client\n");
                        quit_from_server(i, &clients);
                    }
                    else if (n_read == 0) //клиент отключился
                        quit_from_server(i, &clients);
                    else if (n_read == 1) //пустое сообщение
                    {
                        null_array(buf,MAX_BUF);
                        strcpy(buf,"\n###Вы ввели пустое сообщение, оно не будет доставлено! Введите другое\n\0");
                        if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                        {
                            perror("\nerror write");
                            quit_from_server(i, &clients);
                        }

                    }
                    else if (n_read == MAX_BUF)
                    {
                        char c[1];
                        ssize_t n;
                        n = read(fd_event[i].fd,c,1);
                        while (c[0] != EOF && c[0] != '\n' && n != 0) 
                            n = read(fd_event[i].fd,c,1);
                        null_array(buf, MAX_BUF);
                        strcpy(buf,"\n###Вы превысили допустимый размер сообщения, введите сообщение < 256 символов\n\0");
                        if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                        {
                            perror("\nerror write\n");
                            quit_from_server(i, &clients);
                        }
                    }
                    else
                    {
                        buf[n_read - 1] = '\0';
                        delete_space(buf);
                        if (buf[0] == '\\') //команда
                        {
                            if (!strcmp(buf,"\\users\0"))
                            {
                                online = 1;
                                null_array(buf, MAX_BUF);
                                strcpy(buf,"\n###Онлайн пользователи: \n\0");
                                if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                                {
                                    perror("\nerror write\n");
                                    quit_from_server(i, &clients);
                                }
                                for (int j = 0; j < clients; j++)
                                {
                                    if (strlen(all_nick[j]) && j != (i-1))
                                    {
                                        online = 0;
                                        null_array(buf, MAX_BUF);
                                        strcpy(buf,all_nick[j]);
                                        strcat(buf,"\n\0");
                                        if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                                        {
                                            perror("\nerror write\n");
                                            quit_from_server(i, &clients);
                                        }
                                    }
                                }
                                if (online)
                                {
                                    null_array(buf, MAX_BUF);
                                    strcpy(buf,"Онлайн пользователей нет\n\n\0");
                                    if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                                    {
                                        perror("\nerror write\n");
                                        quit_from_server(i, &clients);
                                    }
                                }
                            }
                            else if (!strncmp(buf,"\\quit ",6) || !strcmp(buf,"\\quit\0"))
                            {
                                char quit_message[MAX_BUF];
                                strcpy(quit_message,buf+5);
                                null_array(buf,MAX_BUF);
                                strcpy(buf,"\n###Чат покидает: ");
                                strcat(buf,all_nick[i - 1]);
                                if (quit_message[0]) //есть прощальное сообщение 
                                {
                                    strcat(buf,", его сообщение перед уходом: ");
                                    strcat(buf, quit_message);
                                    strcat(buf,"\n\n\0");
                                }
                                all_message(i, buf);
                                quit_from_server(i, &clients);
                            }
                            else
                            {
                                null_array(buf,MAX_BUF);
                                strcpy(buf,"\n###Введена неверная команда!\n\0");
                                if (write(fd_event[i].fd, buf, strlen(buf)) == -1)
                                {
                                    perror("\nerror write\n");
                                    quit_from_server(i, &clients);
                                }
                            }

                        }
                        else //не команда
                        {
                            buf[n_read - 1] = '\0';
                            null_array(message, MAX_MESSAGE);
                            strcpy(message,"\n-->\0");
                            strcat(message, all_nick[i - 1]);
                            strcat(message,": ");
                            strcat(message, buf);
                            strcat(message,"\n\n\0");
                            all_message(0, message);
                            printf("%s",message);
                            fflush(stdout);
                        }
                    }
                }   
            }
            fd_event[i].revents = 0;
        }
    }
}   





