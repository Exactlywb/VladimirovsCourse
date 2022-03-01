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
%type <AST::Node*>                  assignStatement
%type <AST::Node*>                  orStatement
%type <AST::Node*>                  andStatement
%type <AST::Node*>                  eqStatement
%type <AST::Node*>                  cmpStatement
%type <AST::Node*>                  addStatement
%type <AST::Node*>                  mulStatement
%type <AST::Node*>                  unaryStatement

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

%type <std::vector<AST::Node*>*>    argsList
%type <std::vector<AST::Node*>*>    args

%type <std::vector<AST::Node*>*>    exprList
%type <std::vector<AST::Node*>*>    expA

%type <std::vector<AST::Node*>*>    statementHandler

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
                            |   orStatement SEMICOLON           {   $$ = $1;    }
                            |   printStatement                  {   $$ = $1;    }
                            |   body                            {   $$ = $1;    }
                            |   returnStatement                 {   $$ = $1;    }
                            |   error SEMICOLON                 {   driver->pushError (@1, "Undefined statement");  $$ = nullptr;   }
                            |   error END                       {   driver->pushError (@1, "Undefined statement");  $$ = nullptr;   };

returnStatement             :   RET assignStatement SEMICOLON   {   $$ = makeUnaryOperNode (AST::OperNode::OperType::RETURN, $2, @1);   };                                    

printStatement              :   PRINT assignStatement SEMICOLON {   $$ = makeUnaryOperNode (AST::OperNode::OperType::PRINT, $2, @1);     }
                            |   PRINT error SEMICOLON           {   driver->pushError (@2, "Undefined expression in print");    $$ = nullptr;   }
                            |   PRINT error END                 {   driver->pushError (@2, "Undefined expression in print");    $$ = nullptr;   };

argsList                    :   OPCIRCBRACK args CLCIRCBRACK    {   $$ = $2;        }
                            |   OPCIRCBRACK CLCIRCBRACK         {   $$ = nullptr;   };

args                        :   ID                              {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    AST::VarNode* newParam = new AST::VarNode ($1, @1);
                                                                    $$->push_back (newParam);
                                                                }
                            |   args COMMA ID                   {
                                                                    AST::VarNode* newParam = new AST::VarNode ($3, @3);
                                                                    $1->push_back (newParam);
                                                                    $$ = $1;
                                                                };

exprList                    :   OPCIRCBRACK expA CLCIRCBRACK    {   $$ = $2;        }
                            |   OPCIRCBRACK CLCIRCBRACK         {   $$ = nullptr;   };

expA                        :   assignStatement                 {
                                                                    $$ = new std::vector<AST::Node*>;
                                                                    $$->push_back ($1);
                                                                }
                            |   expA COMMA assignStatement      {   $1->push_back ($3); $$ = $1;    };

ifStatement                 :   IF conditionExpression statement{
                                                                    if ($2 && $3) {
                                                                        $$ = makeCondNode (AST::CondNode::ConditionType::IF, $2, $3, @1);
                                                                    } else {
                                                                        $$ = nullptr;
                                                                        delete $2;
                                                                        delete $3;
                                                                    }
                                                                };

whileStatement              :   WHILE conditionExpression statement
                                                                {
                                                                   if ($2 && $3) {
                                                                        AST::ScopeNode* newScope = new AST::ScopeNode (@3, nullptr);
                                                                        newScope->addChild ($3);
                                                                        $$ = makeCondNode (AST::CondNode::ConditionType::WHILE, $2, newScope, @1);
                                                                    } else {
                                                                        $$ = nullptr;
                                                                        delete $2;
                                                                        delete $3;
                                                                    } 
                                                                };

conditionExpression         :   OPCIRCBRACK assignStatement CLCIRCBRACK   
                                                                {   $$ = $2;    }
                            |   OPCIRCBRACK error CLCIRCBRACK   {   driver->pushError (@2, "Bad expression for condition"); $$ = nullptr;   };


body                        :   OPCURVBRACK statementHandler CLCURVBRACK 
                                                                {
                                                                    AST::ScopeNode* newScope = new AST::ScopeNode (@1);
                                                                    if ($2) {
                                                                        for (auto curStmtNode: *($2))
                                                                           newScope->addChild (curStmtNode);
                                                                    }
                                                                    delete $2;
                                                                    $$ = newScope;
                                                                }
                            |   OPCURVBRACK CLCURVBRACK         {   $$ = new AST::ScopeNode (@1);   };


assignment                  :   ID ASSIGN assignStatement SEMICOLON       
                                                                {   $$ = makeAssign ($1, $3, @2, @1);   }
                            |   ID ASSIGN func SEMICOLON        {   $$ = makeAssign ($1, $3, @2, @1);   }
                            |   ID ASSIGN func                  {   $$ = makeAssign ($1, $3, @2, @1);   }
                            |   ID ASSIGN error SEMICOLON       {   driver->pushError (@3, "Bad expression after assignment");  
                                                                    $$ = nullptr;   
                                                                }
                            |   ID ASSIGN body                  {   $$ = makeAssign ($1, $3, @2, @1);   }
                            |   ID error SEMICOLON              {   driver->pushError (@1, "Unexpected operation with variable");   
                                                                    $$ = nullptr;
                                                                }
                            |   error ASSIGN assignStatement SEMICOLON 
                                                                {   
                                                                    driver->pushError (@1, "rvalue can't become lvalue"); 
                                                                    $$ = nullptr; 
                                                                };

