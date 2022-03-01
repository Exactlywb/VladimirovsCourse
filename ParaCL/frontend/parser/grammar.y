%language "c++"

%skeleton "lalr1.cc"

%defines
%define api.value.type variant
%param {yy::FrontendDriver* driver}

%code requires
{
#include <iostream>
#include <string>
#include "frontend/AST/ast.hpp"


namespace yy {class FrontendDriver;};

}

%code
{
extern int yylineno;
#include "frontend/driver/driver.hpp"
#include <string>

namespace yy {

    parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* location, FrontendDriver *driver);

}

namespace {

    AST::OperNode* makeUnaryOperNode (AST::OperNode::OperType type, AST::Node* child, yy::location loc) {

        AST::OperNode* newOperNode = new AST::OperNode (type, loc);
        newOperNode->addChild (child);
        return newOperNode;
    }

    AST::OperNode* makeBinOperNode (AST::OperNode::OperType type, 
                                    AST::Node* firstChild, AST::Node* secondChild, yy::location loc) {

        AST::OperNode* newOperNode = new AST::OperNode (type, loc);
        newOperNode->addChild (firstChild);
        newOperNode->addChild (secondChild);
        return newOperNode;
    }

    AST::CondNode* makeCondNode (AST::CondNode::ConditionType type, AST::Node* expr, 
                                 AST::Node* body, yy::location loc) {

        AST::CondNode* newCondNode = new AST::CondNode (type, loc);
        newCondNode->addChild (expr);
        newCondNode->addChild (body);
        return newCondNode;
    }

    AST::OperNode* makeAssign (const std::string &firstChild, AST::Node* secondChild, 
                               yy::location assignLoc, yy::location varLoc) {
        AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN, assignLoc);
        newNode->addChild (new AST::VarNode (firstChild, varLoc));
        newNode->addChild (secondChild);
        return newNode;
    }
}

}

%locations

%token <int>                NUMBER
%token <std::string>        ID

%token                      ASSIGN      "="

%token                      ADD         "+"
%token                      SUB         "-"

%token                      UN_ADD      '+'
%token                      UN_SUB      '-'

%token                      MUL         "*"
%token                      DIV         "/"
%token                      MOD         "%"

%token                      EQ          "=="
%token                      NEQ         "!="
%token                      MORE        ">"
%token                      LESS        "<"
%token                      GTE         ">="
%token                      LTE         "<="

%token                      AND         "&&"
%token                      OR          "||"

%token                      IF          "if"

%token                      WHILE       "while"
%token                      PRINT       "print"

%token                      OPCIRCBRACK "("
%token                      CLCIRCBRACK ")"

%token                      OPCURVBRACK "{"
%token                      CLCURVBRACK "}"

%token                      COMMA       ","
%token                      SEMICOLON   ";"

%token                      COLON       ":"

%token                      SCAN        "?"

%token                      END         0   "end of file"

%token                      FUNC_DECL   "func"
%token                      RET         "return"

%token                      LEXERR

/*PRIORITIES*/
%left OR
%left AND
%left EQ NEQ
%left MORE LESS GTE LTE
%left ADD SUB
%left MUL DIV MOD
%precedence UNARY_OP

/* AST TREE */
%type <std::vector<AST::Node*>*>    statementHandler
%type <AST::Node*>                  statement

%type <AST::Node*>                  expression
%type <AST::Node*>                  opStatement

%start translationStart

%%

translationStart            :   statementHandler                {
                                                                    AST::ScopeNode* globalScope = new AST::ScopeNode (@1, nullptr);
                                                                    if ($1) {
                                                                        for (auto curStmtNode: *($1))             
                                                                        {   
                                                                            if (!curStmtNode)
                                                                                continue;
                                                                            
                                                                            globalScope->addChild (curStmtNode);
                                                                        }
                                                                        
                                                                        delete $1;
                                                                    }
                                                                    driver->setRoot (globalScope);
                                                                    #if 1
                                                                        driver->callDump (std::cout);
                                                                        exit (1);
                                                                    #endif
                                                                };

statementHandler            :   statement                       {
                                                                    if ($1) {
                                                                        $$ = new std::vector<AST::Node*>;
                                                                        $$->push_back ($1);
                                                                    } else
                                                                        $$ = nullptr;
                                                                }
                            |   statementHandler statement      {
                                                                    if ($1 && $2) {
                                                                        $1->push_back ($2);
                                                                        $$ = $1;
                                                                    } else {
                                                                        $$ = nullptr;
                                                                        if ($1) {
                                                                            for (auto v: *($1))
                                                                                delete v;
                                                                        }
                                                                        delete $1;
                                                                        delete $2;
                                                                    }
                                                                };


statement                   :   expression                      {   $$ = $1;        }
                            |   SEMICOLON                       {   $$ = nullptr;   };

expression                  :   opStatement                     {   $$ = $1;        };

/*OPERATORS*/
opStatement                 :   NUMBER                          {   $$ = new AST::NumNode   ($1);                                       }
                            |   opStatement OR opStatement      {   $$ = makeBinOperNode (AST::OperNode::OperType::OR, $1, $3, @2);     }
                            |   opStatement AND opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::AND, $1, $3, @2);    }
                            |   opStatement EQ opStatement      {   $$ = makeBinOperNode (AST::OperNode::OperType::EQ, $1, $3, @2);     }
                            |   opStatement NEQ opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::NEQ, $1, $3, @2);    }
                            |   opStatement MORE opStatement    {   $$ = makeBinOperNode (AST::OperNode::OperType::MORE, $1, $3, @2);   }
                            |   opStatement LESS opStatement    {   $$ = makeBinOperNode (AST::OperNode::OperType::LESS, $1, $3, @2);   }
                            |   opStatement GTE opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::GTE, $1, $3, @2);    }
                            |   opStatement LTE opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::LTE, $1, $3, @2);    }
                            |   opStatement ADD opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::ADD, $1, $3, @2);    }
                            |   opStatement SUB opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::SUB, $1, $3, @2);    }
                            |   opStatement MUL opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::MUL, $1, $3, @2);    }
                            |   opStatement DIV opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::DIV, $1, $3, @2);    }
                            |   opStatement MOD opStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::MOD, $1, $3, @2);    }
                            |   UN_SUB opStatement %prec UNARY_OP  {   $$ = makeUnaryOperNode (AST::OperNode::OperType::UNARY_M, $2, @1);  }
                            |   UN_ADD opStatement %prec UNARY_OP  {   $$ = makeUnaryOperNode (AST::OperNode::OperType::UNARY_P, $2, @1);  };

%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location, FrontendDriver* driver) {
    
    try {

        return driver->yylex (yylval, location);
    
    } catch (std::runtime_error& err) {

        std::cout << err.what () << std::endl;
        throw err; 

    }

}

void parser::error (const parser::location_type& location, const std::string& what) {

}

}
