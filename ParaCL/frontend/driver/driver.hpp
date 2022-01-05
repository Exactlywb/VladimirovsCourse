#ifndef DRIVER_H__
#define DRIVER_H__

#include "grammar.tab.hh"
#include <FlexLexer.h>
#include <string>
#include "ast.hpp"

namespace yy {

class FrontendDriver {

    FlexLexer *lexer_;
    AST::Tree tree_;

public:
    FrontendDriver (FlexLexer *lexer): lexer_ (lexer), tree_ () {}

    parser::token_type yylex (parser::semantic_type *yylval) {

        parser::token_type tokenT = static_cast<parser::token_type>(lexer_->yylex ());
        if (tokenT == yy::parser::token_type::NUMBER)
            yylval->as<int>() = std::stoi (lexer_->YYText ());

        switch (tokenT) {

            case yy::parser::token_type::NUMBER: {
                
                yylval->build<int> () = std::stoi (lexer_->YYText ());
                break;

            }
            case yy::parser::token_type::ID: {

                yylval->build<std::string> () = lexer_->YYText ();

            }

        }

        return tokenT;

    }

    bool parse () {

        parser parser (this);
        bool res = parser.parse ();
        return !res;

    }

    void setRoot (AST::Node* node) {
        tree_.setRoot (node);
    }
    
    void callDump (std::ostream& out) {
        tree_.dump (out);
    }

};

}

#endif
