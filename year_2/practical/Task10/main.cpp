#include <iostream>
#include "Ident.h"
#include "Lex.h"
#include "Scanner.h"
#include "Parser.h"
#include "Executer.h"

class Interpretator {
    Parser pars;
    Executer E;
public:
    Interpretator (const char* program): pars (program) {}
    void interpretation ();
};
 
void Interpretator::interpretation () {
    pars.analyze ();
    E.execute ( pars.poliz );
}

int main(int argc, char** argv) {
    const char* file;
    if(argc > 1) 
        file = argv[1];
    else
        file = "a.txt\0";
    try {
        Interpretator I(file);
        I.interpretation ();
    }
    catch(Lex err){
        std::cout << "Unexpected lexeme: " << err << std::endl;
    }
    catch(char c){
        std::cout << "Unexpected char: " << c << std::endl;
    }
    catch(const char * s){
        std::cout << s << std::endl;
    }
};