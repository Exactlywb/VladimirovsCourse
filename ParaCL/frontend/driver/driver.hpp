#ifndef DRIVER_H__
#define DRIVER_H__

#include <FlexLexer.h>

#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "ast.hpp"
#include "compiler.hpp"
#include "customLexer.hpp"
#include "grammar.tab.hh"
#include "interpreter.hpp"
#include "nAryTree.hpp"
#include "semantic.hpp"

namespace yy {

    class FrontendDriver final {
        std::unique_ptr<ParaCLexer> lexer_;
        Tree::NAryTree<AST::Node *> tree_;
        std::unique_ptr<SemanticAnalyzer> analyzer_;

        std::vector<std::string> error_;
        std::vector<std::string> code_;

    public:
        FrontendDriver (const char *input) : lexer_ (std::unique_ptr<ParaCLexer>{new ParaCLexer}),
                                             analyzer_ (std::unique_ptr<SemanticAnalyzer>{new SemanticAnalyzer}),
                                             tree_ ()
        {
            std::fstream inputFile (input, std::ios_base::in);
            while (inputFile) {
                std::string newLine;
                std::getline (inputFile, newLine);
                code_.push_back (newLine);
            }
        }

        parser::token_type yylex (parser::semantic_type *yylval, parser::location_type *location)
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

            *location = lexer_->getLocation ();

            return tokenT;
        }

        void semantic () {
            analyzer_->run (&tree_);
        }

        int getLineNo () const noexcept { return lexer_->lineno (); }

        std::string getYYText () const noexcept { return lexer_->YYText (); }

        void pushError (yy::location curLocation, const std::string &err)
        {
            std::string errPos = std::string ("#") + std::to_string (curLocation.begin.line) + std::string (", ") + std::to_string (curLocation.begin.column) + std::string (": ");
            std::string errMsg = err + std::string (": ");
            std::string codePart = code_[curLocation.begin.line - 1];

            std::string underLine ("\n");
            underLine.insert (1, curLocation.begin.column + errPos.size () + errMsg.size (), '~');
            underLine += std::string ("^");

            error_.push_back (errPos + errMsg + codePart + underLine);
        }

        std::vector<std::string>::const_iterator errBegin () const
        {
            return error_.cbegin ();
        }

        std::vector<std::string>::const_iterator errEnd () const
        {
            return error_.cend ();
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
