#include <stdio.h>
#include <stdlib.h>


typedef struct tNode *tree;
typedef int elemtype;  

typedef struct tNode
{
     elemtype elem;
     tree left;
     tree right;
} tnode;

typedef tree bintree;

int str2int (char *str) {
    int i = 0, x = 0;
    while (str[i] != '\0') 
        x = x*10 + str[i++] - '0';
   return x; 
}           

void intotree(bintree *t, int x) {
    if (*t == NULL) {
        *t = malloc(sizeof(tnode));
        (*t)->elem = x;
        (*t)->left = NULL;
        (*t)->right = NULL;
        return;
    }
    if (x > (*t)->elem) 
        intotree(&((*t)->right),x);
    else if (x < (*t)->elem)
        intotree(&((*t)->left),x);
    else return;
}

void findmax(bintree *t, bintree *q) {
    if ((*t)->right != NULL)
        findmax(&((*t)->right),q);
    else {
        (*q)->elem = (*t)->elem;
        *q = *t;
        *t = (*t)->left;
    }
    return;
}

void delintree(bintree *t, int x) {
    bintree q;
    if (*t == NULL)
        return;
    if (x > (*t)->elem)
        delintree(&((*t)->right), x);
    else if (x < (*t)->elem)
        delintree(&((*t)->left), x);
    else {
        q = *t;
        if (((*t)->left) == NULL) 
            *t = (*t)->right;
        else if (((*t)->right) == NULL)
            *t = (*t)->left;
        else
            findmax(&((*t)->left),&q);
        free(q);
    }
}

void intree(bintree t, int x) {
    if (t == NULL) {
        printf("no\n");
        return;
    } 
    if (x > t->elem)
        intree(t->right, x);
    else if (x < t->elem)
        intree(t->left, x);
    else {
        printf("yes\n");
        return;
    }    

}

void freetree(bintree *t) {
    if (*t != NULL) {
        freetree(&((*t)->left));
        freetree(&((*t)->right));
        free(*t);
    }

}

int main() {
    bintree t = NULL;
    char str[50];
    int x;
    while (scanf("%s",str) != EOF) {
        x = str2int(str+1);
        switch (str[0]) {
        case '+':
            intotree(&t,x);
            break;
        case '-':
            delintree(&t,x);
            break;
        case '?':
            intree(t,x);
            break;
        }
    }
    freetree(&t);       
}

