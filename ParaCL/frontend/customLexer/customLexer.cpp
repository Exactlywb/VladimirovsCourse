#include "customLexer.hpp"

void ParaCLexer::setLocation () {

    location_.begin.line = location_.end.line = lineno ();
    int endCol = location_.end.column;

    char curChar = yytext [0];
    switch (curChar) {

        case '\n':
        case '\v':
        case '\r':
            location_.begin.column = location_.end.column = 1;
            location_.end.line = lineno ();
            break;
        default:
            location_.begin.column   = endCol;
            location_.end.column     = location_.begin.column + YYLeng ();

    }

    //!TODO think about it.

}
