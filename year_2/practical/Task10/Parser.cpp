#include "Parser.h"
extern std::vector <Ident> TID;

template <class T, class T_EL>
void from_st ( T & st, T_EL & i ) {
    if (st.top() == LEX_ID) 
        st.pop();
    i = st.top(); 
    st.pop();
}

void  Parser::gl () { //дает лексему
        curr_lex  = scan.get_lex ();
        c_type    = curr_lex.get_type ();
        c_val     = curr_lex.get_value ();
        c_dval = curr_lex.get_dvalue();
        c_sval = curr_lex.get_svalue();
       std::cout << curr_lex;
}

Parser::Parser (const char *program): scan (program) {}

void Parser::analyze () {
    gl (); //считали первую лексему
    P ();//начальный символ грамматики
    if (c_type != LEX_FIN)
        throw curr_lex; //не дошли до конца
    
    int num = 0;
    cout << std::endl;
    for (Lex l : poliz) {//печать полиза
        std::cerr << num << ' ' << l;
        num++;
    }  
    cout << endl << "The End (Successfull)" << endl;
}
 
void Parser::P () {
    if ( c_type == LEX_PROGRAM ) 
        gl ();
    else 
        throw curr_lex;

    if(c_type == LEX_LBR) 
        gl();
    else 
        throw curr_lex;

    D (); 
    
    O (); // Проверка на закрывающую скобку здесь. Если ее не будет, то не выйдем оттуда

    gl(); //ожидается LEX_FIN, проверка в analyze
    
}
 
void Parser::D () {
    while(c_type == LEX_INT || c_type == LEX_REAL || c_type == LEX_STRING) {
        D1(); //описание
        if(c_type == LEX_SEMICOLON) 
            gl();
        else
            throw curr_lex;
    }
}
 
void Parser::D1 () { //описание переменных
    t = c_type; 
    gl(); // читаем LEX_ID
    D2();
    while ( c_type == LEX_COMMA ) { //если , то несколько переменных одного типа
        gl();
        D2();
    }
            
}

void Parser::D2() {
    int sgn = 1;
    if ( c_type != LEX_ID )
        throw curr_lex;
    else {
        st_int.push(c_val);
        dec(t);
        int i = c_val;
        gl ();
        if(c_type == LEX_EQ){ // Если <идентификатор>=<константа>
            gl();
            if(t == LEX_STRING){
                if(c_type != LEX_STR_CONST) 
                    throw curr_lex; 
                TID[i].set_svalue(c_sval);
            } else {
                if(c_type == LEX_PLUS || c_type == LEX_MINUS){ //если указали знак
                    if(c_type == LEX_MINUS) sgn = -1;
                    gl();
                }
                if(c_type != LEX_NUM && c_type != LEX_RNUM)
                    throw curr_lex;
                else {
                    if (t == LEX_INT){
                        TID[i].set_value(c_val * sgn);
                    }else{
                        TID[i].set_dvalue(c_dval * sgn);
                    }
                   
                } 
            }
            TID[i].set_assign();

            
            gl();
        }
        }
}

 
void Parser::O() { 
    while(c_type != LEX_RBR) {
        O1();
    }
}
 
