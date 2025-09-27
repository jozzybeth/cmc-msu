#include "list.h"

#define SIZE 16

static int c;               /*текущий символ */
static char * buf;          /* буфер для накопления текущего слова*/
static int sizebuf;         /* размер буфера текущего слова*/
static int sizelist;        /* размер списка слов*/
static int curbuf;          /* индекс текущего символа в буфере*/
static int curlist;         /* индекс текущего слова в списке*/


void clearlist(List *lst)
{
    List term = *lst;
    int i;
    sizelist = 0;
    curlist = 0;
    if (term == NULL) 
        return;
    for (i = 0; term[i] != NULL; i++)
        free(term[i]);
    free(term);
    *lst = NULL;
}

void null_list(List *lst) 
{
    sizelist = 0;
    curlist = 0;
    *lst = NULL;
}

void termlist(List *lst) 
{
    List term = *lst;
    if (term == NULL) return;
    if (curlist > (sizelist - 1))
        term = (List) realloc(term, (sizelist+1)*sizeof(*term));   
    term[curlist] = NULL;
    term = (List) realloc(term, (sizelist = curlist+1)*sizeof(*term));
    *lst = term;
}

void changelist(List lst, int size) 
{
    if (lst == NULL)
    {
        return;
    }
    for (int i = 0; i < size-1; i++) 
    {
        if (!strcmp(lst[i], "$USER\0")) 
        {
            struct passwd *temp;
            temp = getpwuid(geteuid());
            lst[i] = realloc(lst[i], strlen(temp->pw_name) + 1);
            strcpy(lst[i], temp -> pw_name);
        }   
        else if (!strcmp(lst[i], "$EUID\0")) 
        {  
            int uid = (int) geteuid(), len, temp;
            temp = uid;
            for (len = 0; temp > 9; len++) 
                temp = temp / 10;
            lst[i] = realloc(lst[i], len + 2);
            sprintf(lst[i], "%d", uid);
        }
        else if (!strcmp(lst[i], "$SHELL\0")) 
        {
            int len = strlen(getenv("PWD"));
            lst[i] = realloc(lst[i], len + 1);
            strcpy(lst[i], getenv("PWD"));
        }
        else if (!strcmp(lst[i], "$HOME\0")) 
        {
            int len = strlen(getenv("HOME"));
            lst[i] = realloc(lst[i], len + 1);
            strcpy(lst[i], getenv("HOME"));
        }
    }           
    return;
}

void nullbuf() 
{
    buf = NULL; //буфер для накопления текущего слова
    sizebuf = 0;
    curbuf = 0;
}

//перевыделение памяти под буфер
void addsym() 
{
    if (curbuf > (sizebuf-1)) {
        char * temp = realloc(buf, sizebuf += SIZE);
        if (temp != NULL) buf = temp;
    }
    buf[curbuf++] = c;
}

void addword(List *list) 
{
    List lst = *list;
    if (curbuf > (sizebuf - 1)) 
    {
        char * temp = realloc(buf, sizebuf += SIZE);
        if (temp != NULL) buf = temp;
    }
    buf[curbuf++] = '\0';
    buf = realloc(buf, sizebuf = curbuf);   // выравниваем память точно по размеру слова
    if (curlist > (sizelist - 1))
        lst = realloc(lst, (sizelist += SIZE)*sizeof(*lst));

    lst[curlist++] = buf;
    *list = lst;
}

void printlist(List lst, int sizelist) 
{
    int i;
    if (lst == NULL) 
        return;
    for (i = 0; i < (sizelist - 1); i++)
        printf("%s ", lst[i]);
    printf("\n");
}

int symset(int c) 
{
    return c != '\n' &&
           c != ' ' &&
           c != '\t' &&
           c != '>' &&
           c != EOF &&
           c != '<' &&
           c != '&' &&
           c != '|' &&
           c != '(' &&
           c != ')' &&
           c != ';';
}

