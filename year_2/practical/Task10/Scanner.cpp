#include "Scanner.h"

int Scanner::look(const std::string& s, const char** list) {
    int i = 0;
    while (list[i]) {
        if (s == list[i]) 
            return i;
        i++;
    }
    return 0;
}

//таблица служебных слов
const char * Scanner::TW [] = { "", "program", "int", "string", "real", "read", "write", "if", "else", "while", "for", "continue", "and", "or", "not", NULL};

//таблица ограничителей
const char * Scanner::TD [] = { "@", "=", "==", "!=", "*", "/", "+", "-", ">", ">=", "<", "<=", ";", ",", ".", "(", ")", "{", "}", NULL };

void Scanner::gc() {c = fgetc(f);}

Scanner::Scanner(const char * program) {
    if (!(f = fopen(program, "r")))
        throw "Error open file";
}

Scanner::~Scanner() {fclose(f);}

Lex Scanner::get_lex() {
        enum state { H, IDENT, NUMB, RNUMB, COM, COM2, ALE, NEQ, SLASH, QUOTE };
        state CS = H;
        int d, j, st = 10;
        double r; //дробная часть
        std::string buf;
        do {
            gc (); //получили символ
            switch (CS) {
                case H:
                    if (feof(f)) {
                        return Lex(LEX_FIN); //файл закончился
                    }
                    //пропуск пробельных литер
                    if ( c == ' ' || c == '\n' || c== '\r' || c == '\t' ); 
                    else if (isalpha (c)) { //начало идентификатора
                        d = 1;
                        buf.push_back (c);
                        CS  = IDENT;
                    }
                    else if (isdigit (c)) { //начало числа
                        d   = c - '0';
                        CS  = NUMB;
                    } 
                    else if (c == '!') { //начало !=
                        buf.push_back (c);
                        CS  = NEQ;
                    }
                    else if (c == '=' || c == '<' || c == '>') {  //начало операции сравнения
                        buf.push_back (c);
                        CS  = ALE; 
                    }
                    else if (c == '/') { 
                        buf.push_back (c);
                        CS  = SLASH;
                    }
                    else if (c == '"'){
                        CS = QUOTE;
                    }
                    else if (c == '@') //конец программы
                        return Lex (LEX_FIN);

                    else { //ограничитель
                        buf.push_back (c);
                        if ((j = look (buf, TD))){ //разрешенный ограничитель
                            return Lex ((type_of_lex)(j + (int) LEX_FIN), j); //тип ограничителя и его номер 
                        }
                        else
                            throw c; //некорректный символ
                    }
                    break;
                case IDENT:
                    if ( isalpha (c) || isdigit (c) ) {
                        d++;
                        if (d == 80)
                            throw "The ID must be less than 80 characters long";
                        buf.push_back (c); 
                    }
                    else {
                        ungetc (c, f); //кладем символ обратно в файл
                        if ((j = look (buf, TW))) //служебное слово
                            return Lex ((type_of_lex) j, j);
                        else { //идентификатор
                            j = put(buf);
                            return Lex (LEX_ID, j);
                        }
                    }
                    break;
                case NUMB:
                    if (isdigit (c)) { //целая часть
                        d = d * 10 + ( c - '0' );
                    }else if(c == '.'){ //начинается дробная
                        r = d;
                        CS = RNUMB;
                    }
                    else {
                        ungetc (c, f);
                        return Lex(LEX_NUM, d);
                    }
                    break;
                case RNUMB: //дробная часть
                    if (isdigit (c)) {
                        r = r + (double)(c - '0') / st;
                        st *= 10;
                    }
                    else {
                        ungetc (c, f);
                        return Lex(LEX_RNUM, 0, r);
                    }
                    break;
                case SLASH:
                    if (c == '*') { //комментарий
                        buf.pop_back(); // убрали /
                        CS = COM;
                    } else if(feof(f)){
                        throw '@';
                    }else {
                        ungetc (c, f);
                        j = look(buf, TD);
                        return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                    }
                    break;
                case COM:
                    if (c == '*' ) { //комментарий возможно закончится
                        CS  = COM2;
                    }
                    else if (feof(f))
                        throw '@';
                    break;
                case COM2:
                    if (c == '/'){ //комментарий закончился
                        CS = H;
                    }else if(feof(f)){
                        throw '@';
                    }else if(c != '*'){ // если *, то ничего не делаем, всё ещё в этом состоянии
                        CS = COM;
                    }
                    break;
                case ALE:
                    if ( c == '=' ) { //<= == >=
                        buf.push_back(c);
                        j = look(buf, TD);
                        return Lex((type_of_lex) ( j + (int) LEX_FIN ), j);
                    }
                    else { //< = >
                        ungetc (c, f);
                        j = look (buf, TD);
                        return Lex((type_of_lex) ( j + (int) LEX_FIN ), j);
                    }
                    break;
                case NEQ:
                    if ( c == '=' ) { //!=
                        buf.push_back(c);
                        j = look ( buf, TD );
                        return Lex (LEX_NEQ, j);
                    }
                    else
                        throw '!';
                    break;
                case QUOTE: 
                    if (c == '"')  //конец строки
                        return Lex(LEX_STR_CONST, 0, 0, buf);
                    else if (feof(f)) {
                        throw '@';
                    }
                    buf.push_back(c);
                    break;
        }
  } while (true);
}