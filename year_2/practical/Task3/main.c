#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#define SIZE 16
int c; /*текущий символ */
char * buf; /* буфер для накопления текущего слова*/
int sizebuf; /* размер буфера текущего слова*/
int sizelist; /* размер списка слов*/
int curbuf; /* индекс текущего символа в буфере*/
int curlist; /* индекс текущего слова в списке*/
char str[SIZE]; /* очередные N символов строки */
int curstr = 0; /* индекс очередного символа в массиве */

void nullbuf() 
{
	buf = NULL;
	sizebuf = 0;
	curbuf = 0;
}

void addsym() 
{
	if (curbuf > sizebuf-1)
		buf = (char *) realloc(buf, sizebuf += SIZE); 
	buf[curbuf++] = c;
}

char getsym()
{
	if (str[curstr] == '\0') 
	{
		int i;
		char c;
		curstr = 0;
		for (i = 0; i < SIZE; str[i] = '\0',i++);
		fscanf(stdin, "%14[^\n]", str);
		c = getchar();
		for (i = 0; str[i] != '\0'; i++);
		str[i] = c;
	}
	return str[curstr++];
}


int symset(int c)
{
	return c != '\n' &&
		   c != ' '  &&
		   c != '\t' &&
		   c != '>'  &&
		   c != '<'  &&
		   c != '|'  &&
		   c != '&'  &&
		   c != ';'  &&
		   c != ','  &&
		   c != EOF;
} 

int main() 
{
	typedef enum {Start, Word, Greater, Or, And, GreaterOrAnd2, Single,
	Newline, Stop} vertex;
 	vertex V = Start;
 	for (int i = 0; i < SIZE; str[i] = '\0', i++);
	c = getsym();
	null_list();
	while (1) 
		switch(V) 
		{
			case Start:
				if (c == ' ' || c == '\t') 
					c = getsym();
				else if (c == EOF) 
				{
					V = Stop;
					if (buf != NULL) 
					{
						printf("\n");
						printf("Длина списка: %d\n", curlist);
						termlist();
						printf("\n");
						printf("Список:\n");
						printlist();
						sort();
						printf("Отсортированный список:\n");
						printlist();
						printf("\n");
						printf("Преобразованный список:\n");
						for (int i = 1; i < sizelist - 1; i += 2)
							printf("%s\n",lst[i]);
						nullbuf();
						clearlist();
					}
				}
				else if (c == '\n') 
				{
					printf("\n");
					printf("Длина списка: %d\n", curlist);
					termlist();
					printf("\n");
					printf("Список:\n");
					printlist();
					sort();
					printf("Отсортированный список:\n");
					printlist();
					printf("\n");
					V = Newline;
					printf("Преобразованный список:\n");
					for (int i = 1; i < sizelist - 1; i += 2)
						printf("%s\n",lst[i]);
					nullbuf();
					c = getsym();
				}
				else 
				{
					nullbuf();
					addsym();
					if (c == '>')
						V = Greater;
					else if (c == '|')
						V = Or;
					else if (c == '&')
						V = And;
					else if (c == ',' || c == '<' || c == ';')
						V = Single;
					else
						V = Word;		
					c = getsym();
				}
				break;
			case Word:
				if (symset(c)) 
				{
					addsym();
					c = getsym();
				}
				else 
				{
					V = Start;
					addword();
				}
				break;
			case Greater:
				if (c == '>') 
				{
					addsym();
					c = getsym();
					V = GreaterOrAnd2;
				}
				else 
				{
					V = Start;
					addword();
				}
				break;
			case Or:
				if (c == '|') 
				{
					addsym();
					c = getsym();
					V = GreaterOrAnd2;
				}
				else 
				{
					V = Start;
					addword();
				}
				break;
			case And:
				if (c == '&') 
				{
					addsym();
					c = getsym();
					V = GreaterOrAnd2;
				}
				else 
				{
					V = Start;
					addword();
				}
				break;
			case GreaterOrAnd2:
				V = Start;
				addword();
				break;	
			case Single:
				V = Start;
				addword();
				break;	
			case Newline:
				clearlist();
				V = Start;
				break;
			case Stop:
				exit(0);
				break;
			}
}
