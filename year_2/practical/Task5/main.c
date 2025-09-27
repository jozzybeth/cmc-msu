#include "list.h"
#include "tree.h"
#include "exec.h"

extern int flag_exit;

int main() 
{
    List lst;       //char **
    tree *tr;       //дерево выполнения команд
    int sizelist, ifeof;
    int isthere;    //для скобок
    char *cwd = (char *) malloc (PATH_MAX * sizeof (char));
    while (1) 
    {  
           // sigsetjmp(buf, 1);
            isthere = 0;
            if (getcwd(cwd, sizeof(char) * PATH_MAX) == NULL)
            {
                fprintf(stderr, "\nНевозможно получить текущий путь => $HOME\n\n");
                cwd = getenv("HOME");
                chdir(cwd);
            }
            else 
                printf("%s$ ", cwd); //печатаем директорию
            ifeof = build_list(&lst, &sizelist, &isthere);    //создаем список слов
            if (!isthere) 
            {
                if (ifeof) //конец 
                {
                    printf("\n");
                    break;
                }
                if (lst == NULL) //пустая строка
                {
                    fflush(stdin);
                    clearlist(&lst);
                    continue;
                }
                changelist(lst, sizelist);
              //  printlist(lst, sizelist);
                tr = create_tree(lst);
                print_tree(tr, 0);
                handler(tr);
                clear_tree(&tr);
            }
            else
                fprintf(stderr, "я не обрабатываю скобки \n");
            clearlist(&lst);
            if (flag_exit) 
                break;
        
    }
    free(cwd); 
    return 0;
}
