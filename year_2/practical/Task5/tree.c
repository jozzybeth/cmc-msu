#include "tree.h"

void clear_argv (char **argv) 
{
    if (argv == NULL)
        return;
    for (int i = 0; argv[i] != NULL; i++)
        free(argv[i]);
    free (argv);
    argv = NULL;
    return;
}

void clear_tree(tree **root) 
{
    tree *term_t = *root;
    if(term_t == NULL)
        return;
    clear_argv(term_t->argv);
    clear_tree(&term_t->pipe);
    clear_tree(&term_t->next);
    free(term_t);
    *root = NULL;
}

tree *make_cmd_null(void) 
{   
    tree *cur = malloc(sizeof(tree));
    cur->argc = 0;
    cur->argv = NULL;
    cur->infile = NULL;
    cur->outfile = NULL;
    cur->append = 0;
    cur->backgrnd = 0;
    cur->tnext = 0;
    cur->pipe = NULL;
    cur->next = NULL;
    return cur;
}

void add_arg(List lst, tree* cur_tree) // добавить argv и argc
{ 
    char **term = NULL;
    int size_term = 0;
    char *lex = *lst;    // одно слово 
    while (lex != NULL && symset(*lex)) //если первый символ "хороший"
    {
        if (term == NULL) 
        {
            term = malloc(2*sizeof(char*)); //для слова и для NULL
            *term = NULL;
        }
        else 
            term = realloc(term, (size_term+2)*sizeof(char*));
        size_term++;
        term[size_term-1] = malloc((strlen(lex)+1)*sizeof(char));
        term[size_term] = NULL;
        strcpy(term[size_term-1], lex);
        lst++;
        lex = *lst;
    }
    cur_tree->argc = size_term; //количество аргументов
    cur_tree->argv = term; //аргументы
    return;
}

void make_bgrnd(tree *t) // в фоновый режим себя и всех в конвейере после себя
{    
    if (t == NULL)
        return;
    t -> backgrnd = 1;
    make_bgrnd(t -> pipe);
}

tree* simple_command (List plex) // argv argc infile outfile sleep echo
{       
    tree* cur_tree = make_cmd_null();
    add_arg(plex, cur_tree);
    plex = plex + cur_tree -> argc ;     // пропуск команды и аргумента
    if (*plex != NULL && **plex == '<') // infile
    {      
        plex++; // стрелочка
        if (*plex == NULL)
        { 
            fprintf(stderr, "\nОтсутствует аргумент у ввода в файл!\n\n");
            clear_tree(&cur_tree);
            return NULL;
        }   
        cur_tree -> infile = *plex;
        plex++; //пропустили файл
    }
    if (*plex != NULL && **plex == '>') // outfile
    {         
    // проверка следующий == '>', значит меняем флаг
        if((*plex)[1] == '>') 
        {
            cur_tree -> append = 1;          // append file
        } 
        plex++; // стрелочка
        if (*plex == NULL)
        {
            fprintf(stderr, "\nОтсутствует аргумент у вывода в файл!\n\n");
            clear_tree(&cur_tree);
            return NULL;
        }
        cur_tree -> outfile = *plex;
    }
    return cur_tree;
} 

char **conv_or_not(List plex) 
{
    while(*plex != NULL && strcmp(*plex, "|")) //пока не кончился список строк и пока слово не равно |
        if (!(strcmp(*plex, "||") && strcmp(*plex, "&&") && strcmp(*plex, ";") && strcmp(*plex, "&")))
            return NULL;
        else
            plex++;
    if (*plex == NULL)
        return NULL;
    return plex;
}

tree *conveyor (tree *cur_tree, List plex) // создаем каркас дерева
{  
    tree *prev_tree = cur_tree;
    cur_tree = simple_command(plex);   // добавили аргументы
    tree *root = cur_tree;
    while ((plex = conv_or_not(plex))) // дальше конвейер?
    { 
        plex++;
        if (*plex == NULL) //если после | ничего нет
        {
            fprintf(stderr, "\nКонвейер не завершен!\n\n");
            return NULL;
        }
        prev_tree = cur_tree; //сохраняем предыдущее дерево

        cur_tree = simple_command(plex); //заполянем дерево новой командой

        prev_tree -> pipe = cur_tree; //добавили в pipe предыдущей команды текущее дерево
    }
    return root; //возвращаем дерево первой команды
}

