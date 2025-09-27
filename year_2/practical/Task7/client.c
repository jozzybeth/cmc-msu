#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/in.h> 
#include <poll.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_BUF 512

int socket_client;
struct pollfd *fd_event;

void Handler()
{
    wait(NULL);
    shutdown(socket_client, SHUT_RDWR);
    close(socket_client);
    free(fd_event);
    printf("\n###Завершение работы клиента\n");
    exit(0);
}

void Handler_2()
{
    signal(SIGCONT, SIG_DFL);
    exit(0);
}

void null_array(char *arr, int max)
{
    int i;
    for (i = 0; i < max; i++)
        arr[i] = '\0';
}


int main (int argc, char **argv)
{
    signal(SIGINT, Handler);
    signal(SIGTSTP, Handler);
    signal(SIGTERM, Handler);
    int port, pid;
    struct sockaddr_in addr;
    ssize_t n_read;
    char buf[MAX_BUF];
	if (argc < 3) 
    {
        perror("###Не указан номер сервера или номер порта\n");
        exit(1);
    }

    if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("error socket\n");
        exit(1);
    }

    port = atoi(argv[1]); //порт из командной строки
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); //перевод порта в сетевой порядок
    if(!inet_aton(argv[2], &(addr.sin_addr))) 
    {
        perror("incorrect IP\n");
        exit(1);
    } 

    if (connect(socket_client, (struct sockaddr*) &addr, sizeof(addr)) == -1) //установление соединения с сервером 
    {
        perror("error connect\n");
        exit(1);
    }

    fd_event = malloc(sizeof(struct pollfd));

    if ((pid = fork()) == 0) //сын принимает сообщения от сервера и выдает инфу пользователю
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, Handler_2);
        fd_event[0].fd = socket_client;
        fd_event[0].events = POLLIN; //можно считывать данные
        fd_event[0].revents = 0;
        int event;
        while (1)
        {
            event = poll(fd_event, 1, 100); //время ожидания 100

            if (event == -1)
            {
                perror("error poll");
                exit(1);
            }

            if (event == 0) //событий не произошло
                continue;

            if (fd_event[0].revents & POLLIN) //есть что считать
            {
                null_array(buf, MAX_BUF);
                n_read = read(fd_event[0].fd, buf, MAX_BUF - 1);
                if (n_read == 0)
                {
                    kill(getppid(),SIGINT);
                    exit(0);
                }
                else if (n_read == -1)
                {
                    perror("error read from server\n");
                    continue;
                }
                else
                {
                    char* temp = "Ошибка\0";
                    if (!strcmp(buf,temp))
                    {
                        printf("\n###Ошибка на сервере\n");
                        kill(getppid(),SIGINT);
                        exit(0);
                    }
                    printf("%s", buf);
                    fflush(stdout);
                }
                fd_event[0].revents = 0;
            }
        }
    }
    while (1) //читает команды пользователя и передает команды серверу
    {   
        null_array(buf, MAX_BUF);
        n_read = read(0, buf, MAX_BUF - 1);
        if (n_read == 0)
        {
            kill(pid, SIGKILL);
            kill(getpid(),SIGINT);
        }
        if (write(socket_client, buf, strlen(buf)) == -1)
        {
            perror("error write to server");
            kill(pid, SIGKILL);
            kill(getpid(),SIGINT);
        }
    }
}