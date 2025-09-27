#ifndef TREE_H
#define TREE_H

#include "list.h"

typedef struct tree 
{
	int argc;				// количество аргументов
    char **argv;			// список из имени команды и аргументов
    char *infile;			// переназначенный файл стандартного ввода
    char *outfile;			// переназначенный файл стандартного вывода
    int append;				// = 1, то записывать в конец файла
    int backgrnd;			// = 1, если команда подлежит выполнению в фоновом режиме
    int tnext;				// = 1, если ; = 2, если && = 3, если || 
    struct tree *pipe;		// следующая команда после “|”
    struct tree *next;		// следующая после “;” , “&”, “&&”, “||”
} tree;


tree *simple_command (List );
tree *conveyor (tree*, List );
tree *list_of_com (tree*, List);
tree *make_cmd_null();
void add_arg (List , tree*);
void make_bgrnd(tree*);
char **conv_or_not(List);
tree *create_tree (List);


void clear_tree(tree **);
void make_shift(int );
void print_argv(char **, int);
void print_tree (tree *, int);
#endif
