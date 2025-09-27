#include "Lex.h"
#include "Scanner.h"
#include <iostream>


Lex::Lex(type_of_lex t, int v, double d, const std::string s) {
    t_lex = t;
    v_lex = v;
    d_lex = d;
    s_lex = s;
}
type_of_lex Lex::get_type() const {return t_lex;}
int Lex::get_value() const {return v_lex;}
double Lex::get_dvalue() const{return d_lex;}
std::string Lex::get_svalue() const {return s_lex;}
void Lex::set_type(type_of_lex t) {t_lex = t;}
void Lex::set_value(int v) {v_lex = v;}
void Lex::set_dvalue(double d) {d_lex = d;}
void Lex::set_svalue(std::string s) {s_lex = s;}
extern std::vector <Ident> TID;
std::ostream & operator<< ( std::ostream &s, Lex l ) {
    std::string t;
    if ( l.get_type() <= LEX_NOT )
        t = Scanner::TW[l.get_type()];
    else if ( l.get_type() >= LEX_FIN && l.get_type() <= LEX_RBR )
        t = Scanner::TD[ l.get_type() - LEX_FIN ];
    else if ( l.get_type() == LEX_NUM ){
        t = "NUMB";
        if(l.get_dvalue() != 0) s << '(' << t << ',' << l.get_dvalue() << ");" << std::endl;
        else s << '(' << t << ',' << l.get_value() << ");" << std::endl;
        return s;
    }
    else if(l.get_type() == LEX_RNUM){
        t = "RNUMB";
        s << '(' << t << ',' << l.get_dvalue() << ");" << std::endl;
        return s;
    }
    else if ( l.get_type() == LEX_ID )
        t = TID[l.get_value()].get_name();
    else if ( l.get_type() == LEX_STR_CONST){
        t = "STRING CONST";
        s << '(' << t << ',' << l.get_svalue() << ");" << std::endl;
        return s;
    }else if ( l.get_type() == POLIZ_LABEL )
        t = "LABEL";
    else if ( l.get_type() == POLIZ_ADDRESS )
        t = "&";
    else if ( l.get_type() == POLIZ_GO )
        t = "!";
    else if ( l.get_type() == POLIZ_FGO ) 
        t = "!F";
    else
        throw l;
    s << '(' << t << ',' << l.get_value() << ");" << std::endl;
    return s;
}