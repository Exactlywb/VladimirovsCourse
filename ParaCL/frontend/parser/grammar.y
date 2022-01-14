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

%token                      SCAN        "?"

%token                      LEXERR

%left '+' '-' '*' '/'
%right '='

/* AST TREE */
%type <AST::Node*>                  lvl15
%type <AST::Node*>                  lvl14
%type <AST::Node*>                  lvl13
%type <AST::Node*>                  lvl9
%type <AST::Node*>                  lvl8
%type <AST::Node*>                  lvl6
%type <AST::Node*>                  lvl5

%type <AST::Node*>                  term

%type <AST::Node*>                  assignment

%type <AST::Node*>                  statement

%type <AST::Node*>                  ifStatement
%type <AST::Node*>                  whileStatement

%type <AST::Node*>                  body

%type <AST::Node*>                  conditionExpression

%type <AST::Node*>                  printStatement

%type <std::vector<AST::Node*>*>    argsList
%type <std::vector<AST::Node*>*>    args
%type <std::vector<AST::Node*>*>    statementHandler

%start translationStart

%%

/* FUNCTIONS IN THE NEAR FUTURE HAS TO BE ADDED HERE */
translationStart            :   statementHandler                {   
                                                                    AST::ScopeNode* globalScope = new AST::ScopeNode ();
                                                                    for (auto curStmtNode: *($1))                                                                
                                                                        globalScope->addChild (curStmtNode);
                                                                    
                                                                    delete $1;
                                                                    driver->setRoot (globalScope);
                                                                    #if 0
                                                                        driver->callDump (std::cout);
                                                                    #endif
                                                                };

statementHandler            :   statement                       {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    $$->push_back ($1);
                                                                }
                            |   statementHandler statement      {
                                                                    $1->push_back ($2);
                                                                    $$ = $1;
                                                                };

statement                   :   assignment                      {   $$ = $1;    }
                            |   ifStatement                     {   $$ = $1;    }
                            |   whileStatement                  {   $$ = $1;    }
                            |   printStatement                  {   $$ = $1;    };

printStatement              :   PRINT argsList SEMICOLON        {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::PRINT);
                                                                    for (auto curArgNode: *($2))
                                                                        newNode->addChild (curArgNode);
                                                                    delete $2;
                                                                    $$ = newNode;
                                                                };

argsList                    :   OPCIRCBRACK args CLCIRCBRACK    {   $$ = $2;    };

args                        :   lvl15                           {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    $$->push_back ($1);
                                                                }
                            |   args COMMA lvl15                {
                                                                    $1->push_back ($3);
                                                                    $$ = $1;
                                                                };

ifStatement                 :   IF conditionExpression body     {   
                                                                    AST::CondNode* newNode = new AST::CondNode (AST::CondNode::ConditionType::IF);
                                                                    newNode->addChild ($2);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

whileStatement              :   WHILE conditionExpression body  {
                                                                    AST::CondNode* newNode = new AST::CondNode (AST::CondNode::ConditionType::WHILE);
                                                                    newNode->addChild ($2);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

conditionExpression         :   OPCIRCBRACK lvl15 CLCIRCBRACK    {   $$ = $2;    };

body                        :   OPCURVBRACK statementHandler CLCURVBRACK 
                                                                {
                                                                    AST::ScopeNode* newScope = new AST::ScopeNode ();
                                                                    for (auto curStmtNode: *($2))
                                                                        newScope->addChild (curStmtNode);
                                                                    delete $2;
                                                                    $$ = newScope;
                                                                };

assignment                  :   ID ASSIGN lvl15 SEMICOLON        {
                                                                    AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    AST::VarNode* newVar    = new AST::VarNode  ($1);
                                                                    newNode->addChild (newVar);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

lvl15                       :   lvl14                           {   $$ = $1;        }
                            |   lvl15 ASSIGN lvl14              {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

lvl14                       :   lvl13                           {   $$ = $1;        }
                            |   lvl14 OR lvl13                  {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::OR);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

lvl13                       :   lvl9                            {   $$ = $1;        }
                            |   lvl13 AND lvl9                  {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::AND);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

lvl9                        :   lvl8                            {   $$ = $1;        }
                            |   lvl9 EQ lvl8                    {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::EQ);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl9 NEQ lvl8                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::NEQ);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };
lvl8                        :   lvl6                            {   $$ = $1;        }
                            |   lvl8 MORE lvl6                  {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::MORE);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl8 LESS lvl6                  {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::LESS);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl8 GTE lvl6                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::GTE);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl8 LTE lvl6                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::LTE);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

lvl6                        :   lvl5                            {   $$ = $1;        }
                            |   lvl6 ADD lvl5                   {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::ADD);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl6 SUB lvl5                   {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::SUB);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

lvl5                        :   term                            {   $$ = $1;        }
                            |   lvl5 MUL term                   {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::MUL);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl5 DIV term                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::DIV);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

term                        :   NUMBER                          {   $$ = new AST::NumNode   ($1);                               }
                            |   SCAN                            {   $$ = new AST::OperNode  (AST::OperNode::OperType::SCAN);    }
                            |   ID                              {   $$ = new AST::VarNode   ($1);                               }
                            |   OPCIRCBRACK lvl15 CLCIRCBRACK   {   $$ = $2;                                                    };

%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, FrontendDriver* driver) {
    return driver->yylex (yylval);
}

void parser::error (const std::string&) {/*todo*/}

}
