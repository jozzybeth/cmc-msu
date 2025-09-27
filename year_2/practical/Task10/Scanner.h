#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <string>
#include "Lex.h"
#include "Ident.h"

class Scanner {
    FILE * f;
    char c;
    int look(const std::string& , const char**);
    void gc();
public:
    static const char* TW[];
    static const char* TD[];
    Scanner(const char *);
    Lex get_lex();
    ~Scanner();
};

#endif