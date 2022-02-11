#include "customLexer.hpp"

void ParaCLexer::setLocation ()
{
    location_.begin.line = location_.end.line = lineno ();
    int endCol = location_.end.column;

    for (size_t charIdx = 0; yytext[charIdx]; charIdx++) {
        char curChar = yytext[charIdx];
        switch (curChar) {
            case '\n':
            case '\v':
            case '\r':
                location_.begin.column = location_.end.column = 1;
                location_.end.line = lineno ();
                break;
            case '\t':
                location_.begin.column += 8;
                location_.end.column += 8;
                break;
            case ' ':
                location_.begin.column++;
                location_.end.column++;
                break;
            default: location_.begin.column = endCol; location_.end.column = location_.begin.column + YYLeng ();
        }
    }
}
