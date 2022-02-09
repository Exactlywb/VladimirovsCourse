#ifndef SEMANTIC_HPP__
#define SEMANTIC_HPP__

#include <functional>

#include "scopeTree.hpp"
#include "ast.hpp"
#include "nAryTree.hpp"
#include "errorDetector.hpp"

struct SemanticAnalyzer {
    
    enum class ContextType {

        UsualContext,
        FuncContext

    };

private:
    ContextType context_ = ContextType::UsualContext;

    Tree::NAryTree<AST::Node *> *tree_;

public:
    void setContext (ContextType context)
    {
        context_ = context;
    }

    ContextType getContext () const
    {
        return context_;
    }

    SemanticAnalyzer () = default;

    //Let's implement 0-rule
    SemanticAnalyzer (const SemanticAnalyzer &other) = delete;
    SemanticAnalyzer (SemanticAnalyzer &&other) = delete;
    SemanticAnalyzer &operator= (const SemanticAnalyzer &other) = delete;
    SemanticAnalyzer &operator= (SemanticAnalyzer &&other) = delete;

    void run    (Tree::NAryTree<AST::Node *> *tree, 
                 const std::function<void (yy::location, const std::string &)> pushWarning, 
                 const std::function<void (yy::location, const std::string &)>);
};

#endif
