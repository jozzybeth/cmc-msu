%{
/* Вычисление протяжения скобочной системы */
#include <stdio.h>
%}
%%
P: S {printf("Протяжение: %d\n", $1);}
 S: '('S')'S {$$ = $4 + 1;}
  | /*eps*/ {$$ = 0;}
  %%

int main(){
  printf("Введите скобочную систему: ");
  yyparse();
}

yylex(){
  int c;
  c = getchar();
  if(c == '\n') 
    return 0;
  yylval = c;
  return c;
}

yyerror(char *s){
  printf("Ошибка!! %s\n", s);
} 