void Parser::O1 () {
    int pl0, pl1, pl2, pl3;
 
    if ( c_type == LEX_IF ) {
        gl ();
        if (c_type != LEX_LPAR)
            throw curr_lex;
        else {
            gl();
            E (); 
            pl2 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            if (c_type == LEX_RPAR) {
                gl();
                eq_bool();
                O1();
                pl3 = poliz.size();
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
                if (c_type == LEX_ELSE) {
                    poliz[pl2] = Lex(POLIZ_LABEL, poliz.size()); //poliz.size - начало else здесь
                    gl();
                    O1();
                    poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
                } else 
                    throw curr_lex;
            } else
                throw curr_lex;
        }
    }//конец if

    else if ( c_type == LEX_WHILE ) {
        pl0 = poliz.size();
        labels_for_con.push(poliz.size()); //запомнили место начала цикла
        gl ();
        if (c_type != LEX_LPAR) 
            throw curr_lex;
        else {
            gl();
            E();
            if (c_type == LEX_RPAR){
                eq_bool();
                pl1 = poliz.size();
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_FGO));
                gl();
                O1();
                poliz.push_back(Lex(POLIZ_LABEL, pl0));
                poliz.push_back(Lex(POLIZ_GO));
                poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
                while (!labels_for_con.empty())
                        labels_for_con.pop();
            } else
                throw curr_lex;
        }
    }//конец while

    else if (c_type == LEX_CONTINUE) {
        if (labels_for_con.empty()) //не в цикле
            throw curr_lex;
        int lab = labels_for_con.top();
        labels_for_con.pop();
        poliz.push_back(Lex(POLIZ_LABEL, lab));
        poliz.push_back(Lex(POLIZ_GO));
        gl();
        if (c_type != LEX_SEMICOLON) 
            throw curr_lex;
        else 
            gl();
    }//конец continue;

    else if (c_type == LEX_FOR) {
        gl();
        if (c_type != LEX_LPAR)
            throw curr_lex;
        else {
            gl();
            if(c_type == LEX_ID || c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_NOT || c_type == LEX_NUM || c_type == LEX_STR_CONST) //проверка на выражение, которого может не быть
                E(); 
            if(c_type != LEX_SEMICOLON)
                throw curr_lex;
            else {
                pl0 = poliz.size(); //запоминаем конец 1го выражения

                gl();
                if(c_type == LEX_ID || c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_NOT || c_type == LEX_NUM || c_type == LEX_STR_CONST) //проверка на выражение, которого может не быть
                    E();
            }
            if(c_type != LEX_SEMICOLON)
                throw curr_lex;
            else {
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_FGO));
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
                pl1 = poliz.size();
                labels_for_con.push(poliz.size());

                gl();

                if(c_type == LEX_ID || c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_NOT || c_type == LEX_NUM || c_type == LEX_STR_CONST) //проверка на выражение, которого может не быть
                    E();
            }

            if(c_type != LEX_RPAR)
                throw curr_lex;
            else {
                poliz.push_back(Lex(POLIZ_LABEL, pl0));
                poliz.push_back(Lex(POLIZ_GO));
                poliz[pl1 - 2] = Lex(POLIZ_LABEL, poliz.size());
                gl();
                O1();
                poliz.push_back(Lex(POLIZ_LABEL, pl1));
                poliz.push_back(Lex(POLIZ_GO));
                poliz[pl1 - 4] = Lex(POLIZ_LABEL, poliz.size());
                while (!labels_for_con.empty())
                    labels_for_con.pop();
            }
        }
    } //конец for

    else if ( c_type == LEX_READ ) {
        gl ();
        if ( c_type != LEX_LPAR )
            throw curr_lex;
        else {
            gl ();

            if (c_type == LEX_ID) {
                check_id_in_read ();
                poliz.push_back ( Lex( POLIZ_ADDRESS, c_val) );
                gl();
            } else
                throw curr_lex;

            if (c_type == LEX_RPAR) {
                gl ();
                poliz.push_back (Lex (LEX_READ));
            } else
                throw curr_lex;

            if(c_type != LEX_SEMICOLON)
                throw curr_lex;
            else 
                gl();
        }
    }//конец read

    else if (c_type == LEX_WRITE) {
        gl ();
        if (c_type == LEX_LPAR) {
            do {
                gl();
                E();
            } while (c_type == LEX_COMMA);

            if ( c_type == LEX_RPAR ) {
                gl ();
                poliz.push_back (Lex(LEX_WRITE));
            } else
                throw curr_lex;
            
            if(c_type != LEX_SEMICOLON)
                throw curr_lex;
            else
                gl();   
        }
        else
            throw curr_lex;
    }//конец write

    else if (c_type == LEX_ID || c_type == LEX_NUM  || c_type == LEX_STR_CONST) { 
        E();
        if(c_type != LEX_SEMICOLON) 
            throw curr_lex;
        else {
            gl();
            poliz.push_back(Lex(LEX_SEMICOLON));
        }
    }// Оператор - выражение

    else if(c_type == LEX_LBR) { 
        gl();
        O();
        if(c_type != LEX_RBR) 
            throw curr_lex;
        else gl();
    } //составной оператор
    else if(c_type == LEX_SEMICOLON) //Пустой оператор
        gl();
    else
        throw curr_lex;
}
 
void Parser::E () {
    E_OR ();
    while (c_type == LEX_EQ) {
        int pl1 = poliz.size() - 1;
        if(st_lex.top() == LEX_ID) //если был идентификатор слева от =, то нужно сделать адрес
            poliz[pl1] = Lex(POLIZ_ADDRESS, poliz[pl1].get_value());
        gl (); 
        E (); 
        eq_type();
        poliz.push_back(Lex(LEX_EQ));
    }
}
 
void Parser::E_OR () {
    E_AND ();
    while ( c_type == LEX_OR) {
        st_lex.push(c_type);
        gl ();
        E_AND ();
        check_op();
    }
}
 
void Parser::E_AND () {
    E_R ();
    while ( c_type == LEX_AND) {
        st_lex.push(c_type);
        gl ();
        E_R ();
        check_op();
    }
}

void Parser::E_R () {
    E_AS ();
    while (c_type == LEX_GTS || c_type == LEX_GTSEQ || c_type == LEX_LTS || c_type == LEX_LTSEQ || c_type == LEX_NEQ || c_type == LEX_DEQ) {
        st_lex.push(c_type);
        gl ();
        E_AS ();
        check_op();
    }
}
void Parser::E_AS () {
    E_MD ();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS) {
        st_lex.push(c_type);
        gl ();
        E_MD ();
        check_op();
    }
}

void Parser::E_MD () {
    F ();
    while (c_type == LEX_STAR || c_type == LEX_SLASH) {
        st_lex.push(c_type);
        gl ();
        F ();
        check_op();
    }
}
 
