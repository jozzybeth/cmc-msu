## Task10: Интерпретатор модельного языка программирования      
### Горина Елизавета, 212 группа    
  

1. __Вариант задания__  
    Реализуются if-else, циклы for и while, continue, read, write.  
    Логические операции: and, or not.  
    Типы данных: int, string, real.  
    Добавлены операции: унарные + и -.   

2. __Грамматика реализуемого языка__  
    {} - повторение конструкций.  
    [] - фигурные скобки - терминалы.  

    P -> program [D O @  
    D -> {int D1;| real D1; | string D1;} | eps  
    D1 -> D2 {,D2}  
    D2 -> id = "str_const" | id = +num | id = -num | id  
    O -> O1 O| ]  
    O1 -> if (E) O1 else O1 | while (E) O1 | continue; |  
          for (id E| + E| - E| not E| num E| str_const E ;    
               id E| + E| - E| not E| num E| str_const E ;  
               id E| + E| - E| not E| num E| str_const E) O1 |  
          read(id); | write(E {,E}); | id E; | num E; | str_const E; | ~{ O | ;   
    E -> E_OR { = E}  
    E_OR -> E_AND { or E_AND}  
    E_AND -> E_R { and E_R}  
    E_R -> E_AS {> E_AS | >= E_AS | < E_AS | <= E_AS | != E_AS | == E_AS}  
    E_AS -> E_MD {+ E_MD | - E_MD}  
    E_MD -> F {* F | / F}  
    F -> id | num | str_const | not F | + F | - F | (E)    
 
3. __Процесс сборки__    
    В Makefile цель по умолчанию - компиляция итогового файла inter.    
  
4.  __Процесс запуска__  
    Если при запуске файла указать название файла, то обрабатываться будет он, иначе файлом по умолчанию является a.txt.    
  
4. __Содержимое файлов__   
    main.cpp: создание объекта типа Interpretator, запуск interpretation, обработка ошибок.      
    Ident.cpp, Ident.h: класс для идентификаторов.   
    Lex.cpp, Lex.h: класс для лексем.  
    Scanner.cpp, Scanner.h: лексический анализ, содержит основной метод get_lex для выдачи лексемы.   
    Parser.cpp, Parser.h: синтаксический, семантический анализ, перевод в ПОЛИЗ.   
    Executer.cpp, Executer.h: этап выполнения.  

 



