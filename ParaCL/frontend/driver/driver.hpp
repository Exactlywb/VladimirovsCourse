#ifndef DRIVER_H__
#define DRIVER_H__

#include <FlexLexer.h>

#include <memory>
#include <string>
#include <vector>
#include <fstream>

#include "compiler.hpp"
#include "ast.hpp"
#include "grammar.tab.hh"
#include "interpreter.hpp"
#include "nAryTree.hpp"

namespace yy {

    class FrontendDriver {
        std::unique_ptr<FlexLexer> lexer_;
        Tree::NAryTree<AST::Node*> tree_;

        std::vector<std::string> error_;
        std::vector<std::string> code_;

    public:
        FrontendDriver (const char* input): 
                lexer_ (std::unique_ptr<yyFlexLexer>{new yyFlexLexer}),
                tree_ () {
            
            std::fstream inputFile (input, std::ios_base::in);
            while (inputFile) {
                
                std::string newLine;
                std::getline (inputFile, newLine);
                code_.push_back (newLine);

            }

        }

        parser::token_type yylex (parser::semantic_type *yylval)
        {
            parser::token_type tokenT = static_cast<parser::token_type> (lexer_->yylex ());

            switch (tokenT) {
                //!TODO try catch
                case yy::parser::token_type::NUMBER: {

                    yylval->build<int> () = std::stoi (lexer_->YYText ());
                    break;
                }
                case yy::parser::token_type::ID: {
                    yylval->build<std::string> () = lexer_->YYText ();
                    break;
                }
                case yy::parser::token_type::LEXERR: {
                    throw std::runtime_error ("Unexpected word");
                }
            }

            return tokenT;
        }

        int getLineNo() const noexcept {return lexer_->lineno();}

        std::string getYYText() const noexcept {return lexer_->YYText();}

        void pushError (const std::string& err)  {

            error_.push_back(std::string("#") + std::to_string(getLineNo ()) + std::string(": ")
                            + err + std::string (" ") + code_ [getLineNo () - 1]);

        }

        void pushErrorWithoutYYText (const std::string& err) {

            error_.push_back(std::string("#") + std::to_string(getLineNo ()) + std::string(": ")
                            + err);

        }

        std::vector<std::string> getErrVec () const {

            return error_;
        } 

        bool parse ()
        {
            parser parser (this);
            bool res = parser.parse ();
            return !res;
        }

        void setRoot (AST::Node *node)
        {
            tree_.setRoot (node);
        }

        void callDump (std::ostream &out)
        {
            tree_.dump (out);
        }

        void interpret ()
        {  //!TODO does it have to be in driver?

            interpret::Interpreter interpret (&tree_);

            try {
                interpret.run ();
            }
            catch (const std::runtime_error &err) {
                std::cout << "Runtime error: " << err.what () << std::endl;
            }
        }

        void compile () 
        {

            ParaCompiler::ParaCLLVMCompiler compiler (&tree_);
            
            try {
                compiler.run ();
            }
            catch (const std::runtime_error &err) {
                std::cout << "Runtime error: " << err.what ();
            }

        }

    };

}  // namespace yy

#endif
