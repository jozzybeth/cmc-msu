#include <stdio.h>
#include <stdlib.h>
#define SIZE 16
char ** lst; /* список слов (в виде массива)*/
extern char * buf; /* буфер для накопления текущего слова*/
extern int sizebuf; /* размер буфера текущего слова*/
extern int sizelist; /* размер списка слов*/
extern int curbuf; /* индекс текущего символа в буфере*/
extern int curlist;
void clearlist() 
{
	int i;
 	sizelist = 0;
 	curlist = 0;
	if (lst == NULL) return;
 	for (i = 0; lst[i] != NULL; i++)
 		free(lst[i]);
	free(lst);
	lst = NULL;
}

void null_list() 
{
	sizelist = 0;
	curlist = 0;
	lst = NULL;
}

void termlist() 
{
	if (lst == NULL) 
		return;
	if (curlist > sizelist-1)
		lst = (char **) realloc(lst,(sizelist+1)*sizeof(*lst));
 	lst[curlist] = NULL;
 	lst = (char **) realloc(lst,(sizelist = curlist+1)*sizeof(*lst));
}

void printlist() 
{
	int i, j, count = 0;
	if (lst == NULL) 
		return;
	for (i = 0; i < sizelist-1; i++)
		printf("%s\n",lst[i]);
	printf("\n");
	printf("Каждый четный символ\n");
	for (i = 0; i < sizelist-1; i++) {
		for (j = 0; lst[i][j] != '\0'; j++) 
			if (j % 2 != 0)
				printf("%c",lst[i][j]);
		printf(" ");	
	}
	printf("\n");
	printf("\n");
}

void addword() 
{
	if (curbuf > sizebuf-1)
		buf = (char *) realloc(buf, sizebuf+=1); 
	buf[curbuf++] = '\0';
	buf = (char *) realloc(buf,sizebuf = curbuf);
	if (curlist > sizelist-1)
		lst = (char **) realloc(lst, (sizelist+=SIZE)*sizeof(*lst)); 
	lst[curlist++] = buf;
}

int striden(char* str1, char* str2)
{
	while(*str1 == *str2 && *str1 != '\n' && *str2 != '\n')
 	{
  		str1++;
  		str2++;
 	}	
	if (*str1 == '\n' && *str2 == '\n')
		return 0;
	else if (*str1 == '\n')
		return -1;
	else if (*str2 == '\n')
		return 1;
	else
		return *str1 - *str2;
}

void sort () 
{
	int i,j;
	for (i = 0; i < curlist - 1; i++)
		for (j = 0; j < curlist - i - 1; j++)
			if (striden(lst[j],lst[j+1]) > 0) 
			{
				char *temp = lst[j];
    			lst[j] = lst[j+1];
    			lst[j+1] = temp;
			}
}