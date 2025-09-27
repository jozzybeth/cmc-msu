#ifndef LEX_H
#define LEX_H

#include <string>
enum type_of_lex {
    //TW
    LEX_NULL,
    LEX_PROGRAM,
    LEX_INT,
    LEX_STRING,
    LEX_REAL,
    LEX_READ,
    LEX_WRITE,
    LEX_IF,
    LEX_ELSE,
    LEX_WHILE,
    LEX_FOR,
    LEX_CONTINUE,
    LEX_AND, 
    LEX_OR,
    LEX_NOT,
    //TD
    LEX_FIN,
    
    LEX_EQ,
    LEX_DEQ, //double =
    LEX_NEQ,
    LEX_STAR,
    LEX_SLASH,
    LEX_PLUS,
    LEX_MINUS,
    LEX_GTS, //greater than sign
    LEX_GTSEQ, // >=
    LEX_LTS,
    LEX_LTSEQ,
    LEX_SEMICOLON, 
    LEX_COMMA,
    LEX_DOT,
    LEX_LPAR, //(
    LEX_RPAR,
    LEX_LBR, //{ (left bracket)
    LEX_RBR,
    //остальное
    LEX_NUM,
    LEX_RNUM,
    LEX_ID,
    LEX_STR_CONST,
    POLIZ_LABEL,
    POLIZ_ADDRESS,
    POLIZ_GO,
    POLIZ_FGO,
    POLIZ_WRITE
};

class Lex {
    type_of_lex t_lex;
    int v_lex;
    double d_lex;
    std::string s_lex;
public:
    Lex(type_of_lex t = LEX_NULL, int v = 0, double d = 0, const std::string s = " ");
    type_of_lex get_type() const;
    int get_value() const;
    double get_dvalue() const;
    std::string get_svalue() const;
    void set_type(type_of_lex);
    void set_value(int);
    void set_dvalue(double);
    void set_svalue(std::string);
};
std::ostream & operator<< ( std::ostream &, Lex);

#endif