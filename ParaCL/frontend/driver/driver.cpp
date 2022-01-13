#include <iostream>

#include "driver.hpp"
#include "InputHandler.hpp"

int     yyFlexLexer::yywrap () {    return 1;   }

int main (int argc, char** argv) {
    
    InputHandler input;

    try {
        
        input.setParams (argc, argv);

    } catch (const std::runtime_error& err) {

        std::cout << "Input error: " <<  err.what () << std::endl;
        return 0;
    
    }

    std::cin.rdbuf (input.code_.rdbuf ());

    FlexLexer *lexer = new yyFlexLexer;
    yy::FrontendDriver driver (lexer);
    driver.parse ();

    driver.interpret ();

    delete lexer;

    return 0;

}

