#ifndef SEMANTIC_HPP__
#define SEMANTIC_HPP__

#include <functional>
#include <queue>

#include "ast.hpp"
#include "errorDetector.hpp"
#include "nAryTree.hpp"
#include "scopeTree.hpp"

struct SemanticAnalyzer final {
    enum class ContextType {

        UsualContext,
        FuncContext

    };

private:
    ContextType context_ = ContextType::UsualContext;

    Tree::NAryTree<Scope *> *globalScope_;

public:
    void setContext (ContextType context) { context_ = context; }

    ContextType getContext () const { return context_; }

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

    void BuildingBinaryOperation (Scope *curScope,
                                  AST::Node *node,
                                  const std::function<void (yy::location, const std::string &)> pushWarning,
                                  const std::function<void (yy::location, const std::string &)> pushError);

    void CheckUnaryOperScope (Scope *curScope,
                              AST::Node *node,
                              const std::function<void (yy::location, const std::string &)> pushWarning,
                              const std::function<void (yy::location, const std::string &)> pushError);

    void CheckExprScope (Scope *curScope,
                         AST::OperNode *node,
                         const std::function<void (yy::location, const std::string &)> pushWarning,
                         const std::function<void (yy::location, const std::string &)> pushError);

    void CheckAssignStatementScope (Scope *curScope,
                                    AST::OperNode *node,
                                    const std::function<void (yy::location, const std::string &)> pushWarning,
                                    const std::function<void (yy::location, const std::string &)> pushError);

    void AnalyzeScopes (Scope *curScope,
                        AST::ScopeNode *node,
                        const std::function<void (yy::location, const std::string &)> pushWarning,
                        const std::function<void (yy::location, const std::string &)> pushError);

    void CheckConditionExpression (Scope *curScope,
                                   AST::CondNode *node,
                                   const std::function<void (yy::location, const std::string &)> pushWarning,
                                   const std::function<void (yy::location, const std::string &)> pushError);

    void CheckCondScope (Scope *curScope,
                         AST::CondNode *node,
                         const std::function<void (yy::location, const std::string &)> pushWarning,
                         const std::function<void (yy::location, const std::string &)> pushError);
};

#endif
