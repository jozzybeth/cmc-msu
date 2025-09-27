#ifndef IDENT_H
#define IDENT_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "Scanner.h"

class Ident {
    std::string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;
    double dvalue;
    std::string svalue;
public:
    Ident();
    Ident(const std::string);
    bool operator == (const std::string&) const;
    std::string get_name() const;
    bool get_declare() const;
    void set_declare();
    type_of_lex get_type() const;
    void set_type(type_of_lex t);
    bool get_assign() const;
    void set_assign();
    int get_value() const;
    void set_value(const int);
    double get_dvalue() const;
    void set_dvalue(const double);
    std::string get_svalue() const;
    void set_svalue(const std::string);

};

int put(const std::string&);


#endif