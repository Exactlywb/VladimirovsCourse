#ifndef DRIVER_H__
#define DRIVER_H__

#include "grammar.tab.hh"
#include <FlexLexer.h>
#include <string>
#include "ast.hpp"
#include "interpreter.hpp"

namespace yy {

class FrontendDriver {

    FlexLexer *lexer_;
    AST::Tree tree_;

public:
    FrontendDriver (FlexLexer *lexer): lexer_ (lexer), tree_ () {}

    parser::token_type yylex (parser::semantic_type *yylval) {

        parser::token_type tokenT = static_cast<parser::token_type>(lexer_->yylex ());
        
        switch (tokenT) {
            //!TODO try catch
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

    void interpret () { //!TODO does it have to be in driver?

        interpret::Interpreter interpret (&tree_);
        
        try {
       
            interpret.run ();

        } catch (const std::runtime_error& err) {

            std::cout << "Runtime error: " << err.what () << std::endl;
            throw err;

        }

    }

};

}

#endif
