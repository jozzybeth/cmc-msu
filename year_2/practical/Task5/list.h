#ifndef LIST_H
#define LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
typedef char ** List;

void printlist(List lst, int sizelist);
void clearlist(List *lst);
void changelist(List lst, int size);
int build_list(List *lst, int *size, int *isthere);
int symset(int c);

#endif
