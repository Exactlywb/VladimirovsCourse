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

}

%locations

%token <int>                NUMBER
%token <std::string>        ID

%token                      ASSIGN      "="

%token                      ADD         "+"
%token                      SUB         "-"
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

%left '+' '-' '*' '/' '%'
%right '=' 

/* AST TREE */
%type <AST::Node*>                  lvl15
%type <AST::Node*>                  lvl14
%type <AST::Node*>                  lvl13
%type <AST::Node*>                  lvl9
%type <AST::Node*>                  lvl8
%type <AST::Node*>                  lvl6
%type <AST::Node*>                  lvl5
%type <AST::Node*>                  lvl3

%type <AST::Node*>                  term
%type <AST::Node*>                  atomic

%type <AST::Node*>                  assignment

%type <AST::Node*>                  statement

%type <AST::Node*>                  ifStatement
%type <AST::Node*>                  whileStatement

%type <AST::Node*>                  body

%type <AST::Node*>                  conditionExpression

%type <AST::Node*>                  printStatement

%type <AST::Node*>                  func
%type <AST::Node*>                  returnStatement
%type <AST::Node*>                  hiddenReturn

%type <std::vector<AST::Node*>*>    argsList
%type <std::vector<AST::Node*>*>    args

%type <std::vector<AST::Node*>*>    exprList
%type <std::vector<AST::Node*>*>    expA

%type <std::vector<AST::Node*>*>    statementHandler

%start translationStart

%%

/* FUNCTIONS IN THE NEAR FUTURE HAS TO BE ADDED HERE */
translationStart            :   statementHandler                {
                                                                    AST::ScopeNode* globalScope = new AST::ScopeNode ();
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
                                                                    #if 0
                                                                        driver->callDump (std::cout);
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


statement                   :   assignment                      {   $$ = $1;    }
                            |   ifStatement                     {   $$ = $1;    }
                            |   whileStatement                  {   $$ = $1;    }
                            |   printStatement                  {   $$ = $1;    }
                            |   returnStatement                 {   $$ = $1;    }
                            |   hiddenReturn                    {   $$ = $1;    }
                            |   error SEMICOLON                 {
                                                                    driver->pushError (@1, "Undefined statement");
                                                                    $$ = nullptr;
                                                                }
                            |   error END                       {
                                                                    driver->pushError (@1, "Undefined statement");
                                                                    $$ = nullptr;
                                                                };

hiddenReturn                :   lvl14 SEMICOLON                 {
                                                                    AST::OperNode* retNode = new AST::OperNode (AST::OperNode::OperType::RETURN);
                                                                    retNode->addChild ($1);
                                                                    $$ = retNode;
                                                                };
returnStatement             :   RET lvl15 SEMICOLON             {
                                                                    AST::OperNode* retNode = new AST::OperNode (AST::OperNode::OperType::RETURN);
                                                                    retNode->addChild ($2);
                                                                    $$ = retNode;
                                                                };                                    

printStatement              :   PRINT lvl15 SEMICOLON           {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::PRINT);
                                                                    newNode->addChild ($2); 
                                                                    $$ = newNode;
                                                                }
                            |   PRINT error SEMICOLON           { 
                                                                    driver->pushError (@2, "Undefined expression in print");
                                                                    $$ = nullptr;
                                                                }
                            |   PRINT error END                 {   
                                                                    driver->pushError (@2, "Undefined expression in print");
                                                                    $$ = nullptr;
                                                                };

argsList                    :   OPCIRCBRACK args CLCIRCBRACK    {   $$ = $2;    };

args                        :   ID                              {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    AST::VarNode* newParam = new AST::VarNode ($1);
                                                                    $$->push_back (newParam);
                                                                }
                            |   args COMMA ID                   {
                                                                    AST::VarNode* newParam = new AST::VarNode ($3);
                                                                    $1->push_back (newParam);
                                                                    $$ = $1;
                                                                };

exprList                    :   OPCIRCBRACK expA CLCIRCBRACK    {   $$ = $2;    };

expA                        :   lvl15                           {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    $$->push_back ($1);
                                                                }
                            |   args COMMA lvl15                {
                                                                    $1->push_back ($3);
                                                                    $$ = $1;
                                                                };

ifStatement                 :   IF conditionExpression body     {
                                                                    if ($2 && $3) {
                                                                        AST::CondNode* newNode = new AST::CondNode (AST::CondNode::ConditionType::IF);
                                                                        newNode->addChild ($2);
                                                                        newNode->addChild ($3);
                                                                        $$ = newNode;
                                                                    } else {
                                                                        $$ = nullptr;
                                                                        delete $2;
                                                                        delete $3;
                                                                    }

                                                                };

whileStatement              :   WHILE conditionExpression body  {
                                                                    if ($2 && $3) {
                                                                        AST::CondNode* newNode = new AST::CondNode (AST::CondNode::ConditionType::WHILE);
                                                                        newNode->addChild ($2);
                                                                        newNode->addChild ($3);
                                                                        $$ = newNode;
                                                                    } else {
                                                                        $$ = nullptr;
                                                                        delete $2;
                                                                        delete $3;
                                                                    }
                                                                };

