#ifndef __EXEC__

#define __EXEC__
#include "list.h"
#include "tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

void handler (tree *);                 // обход дерева

#endif
