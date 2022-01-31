#ifndef CUSTOM_LEXER_H__
#define CUSTOM_LEXER_H__

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "location.hh"

class ParaCLexer final : public yyFlexLexer {
    yy::location location_{};

public:
    ParaCLexer () = default;

    void setLocation ();

    yy::location getLocation () const { return location_; }

    int yylex () override;
};

#endif
