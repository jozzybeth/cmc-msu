#include <stdio.h>
#include <stdlib.h>


typedef  struct Node *link; /*  pointer to list node */
typedef  char elemtype[50]; /*  list element type */
typedef struct Node {                    /* list node type */
                        elemtype elem;  /* element type */
                        link next;      /* pointer to the next node */
                    } node;


typedef link list; /* list type (pointer to the first node) */

int equal (char *str1,char *str2) {
    int i = 0;
    for (i = 0; str1[i] == str2[i]; i++)
        if (str1[i] == '\0')
            return 1;
    return 0;    
}     


list del(list p) {
    list q = p, s;
    int maxlen = 0,i = 0;
    while (q != NULL) {
        while (q->elem[i] != '\0') 
            i++;
        if (i > maxlen)
            maxlen = i;
        q = q->next;
        i = 0;  
    }
    q = p;
    s = p;
    while (q != NULL) {
        while (q->elem[i] != '\0') 
            i++; 
        if (i == maxlen) {
            if (q == p) {
                q = q->next;
                free(p);
                p = q;
                s = q;
            }
            else {
                s->next = q->next;
                free(q);
                q = s->next;    
            }    
        }
        else {
            s = q; 
            q = q->next; 
        }    
        i = 0;      
    } 
    return p; 
} 

int main() {
    list lst = NULL;
    link q, p, r;
    int i;
    char str[50];
    while (scanf("%s",str) != EOF) {
        p = malloc(sizeof(node));
        i = 0;
        while (str[i] != '\0') {
            p->elem[i] = str[i];
            i++;
        }
        p->elem[i] = '\0';
        p->next = NULL;
        if (lst == NULL) {
            lst = p;
            q = p;
        } else {
            q->next = p;
            q = q->next;
        }
    }          
    p = lst;
    r = lst;
    while (p != q) {
        if (equal(p->elem,q->elem)) {
            if (p == lst) {
                p = p->next;
                free(lst);
                lst = p;
                r = p;
            } else {
                r->next = p->next;
                free(p);
                p = r->next;
            }
        } else {
            r = p;
            p = p->next;
        }
    }
    lst = del(lst);
    q = lst;
    while (q != NULL) {
        printf("%s ",q->elem);
        q = q->next;  
        free(lst);
        lst = q;  
    }
    printf("\n");

}

