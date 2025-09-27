#include "Ident.h"

Ident::Ident (){
    declare = false;
    assign = false;
    value = 0;
    dvalue = 0;
    svalue = "";
}
Ident::Ident (const std::string n){
    name = n;
    declare = false;
    assign = false;
    value = 0;
    dvalue = 0;
    svalue = "";
}
bool Ident::operator == (const std::string& s) const {return s == name;}
std::string Ident::get_name() const {return name;}
bool Ident::get_declare() const {return declare;}
void Ident::set_declare() {declare = true;}
type_of_lex Ident::get_type() const {return type;}
void Ident::set_type(type_of_lex t) {type = t;}
bool Ident::get_assign() const {return assign;}
void Ident::set_assign() {assign = true;}
int Ident::get_value() const {return value;}
void Ident::set_value(const int v) {value = v;}
double Ident::get_dvalue() const {return dvalue;}
void Ident::set_dvalue(double d){dvalue = d;}
std::string Ident::get_svalue() const {return svalue;}
void Ident::set_svalue(std::string s){svalue = s;}



std::vector <Ident> TID;
int put(const std::string& buf) {
    std::vector <Ident> :: iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end()) {
        return k - TID.begin();
    };
    TID.push_back(Ident(buf.c_str()));
    return TID.size() - 1;
};