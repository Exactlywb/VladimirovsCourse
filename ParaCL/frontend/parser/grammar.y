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
%type <AST::Node*>                  fakeAssignment

%type <AST::Node*>                  cond

%type <AST::Node*>                  expr

%type <AST::Node*>                  assignment

%type <AST::Node*>                  factor
%type <AST::Node*>                  term

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

args                        :   fakeAssignment                            {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    $$->push_back ($1);
                                                                }
                            |   args COMMA fakeAssignment                 {
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

conditionExpression         :   OPCIRCBRACK fakeAssignment CLCIRCBRACK    {   $$ = $2;    };

body                        :   OPCURVBRACK statementHandler CLCURVBRACK 
                                                                {
                                                                    AST::ScopeNode* newScope = new AST::ScopeNode ();
                                                                    for (auto curStmtNode: *($2))
                                                                        newScope->addChild (curStmtNode);
                                                                    delete $2;
                                                                    $$ = newScope;
                                                                };

assignment                  :   ID ASSIGN fakeAssignment SEMICOLON        {
                                                                    AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    AST::VarNode* newVar    = new AST::VarNode  ($1);
                                                                    newNode->addChild (newVar);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

fakeAssignment              :   cond                            {   $$ = $1;        }
                            |   fakeAssignment ASSIGN cond      {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

cond                        :   expr                            {   $$ = $1;        }
                            |   cond EQ expr                    {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::EQ);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   cond NEQ expr                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::NEQ);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   cond MORE expr                  {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::MORE);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   cond LESS expr                  {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::LESS);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   cond GTE expr                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::GTE);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   cond LTE expr                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::LTE);
                                                                    newNode->addChild ($1);
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

term                        :   NUMBER                          {   $$ = new AST::NumNode   ($1);                               }
                            |   SCAN                            {   $$ = new AST::OperNode  (AST::OperNode::OperType::SCAN);    }
                            |   ID                              {   $$ = new AST::VarNode   ($1);                               }
                            |   OPCIRCBRACK fakeAssignment CLCIRCBRACK    {   $$ = $2;                                                    };

%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, FrontendDriver* driver) {
    
    try {

        return driver->yylex (yylval);
    
    } catch (std::runtime_error& err) {

        throw err;        

    }

}

void parser::error (const std::string&) {/*todo*/}

}