tree *list_of_com(tree* cur_tree, List plex)
{
    tree *prev_tree = cur_tree;
    cur_tree = conveyor(cur_tree, plex);
    tree *root = cur_tree;
    int next;
    while (*plex != NULL) 
    {
        if (cur_tree == NULL)
            return NULL;
        if (cur_tree -> argv != NULL) 
        {
            while (*plex != NULL)
            {
                if (**plex == '&' || **plex == ';'  || !strcmp(*plex,"||"))
                    break;
                plex++;
            }  // дошли до спец символа 
            if  (*plex == NULL) //спец символов не было, дополнять дерево не нужно
                break;
            int flag = 0;   // flag = 1 если & или ;
            if (!strcmp(*plex,"&")) 
            {
                flag = 1;
                make_bgrnd(cur_tree);
                plex++;  
            } else if(!strcmp(*plex, "&&")) 
            {
                next = 2;
                plex++;
            } else if(!strcmp(*plex, "||")) 
            {
                next = 3;
                plex++;
            } else if(!strcmp(*plex, ";")) 
            {
                next = 1;
                flag = 1;
                plex++;
            } 
            if (*plex == NULL && flag) //команды & и ; не требуют после себя команд
                break;
            else if (*plex == NULL) //необходима команда после этих операций
            { 
                fprintf(stderr, "\nОтсутствует команда после && или ||!\n\n");
                clear_tree(&cur_tree);
                return NULL;
            }
        }
        else
        {
            fprintf(stderr, "\nОтсутствуют аргументы!\n\n");
            clear_tree(&cur_tree);
            return NULL;
        }
        prev_tree = cur_tree;
        cur_tree = conveyor(prev_tree, plex);
        if (cur_tree)
            cur_tree ->tnext = next;
        prev_tree -> next = cur_tree;
    }
    return root;
}

tree *create_tree (List lst) 
{
    /*основная функция*/
    tree *cur_tree = NULL;           // начало дерева
    cur_tree = list_of_com(cur_tree, lst);
    return cur_tree;
}

//печать дерева

void make_shift(int n)
{
    while(n--)
        printf(" ");
}

void print_argv(char **p, int shift)
{
    char **q = p;
    if(p != NULL)
        while(*p != NULL)
        {
            make_shift(shift);
            printf("argv[%d] = %s\n",(int) (p-q), *p);
            p++;
        }
}

void print_tree (tree *t, int shift)
{
    if (t == NULL)
        return;
    char **p;
    p = t->argv;

    make_shift(shift);
    printf("argc = %d\n", t->argc);
    if(p != NULL)
        print_argv(p, shift);
    
    make_shift(shift);
    if(t->infile == NULL)
        printf("infile = NULL\n");
    else
        printf("infile = %s\n", t->infile);

    make_shift(shift);
    if(t->outfile == NULL)
        printf( "outfile = NULL\n");
    else
        printf("outfile = %s\n", t->outfile);

    make_shift(shift);
    printf("append = %d\n", t->append);

    make_shift(shift);
    printf("background = %d\n", t->backgrnd);

    make_shift(shift);
    printf("type = %s\n", t->tnext == 1? "NXT": t->tnext == 3? "OR": t->tnext == 2? "AND": "0" );

    make_shift(shift);
    if(t->pipe == NULL)
        printf("pipe = NULL \n");
    else
    {
        printf("pipe---> \n");
        print_tree(t->pipe, shift + 5);
    }
    make_shift(shift);
    if(t->next == NULL)
        printf("next = NULL \n");
    else
    {
        printf("next---> \n");
        print_tree(t->next, shift + 5);
    }
}
