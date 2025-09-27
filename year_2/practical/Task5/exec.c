#include "exec.h"

int flag_exit = 0;

int cd (int argc, char **argv) 
{
    char *s;
    if (argc == 1)
    {
        s = getenv("HOME");
        if (s == NULL)
        {
            fprintf(stderr, "\nОшибка смены директории!\n\n");
            return 1;
        } 
        else 
        {
            chdir(s);
            return 0;
        }
    } 
    else if (argc > 2)
    {
        fprintf(stderr, "\nОшибка: cd допускает 1 аргумент!\n\n");
        return 1;
    } 
    else if (chdir(argv[1]))
    {
        fprintf(stderr, "\nОшибка смены директории!\n\n");
        return 1;
    }
    return 0;
}

void in_out_files (tree *root) // перенаправление входного и выходного потоков
{
    if (root -> outfile != NULL) 
    {
        int f_out;
        if (root -> append == 1)
            f_out = open (root -> outfile, O_WRONLY| O_APPEND |O_CREAT, 0664);
        else
            f_out = open (root -> outfile, O_WRONLY| O_TRUNC |O_CREAT, 0664);
        if (f_out == -1)
        {
            fprintf(stderr, "\nНевозможно перенаправить вывод\n\n");
            exit(1);
        } else 
            dup2(f_out, 1);
        close (f_out);
    }

    if (root -> infile != NULL)
    {
        int f_in;
        f_in  = open (root -> infile, O_RDONLY);
        if (f_in == -1)
        {
            fprintf(stderr, "\nВходного файла не существует\n\n");
            exit(1);
        } else 
            dup2(f_in, 0);
        close (f_in);
    }
}

void cmd_processing (tree *cur_tree, int backgrnd) // выполнение команды
{
    if (!strcmp(cur_tree -> argv[0],"cd") || !strcmp(cur_tree -> argv[0],"exit"))
        exit(0);
    in_out_files (cur_tree);
    
    if ((backgrnd) && (cur_tree -> backgrnd)) //backgrnd для того чтобы конвейер брал данные из предыдущего а не из devnull
    { 
        int f = open("/dev/null", O_RDONLY);
        dup2(f, 0);
        close(f);
    }
    execvp(cur_tree -> argv[0], cur_tree -> argv);
    fprintf(stderr, "%s: Команда не найдена\n", *(cur_tree -> argv));
    exit(1);
}

// выполнение конвейера
int pipe_processing (tree *root)  // внук все выполняет, сын ждет внука или не ждет (фоновый), отец ждет сына
{ 
    int ppid, res = 0;
    if (root == NULL)
        return 0;
    if (root -> argv && !strcmp(root-> argv[0], "exit"))
    {
        flag_exit = 1;
        return 0;
    }
    if (root -> argv && !strcmp(root -> argv[0], "cd") && root -> pipe == NULL)
    {
        return cd(root -> argc, root -> argv);
    }
    if ((ppid = fork()) == 0)
    {
        int pid;
        if ((pid = fork()) == 0)
        {
            tree* cur_tree = root;
            int fd_temp, fd[2];
            fd_temp = -1;
            if (root -> backgrnd)
                signal(SIGINT, SIG_IGN);
            else
                signal(SIGINT, SIG_DFL);
            while (cur_tree != NULL)
            {    
                if (cur_tree == root) // первая вершина
                {                     
                    if (cur_tree -> pipe != NULL) // первая и не последняя вершина
                    {         
                        if (pipe(fd) < 0)
                            exit(1);
                        if ((pid = fork()) == 0)
                        {
                            dup2(fd[1], 1);
                            close(fd[0]);
                            close(fd[1]);
                            cmd_processing(cur_tree, 1);
                        }
                        close(fd[1]);
                        fd_temp = fd[0];
                    }
                    else if ((pid = fork()) == 0)           // первая и последняя
                        cmd_processing(cur_tree, 1);
                }
                else if (cur_tree -> pipe == NULL) // последняя вершина
                {      
                    if ((pid = fork()) == 0)
                    {
                        dup2(fd_temp, 0);
                        close(fd_temp);
                        cmd_processing(cur_tree, 0);
                    }
                    close(fd_temp);
                }
                else // не первая и не последняя вершина
                {                                      
                    if (pipe(fd) < 0)
                        exit(1);
                    else if ((pid = fork()) == 0)
                    {
                        dup2(fd_temp, 0);
                        dup2(fd[1], 1);
                        close(fd[0]);
                        close(fd[1]);
                        close(fd_temp);
                        cmd_processing(cur_tree, 0);
                    }
                    close(fd[1]);
                    close(fd_temp);
                    fd_temp = fd[0];
                }
                cur_tree = cur_tree -> pipe;
            }
            waitpid(pid, &res, 0); //в res записывается результат последней команды
            while (wait(NULL) != -1);

            if (WIFEXITED(res))
                res = WEXITSTATUS(res);
            exit(res);
        }
        if (root -> backgrnd) 
            exit(0);
        else 
        {
            waitpid(pid, &res, 0);
            if (WIFEXITED(res))
                res = WEXITSTATUS(res);
            exit(res);
        }
    } 
    waitpid(ppid, &res, 0);
    if (WIFEXITED(res))
        res = WEXITSTATUS(res);
    return res;
}

void handler (tree *cur_tree)
{
    if (cur_tree == NULL)
        return;
    int status_prev = 0;        // статус завершения предыдущей команды
    //int tnext = (1, если ;) или (2, если &&) или (3, если ||)
    while (cur_tree != NULL)
    {
        if (cur_tree -> tnext == 2 && status_prev == 0) // &&
            status_prev = pipe_processing(cur_tree);

        else if (cur_tree -> tnext == 3 && status_prev == 1) // ||
            status_prev = pipe_processing(cur_tree);

        else if (cur_tree -> tnext < 2) // ; или один конвейер
            status_prev = pipe_processing(cur_tree);

        if  (flag_exit == 1)
            return;
        cur_tree = cur_tree -> next;
    }
}