static char str[10];
static int ind = 0;

void getsym()  //если вдруг последний \n, то fscanf его не считает, так что считываем отдельно механически
{  
    if (str[ind] == '\0') 
    {
        ind = 0;
        str[0] = '\n';
        fscanf(stdin, "%9[^\n]", str);  //считываем 9 символов либо до \n
        c = getchar();
        if (c != EOF) 
        {
            int i;
            for (i = 0; str[i] != '\0'; i++);   //идем до конца
            str[i] = c;
            str[i+1] = '\0';
        }
        else return;
    }
    c = str[ind++];
}

int build_list(List *lst, int* size, int* isthere) 
{
    typedef enum {Start, Word,  Greater, Greater2, Stop, Vert, Vert2, Smaller, Emp, Emp2, Dop, Not_proc,
    Comment, Quot} vertex;  
    vertex V = Start;
    for (int i = 0; i < 10; str[i] = '\0', i++);
    getsym();   //обработка \n, _возврат текущего символа_ и заполнение буфера str
    null_list(lst);
    nullbuf();
    while (1) 
        switch(V) 
        {
            case Start:
                if (c == ' ' || c == '\t') 
                    getsym();
                else if (c == EOF) 
                {
                    termlist(lst);
                    V = Stop;
                }

                else if (c == '\n') 
                {
                    termlist(lst);
                    V = Stop;
                }

                else if (c == '#') 
                {    //символы меж двух # игнорируются
                    V = Comment;
                    getsym();
                }
                else if (c == '\"')
                {
                    V = Quot;
                    getsym();
                }

                else 
                {  
                    nullbuf();  //обнуляем буфер для накопления текущего слова
                    addsym();
                    switch(c) 
                    {
                        case '(': 
                        case ')': V = Not_proc; break;
                        case '>': V = Greater; break;
                        case '<': V = Smaller; break;
                        case '|': V = Vert; break;
                        case '&': V = Emp; break;
                        case ';': V = Dop; break;
                        default: V = Word; break;
                    }
                    getsym();
                }

        break;

            case Comment:
                if (c == '#') V = Start;
                getsym();
                break;
            case Quot:
                nullbuf();
                while (c != '\"' && c != EOF && c != '\n') 
                {
                    addsym();   //перевыделили память, добавили в буфер для накопления слова символ с
                    getsym();   //считали символ
                }
                V = Start;
                addword(lst);
                getsym();
                break;

            case Word:
                    if (symset(c)) 
                    {
                        addsym();   //перевыделили память, добавили в буфер для накопления слова символ с
                        getsym();   //считали символ
                    }
                    //если символ разделитель
                    else 
                    {
                        V = Start;
                        addword(lst);
                    }
                break;

            case Greater:
                    //если было 2 >>
                    if (c == '>') 
                    {
                        addsym();
                        getsym();
                        V = Greater2;
                    }
                    else 
                    {
                        V = Start;
                        addword(lst);
                    }

                break;

            case Greater2:
                    V = Start;
                    addword(lst);
                    break;

            case Vert:
                if (c == '|') 
                {
                    addsym();
                    getsym();
                    V = Vert2;
                }
                else 
                {
                    V = Start;
                    addword(lst);
                }
                break;

            case Vert2:
                V = Start;
                addword(lst);
                break;

            case Emp:
                if (c == '&') 
                {
                    addsym();
                    getsym();
                    V = Emp2;
                }
                else 
                {
                    V = Start;
                    addword(lst);
                }
                break;

            case Emp2:
                V = Start;
                addword(lst);
                break;

            case Smaller:
                V = Start;
                addword(lst);
                break;

            case Dop:
                V = Start;
                addword(lst);
                break;

            case Not_proc:
                V = Stop;
                *isthere = 1;
                break;

            case Stop:
                *size = sizelist;
                return (c == EOF) ? 1:0;
                break;
    }
}
