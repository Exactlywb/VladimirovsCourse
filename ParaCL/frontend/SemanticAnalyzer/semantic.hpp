#ifndef SEMANTIC_HPP__
#define SEMANTIC_HPP__

#include <functional>
#include <queue>

#include "ast.hpp"
#include "errorDetector.hpp"
#include "nAryTree.hpp"
#include "scopeTree.hpp"

struct SemanticAnalyzer final {
private:
    Tree::NAryTree<Scope *> *globalScope_;

    std::function<void (yy::location, const std::string &)> pushWarning_;
    std::function<void (yy::location, const std::string &)> pushError_;

public:
    SemanticAnalyzer () : globalScope_ (new Tree::NAryTree<Scope *> (new Scope)){};

    // Let's implement 0-rule
    SemanticAnalyzer (const SemanticAnalyzer &other) = delete;
    SemanticAnalyzer (SemanticAnalyzer &&other) = delete;
    SemanticAnalyzer &operator= (const SemanticAnalyzer &other) = delete;
    SemanticAnalyzer &operator= (SemanticAnalyzer &&other) = delete;

    ~SemanticAnalyzer () { delete globalScope_; }

    void run (Tree::NAryTree<AST::Node *> *tree,
              const std::function<void (yy::location, const std::string &)> pushWarning,
              const std::function<void (yy::location, const std::string &)>);

private:
    // HIDDEN RETURN ANALYZING
    void AnalyzeHiddenReturn (Tree::NAryTree<AST::Node *> *tree);

    void HiddenReturnNodesAnalyze (AST::Node *curNode);
    void UselessStatementRecognizer (AST::Node *curNode);

    // SCOPES ANALYZING
    void AnalyzeScopes (Scope *curScope, AST::ScopeNode *node);

    // RETURN POSITION ANALYZING
    void AnalyzeReturnPosition (Tree::NAryTree<AST::Node *> *tree);

    void HandleBinaryOperation (Scope *curScope, AST::Node *node);
    void CheckUnaryOperScope (Scope *curScope, AST::Node *node);
    void CheckExprScope (Scope *curScope, AST::OperNode *node);
    void CheckAssignStatementScope (Scope *curScope, AST::OperNode *node);
    void HandleExistVarInScope (Scope *curScope,
                                AST::VarNode *clearID,
                                AST::OperNode *node,
                                std::pair<Scope *, Scope::tblIt> findRes);
    void CheckScopeVarInExpr (Scope *curScope, AST::VarNode *node);

    void CheckConditionScopeExpr (Scope *curScope, AST::CondNode *node);
    void CheckCondScope (Scope *curScope, AST::CondNode *node);

    void CreateNewObjectInScope (Scope *curScope, AST::OperNode *node, AST::VarNode *clearID);
    void CreateNewFunctionInScope (Scope *curScope, AST::OperNode *node, AST::VarNode *clearID, AST::Node *rNode);
    void CreateNewVariableInScope (Scope *curScope, AST::OperNode *node, AST::VarNode *clearID);
    void CreateNewVariableViaScope (Scope *curScope, AST::OperNode *node, AST::VarNode *clearID, AST::Node *rNode);
    void AnalyzeNewScope (Scope *curScope, AST::Node *newScopeNode);

    void BuildExecStackFromExpression (std::vector<AST::Node *> &execVector, AST::Node *node);
    void CheckCallOperatorInExpr (Scope *curScope, AST::OperNode *node);
    void CheckArgsAmmountForCall (AST::FuncNode *funcArgs, AST::FuncNode *callArgs);
    void CheckOperatorInExpr (Scope *curScope, AST::OperNode *node);
};

#endif