conditionExpression         :   OPCIRCBRACK lvl15 CLCIRCBRACK   {   $$ = $2;    }
                            |   OPCIRCBRACK error CLCIRCBRACK   {
                                                                    $$ = nullptr;
                                                                    driver->pushError (@2, "Bad expression for condition");
                                                                };


body                        :   OPCURVBRACK statementHandler CLCURVBRACK 
                                                                {
                                                                    AST::ScopeNode* newScope = new AST::ScopeNode ();
                                                                    if ($2) {
                                                                        for (auto curStmtNode: *($2))
                                                                           newScope->addChild (curStmtNode);
                                                                    }
                                                                    delete $2;
                                                                    $$ = newScope;
                                                                }
                            |   OPCURVBRACK CLCURVBRACK         {   $$ = new AST::ScopeNode ();   };


assignment                  :   ID ASSIGN lvl15 SEMICOLON       {
                                                                    AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    AST::VarNode* newVar    = new AST::VarNode  ($1);
                                                                    newNode->addChild (newVar);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   ID ASSIGN func SEMICOLON        {
                                                                    AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    AST::VarNode* newVar    = new AST::VarNode ($1);
                                                                    newNode->addChild (newVar);                     //!TODO: remove copypaste
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   ID ASSIGN func                  {
                                                                    AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    AST::VarNode* newVar    = new AST::VarNode ($1);
                                                                    newNode->addChild (newVar);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   ID ASSIGN error SEMICOLON       {
                                                                    $$ = nullptr;
                                                                    driver->pushError (@3, "Bad expression after assignment");
                                                                }
                            |   ID ASSIGN   body                {
                                                                    AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    AST::VarNode* newVar    = new AST::VarNode ($1);
                                                                    newNode->addChild (newVar);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   ID error SEMICOLON              {
                                                                    $$ = nullptr;
                                                                    driver->pushError (@1, "Unexpected operation with variable");
                                                                };

func                        :   FUNC_DECL argsList body         {
                                                                    AST::FuncNode* funcDecl = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_DECL);
                                                                    AST::FuncNode* funcArgs = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_ARGS);
                                                                    for (auto v: *($2))
                                                                        funcArgs->addChild (v);
                                                                    delete $2;
                                                                    funcDecl->addChild (funcArgs);
                                                                    funcDecl->addChild ($3);
                                                                    $$ = funcDecl;
                                                                }
                            |   FUNC_DECL argsList COLON ID body{
                                                                    AST::FuncNode* funcDecl = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_DECL);
                                                                    AST::FuncNode* funcArgs = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_ARGS);
                                                                    for (auto v: *($2))
                                                                        funcArgs->addChild (v);
                                                                    delete $2;

                                                                    AST::FuncNode* funcName = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_NAME);
                                                                    AST::VarNode*  funcID   = new AST::VarNode ($4);
                                                                    funcName->addChild (funcID);

                                                                    funcDecl->addChild (funcName);
                                                                    funcDecl->addChild (funcArgs);
                                                                    funcDecl->addChild ($5);
                                                                    $$ = funcDecl;
                                                                };

lvl15                       :   lvl14                           {   $$ = $1;        }
                            |   ID ASSIGN lvl15                 {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::ASSIGN);
                                                                    newNode->addChild (new AST::VarNode ($1));
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

lvl5                        :   lvl3                            {   $$ = $1;        }
                            |   lvl5 MUL lvl3                   {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::MUL);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl5 DIV lvl3                   {
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::DIV);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                }
                            |   lvl5 MOD lvl3                   {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::MOD);
                                                                    newNode->addChild ($1);
                                                                    newNode->addChild ($3);
                                                                    $$ = newNode;
                                                                };

lvl3                        :   term                            {   $$ = $1;        }
                            |   SUB lvl3                        {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::UNARY_M);
                                                                    newNode->addChild ($2);
                                                                    $$ = newNode;
                                                                }
                            |   ADD lvl3                        {   
                                                                    AST::OperNode* newNode = new AST::OperNode (AST::OperNode::OperType::UNARY_P);
                                                                    newNode->addChild ($2);
                                                                    $$ = newNode;
                                                                };

term                        :   atomic                          {   $$ = $1;    }
                            |   OPCIRCBRACK lvl15 CLCIRCBRACK   {   $$ = $2;    };

atomic                      :   NUMBER                          {   $$ = new AST::NumNode   ($1);                               }
                            |   SCAN                            {   $$ = new AST::OperNode  (AST::OperNode::OperType::SCAN);    }
                            |   ID exprList                     {
                                                                    $$ = new AST::OperNode  (AST::OperNode::OperType::CALL);
                                                                    AST::VarNode* funcName  = new AST::VarNode ($1);

                                                                    AST::FuncNode* funcArgs = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_ARGS);
                                                                    for (auto v: *($2))
                                                                        funcArgs->addChild (v);
                                                                    delete $2;

                                                                    $$->addChild (funcName);
                                                                    $$->addChild (funcArgs);

                                                                }
                            |   ID                              {   $$ = new AST::VarNode   ($1);                               };

%%

namespace yy {

parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location, FrontendDriver* driver) {
    
    try {

        return driver->yylex (yylval, location);
    
    } catch (std::runtime_error& err) {

        throw err; 

    }

}

void parser::error (const parser::location_type& location, const std::string& what) {

}

}