void Parser::F () {
    if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(LEX_ID, c_val));
        gl ();
    } 
    else if ( c_type == LEX_NUM ) {
        st_lex.push(LEX_INT);         
        poliz.push_back(curr_lex);
        gl ();
    } 
    else if ( c_type == LEX_RNUM) {
        st_lex.push(LEX_REAL);         
        poliz.push_back(curr_lex);
        gl ();
    }
    else if (c_type == LEX_STR_CONST) {
        st_lex.push(LEX_STRING);           
        poliz.push_back(curr_lex);
        gl();
    }
    else if ( c_type == LEX_NOT ) {
        gl (); 
        F (); 
        check_not();
    }
    else if(c_type == LEX_PLUS  || c_type == LEX_MINUS) {
        Lex l = c_type;
        gl();
        F();
        check_unar(l);
    }
    else if (c_type == LEX_LPAR) {
        gl (); 
        E ();
        if (c_type == LEX_RPAR)
            gl ();
        else 
            throw curr_lex;
    } else 
        throw curr_lex;
}

//Семантический анализ

void Parser::dec ( type_of_lex type ) { //отметить, что переменная объявляется
    int i;
    while ( !st_int.empty () ) {
        from_st ( st_int, i ); 
        if ( TID[i].get_declare () ) {
            std::cout << TID[i].get_name() << " ";
            throw "twice declare";
        }
        else {
            TID[i].set_declare ();
            TID[i].set_type (type);
        }
    }
}
 
void Parser::check_id () { //проверка, была ли объявлена используемая переменная
    if ( TID[c_val].get_declare() ){
        st_lex.push ( TID[c_val].get_type () );
        st_lex.push(LEX_ID);
    } else {
        std::cout << TID[c_val].get_name() << " ";
        throw "not declared";
    }
}
 
void Parser::check_op () { //проверка корректности операндов бинарной операции
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_INT;
 
    from_st ( st_lex, t2 );
    from_st ( st_lex, op );
    from_st ( st_lex, t1 );

    if (t1 == LEX_STRING) {
        if(t2 != LEX_STRING && t2 != LEX_STR_CONST) 
            throw "only string can do smth with string";
        if(op == LEX_PLUS) 
            r = LEX_STRING;
        else if(op == LEX_GTS ||  op == LEX_LTS || op == LEX_DEQ || op == LEX_NEQ) 
            r = LEX_INT;
        else 
            throw "wrong operator with strings";
    }
    else if (op == LEX_OR || op == LEX_AND) {
        if(t1 == LEX_INT && t2 == LEX_INT)
            r = LEX_INT;
        else 
            throw "wrong operand types";
    }
    else if ((t1 == LEX_INT && t2 == LEX_REAL) || (t1 == LEX_INT && t2 == LEX_REAL) || (t1 == LEX_INT && t2 == LEX_INT) || (t1 == LEX_REAL && t2 == LEX_REAL )){
        if(op == LEX_PLUS || op == LEX_MINUS || op == LEX_STAR || op == LEX_SLASH){
            if(t1 == LEX_REAL || t2 == LEX_REAL)
                r = LEX_REAL;
            else 
                r = LEX_INT;
        }
        else if(op == LEX_GTS ||  op == LEX_LTS || op == LEX_DEQ || op == LEX_NEQ || op == LEX_GTSEQ || op == LEX_LTSEQ){
            r = LEX_INT;
        }
        else throw "wrong operators";
    }
    st_lex.push(r); //положили в семантический стек результат
    poliz.push_back(Lex(op));
}

void Parser::check_not () {
    if(st_lex.top() == LEX_ID){ //убираем LEX_ID 
        st_lex.pop();
        if (st_lex.top() != LEX_INT)
            throw "wrong type is in not";
        else  
            poliz.push_back (Lex (LEX_NOT));
        st_lex.push(LEX_ID);
    }
    else if (st_lex.top() != LEX_INT)
        throw "wrong type is in not";
    else  
        poliz.push_back (Lex(LEX_NOT));
}

void Parser::check_unar (Lex l) {
    if(st_lex.top() == LEX_ID){
        st_lex.pop();
        if (st_lex.top() != LEX_INT && st_lex.top() != LEX_REAL)
            throw "wrong type is in unar op";
        else  
            poliz.push_back (l);
        st_lex.push(LEX_ID);
    }
    else if (st_lex.top() != LEX_INT)
        throw "wrong type is in unar op";
    else  
        poliz.push_back (l);
}
 
void Parser::eq_type() {
    type_of_lex t;
    from_st(st_lex, t); 
    if (st_lex.top() != LEX_ID) 
        throw "Wrong assignment";
    st_lex.pop();
    if (t != st_lex.top()){
        if((t == LEX_INT && st_lex.top() == LEX_REAL)||(st_lex.top() == LEX_INT && t == LEX_REAL))
            return;
        else 
            throw "wrong types are in =";
    }
    
}
 
void Parser::eq_bool () {
    if ( st_lex.top () != LEX_INT )
        throw "boolean expression is not correct";
    st_lex.pop ();
  }
 
void Parser::check_id_in_read () {
    if ( !TID [c_val].get_declare() )
        throw "not declared";
}
 