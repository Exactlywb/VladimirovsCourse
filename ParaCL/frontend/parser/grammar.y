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
#include "frontend/driver/driver.hpp"
#include <string>

namespace yy {

    parser::token_type yylex(parser::semantic_type* yylval, FrontendDriver *driver);

}

}

%token <int>                NUMBER
%token <std::string>        ID

%token                      ASSIGN      "="

%token                      ADD         "+"
%token                      SUB         "-"
%token                      MUL         "*"
%token                      DIV         "/"

%token                      EQ          "=="
%token                      NEQ         "!="
%token                      MORE        ">"
%token                      LESS        "<"
%token                      GTE         ">="
%token                      LTE         "<="

%token                      IF          "if"

%token                      WHILE       "while"

%token                      OPCIRCBRACK "("
%token                      CLCIRCBRACK ")"

%token                      SEMICOLON   ";"

%token                      LEXERR

%left '+' '-' '*' '/'

/* AST TREE */
%type <AST::Node*>                  expr

%type <AST::Node*>                  assignment

%type <AST::Node*>                  factor
%type <AST::Node*>                  term

%type <AST::Node*>                  statement

%type <std::vector<AST::Node*>*>    statementHandler

%start translationStart

%%

/* FUNCTIONS IN THE NEAR FUTURE HAS TO BE ADDED HERE */
translationStart            :   statementHandler                {   
                                                                    AST::ScopeNode* globalScope = new AST::ScopeNode ();
                                                                    for (auto curStmtNode: *($1))                                                                
                                                                        globalScope->addChild (curStmtNode);
                                                                    
                                                                    //delete $1;
                                                                    driver->setRoot (globalScope);
                                                                    driver->callDump (std::cout);
                                                                };

statementHandler            :   statement                       {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    $$->push_back ($1);                                
                                                                }
                            |   statementHandler statement      {
                                                                    $1->push_back ($2);
                                                                    $$ = $1;
                                                                };

statement                   :   assignment                      {   $$ = $1;    };

assignment                  :   ID ASSIGN expr SEMICOLON        {
                                                                    AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    AST::VarNode* newVar    = new AST::VarNode  ($1);
                                                                    newNode->addChild (newVar);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

expr                        :   factor                          {   $$ = $1;        }
                            |   expr ADD factor                 {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::ADD);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   expr SUB factor                 {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::SUB);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

factor                      :   term                            {   $$ = $1;        }
                            |   factor MUL term                 {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::MUL);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   factor DIV term                 {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::DIV);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

term                        :   NUMBER                          {   $$ = new AST::NumNode ($1); }
                            |   OPCIRCBRACK expr CLCIRCBRACK    {   $$ = $2;                    };

%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, FrontendDriver* driver) {
    return driver->yylex (yylval);
}

void parser::error (const std::string&) {/*todo*/}

}
