#include <iostream>

#include "driver.hpp"

int     yyFlexLexer::yywrap () {    return 1;   }

int main () {

    FlexLexer *lexer = new yyFlexLexer;
    yy::FrontendDriver driver (lexer);
    driver.parse ();
    delete lexer;

    return 0;

}
