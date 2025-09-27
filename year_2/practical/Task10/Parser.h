#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <stack>
#include <vector>
#include <math.h>
#include "Scanner.h"
using std::cout;
using std::endl;
using std::stack;
using std::vector;
 
class Parser {
    Lex          curr_lex;
    type_of_lex  c_type;
    type_of_lex  t;
    int          c_val;
    double c_dval;
    std::string c_sval;

    Scanner      scan;
    stack <int> st_int; //стек для номеров идент-ов в TID
    stack <type_of_lex> st_lex; //стек для типов и операций 
    stack<int> labels_for_con;

//синтаксический анализ
    void P();
    void D2();
    void D1();
    void D();
    void O();
    void O1();
    void E();
    void E_OR();
    void E_AND();
    void E_R();
    void E_AS();
    void E_MD();
    void F();

//семантический анализ
    void  dec (type_of_lex);
    void  check_id ();
    void  check_op ();
    void  check_not ();
    void  check_unar (Lex);
    void  eq_type ();
    void  eq_bool ();
    void  check_id_in_read ();

    void gl();

public:
    vector <Lex> poliz;
    Parser (const char *);
    void  analyze(); //синтаксический анализ
};

#endif
 
