#include <stdio.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#define COUNT_F 5
#define LEFT (i - 1 + COUNT_F) % COUNT_F //макрос, определяющий номер левого философа
#define RIGHT (i + 1) % COUNT_F //макрос, определяющий номер правого философа
#define THINKING 0 //код состояния, когда философ размышляет
#define HUNGRY 1 //код состояния, когда философ хочет поесть
#define EATING 2 //код состояния, когда философ ест
struct sembuf s_mut, s_mas, s_state;
int s, mut, state;
union semun 
{
    int val;
    struct semid_ds *buf;
    ushort array [COUNT_F];
} arg;

void SigHandler(int s)
{
        _exit(0);
}

void eat(int i) 
{
        printf("Philosopher #%d is eating...\n", i + 1);
        fflush(stdout);
        sleep(2);
        printf("Philosopher #%d stopped eating...\n", i + 1);
        fflush(stdout);
}
 
void think(int i) 
{
        printf("Philosopher #%d is thinking...\n", i + 1);
        fflush(stdout);
        sleep(2);
}
 
void test(int i) 
{
        int i_state = semctl(state,i,GETVAL,0);
        int i_LEFT = semctl(state,LEFT,GETVAL,0);
        int i_RIGHT = semctl(state,RIGHT,GETVAL,0);
        if (i_state == HUNGRY && i_LEFT != EATING && i_RIGHT != EATING) 
        {
                semctl(state,i,SETVAL,(int) EATING);
                s_mas.sem_op = 1;
                s_mas.sem_num = i;
                semop(s,&s_mas,1);
        }
}
 
void take_forks(int i) 
{
        s_mut.sem_op = -1;
        semop(mut,&s_mut,1);
        semctl(state,i,SETVAL,(int) HUNGRY);
        test(i);
        s_mut.sem_op = 1;
        semop(mut,&s_mut,1);
        s_mas.sem_op = -1;
        s_mas.sem_num = i;
        semop(s,&s_mas,1);
}
 
void put_forks(int i) 
{
        s_mut.sem_op = -1;
        semop(mut,&s_mut,1);
        semctl(state,i,SETVAL,(int) THINKING);
        test(LEFT);
        test(RIGHT);
        s_mut.sem_op = 1;
        semop(mut,&s_mut,1);
}
 
void philosopher(int i) 
{
        while (1) 
        { 
                think(i); 
                take_forks(i);
                eat(i); 
                put_forks(i); 
        }
}
 
int main() 
{
        state = semget(IPC_PRIVATE,COUNT_F,0666|IPC_CREAT);
        s = semget(IPC_PRIVATE,COUNT_F,0666|IPC_CREAT);
        mut = semget(IPC_PRIVATE,1,0666|IPC_CREAT);
        for (int i = 0; i < COUNT_F; i++)
                arg.array[i] = 0;
        semctl(mut,0,SETVAL,(int) 1);
        semctl(s,0,SETALL,arg.array);
        semctl(state,0,SETVAL,arg.array);
        s_mut.sem_flg = 0;
        s_mut.sem_num = 0;
        s_mas.sem_flg = 0;
        s_state.sem_flg = 0;
        signal(SIGINT,SigHandler);
        for (int i = 0; i < COUNT_F; i++)
                if (fork () == 0)
                     philosopher(i);
        signal(SIGINT,SIG_IGN);
        for (int i = 0; i < COUNT_F; i++)
                wait(NULL);
        semctl(s,0,IPC_RMID,0);
        semctl(mut,0,IPC_RMID,0);
        semctl(state,0,IPC_RMID,0);
        return 0;
}