func                        :   FUNC_DECL argsList body         {
                                                                    AST::FuncNode* funcDecl = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_DECL, @1);
                                                                    AST::FuncNode* funcArgs = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_ARGS, @2);
                                                                    if ($2) {
                                                                        for (auto v: *($2))
                                                                            funcArgs->addChild (v);
                                                                        delete $2;
                                                                    }
                                                                    funcDecl->addChild (funcArgs);
                                                                    funcDecl->addChild ($3);
                                                                    $$ = funcDecl;
                                                                }
                            |   FUNC_DECL argsList COLON ID body
                                                                {
                                                                    AST::FuncNode* funcDecl = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_DECL, @1);
                                                                    AST::FuncNode* funcArgs = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_ARGS, @2);
                                                                    if ($2) {
                                                                        for (auto v: *($2))
                                                                            funcArgs->addChild (v);
                                                                        delete $2;
                                                                    }

                                                                    AST::FuncNode* funcName = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_NAME, @4);
                                                                    AST::VarNode*  funcID   = new AST::VarNode ($4, @4);
                                                                    funcName->addChild (funcID);

                                                                    funcDecl->addChild (funcName);
                                                                    funcDecl->addChild (funcArgs);
                                                                    funcDecl->addChild ($5);
                                                                    $$ = funcDecl;
                                                                };

assignStatement             :   orStatement                     {   $$ = $1;        }
                            |   ID ASSIGN assignStatement       {   $$ = makeAssign ($1, $3, @2, @1);  };

orStatement                 :   andStatement                    {   $$ = $1;        }
                            |   orStatement OR andStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::OR, $1, $3, @2);     };


andStatement                :   eqStatement                     {   $$ = $1;        }
                            |   andStatement AND eqStatement    {   $$ = makeBinOperNode (AST::OperNode::OperType::AND, $1, $3, @2);    };

eqStatement                 :   cmpStatement                    {   $$ = $1;        }
                            |   eqStatement EQ cmpStatement     {   $$ = makeBinOperNode (AST::OperNode::OperType::EQ, $1, $3, @2);     }
                            |   eqStatement NEQ cmpStatement    {   $$ = makeBinOperNode (AST::OperNode::OperType::NEQ, $1, $3, @2);    };

cmpStatement                :   addStatement                    {   $$ = $1;        }
                            |   cmpStatement MORE addStatement  {   $$ = makeBinOperNode (AST::OperNode::OperType::MORE, $1, $3, @2);  }
                            |   cmpStatement LESS addStatement  {   $$ = makeBinOperNode (AST::OperNode::OperType::LESS, $1, $3, @2);  }
                            |   cmpStatement GTE addStatement   {   $$ = makeBinOperNode (AST::OperNode::OperType::GTE, $1, $3, @2);   }
                            |   cmpStatement LTE addStatement   {   $$ = makeBinOperNode (AST::OperNode::OperType::LTE, $1, $3, @2);   };

addStatement                :   mulStatement                    {   $$ = $1;        }
                            |   addStatement ADD mulStatement   {   $$ = makeBinOperNode (AST::OperNode::OperType::ADD, $1, $3, @2);   }
                            |   addStatement SUB mulStatement   {   $$ = makeBinOperNode (AST::OperNode::OperType::SUB, $1, $3, @2);   };

mulStatement                :   unaryStatement                  {   $$ = $1;        }
                            |   mulStatement MUL unaryStatement {   $$ = makeBinOperNode (AST::OperNode::OperType::MUL, $1, $3, @2);   }
                            |   mulStatement DIV unaryStatement {   $$ = makeBinOperNode (AST::OperNode::OperType::DIV, $1, $3, @2);   }
                            |   mulStatement MOD unaryStatement {   $$ = makeBinOperNode (AST::OperNode::OperType::MOD, $1, $3, @2);   };

unaryStatement              :   term                            {   $$ = $1;        }
                            |   SUB unaryStatement              {   $$ = makeUnaryOperNode (AST::OperNode::OperType::UNARY_M, $2, @1); }
                            |   ADD unaryStatement              {   $$ = makeUnaryOperNode (AST::OperNode::OperType::UNARY_P, $2, @1); };

term                        :   atomic                          {   $$ = $1;    }
                            |   OPCIRCBRACK assignStatement CLCIRCBRACK
                                                                {   $$ = $2;    };

atomic                      :   NUMBER                          {   $$ = new AST::NumNode   ($1);                                   }
                            |   SCAN                            {   $$ = new AST::OperNode  (AST::OperNode::OperType::SCAN, @1);    }
                            |   ID exprList                     {
                                                                    $$ = new AST::OperNode  (AST::OperNode::OperType::CALL, @1);
                                                                    AST::VarNode* funcName  = new AST::VarNode ($1, @1);

                                                                    AST::FuncNode* funcArgs = new AST::FuncNode (AST::FuncNode::FuncComponents::FUNC_ARGS, @2);
                                                                    if ($2) {
                                                                        for (auto v: *($2))
                                                                            funcArgs->addChild (v);
                                                                        delete $2;
                                                                    }

                                                                    $$->addChild (funcName);
                                                                    $$->addChild (funcArgs);

                                                                }
                            /* |   body                            {   $$ = $1;    } */
                            |   ID                              {   $$ = new AST::VarNode   ($1, @1);                               };

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
