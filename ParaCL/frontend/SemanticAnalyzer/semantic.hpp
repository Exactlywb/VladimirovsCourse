#ifndef SEMANTIC_HPP__
#define SEMANTIC_HPP__

#include "nAryTree.hpp"
#include "ast.hpp"

struct SemanticAnalyzer {

    enum class ContextType {

        UsualContext,
        FuncContext

    };

private:
    ContextType context_ = ContextType::UsualContext;

public:
    void setContext (ContextType context) {
        context_ = context;
    }

    ContextType getContext () const {
        return context_;
    }

    SemanticAnalyzer () = default;

    //Let's implement 0-rule
    SemanticAnalyzer (const SemanticAnalyzer &other) = delete;
    SemanticAnalyzer (SemanticAnalyzer &&other) = delete;
    SemanticAnalyzer &operator= (const SemanticAnalyzer &other) = delete;
    SemanticAnalyzer &operator= (SemanticAnalyzer &&other) = delete;

    void run (Tree::NAryTree<AST::Node*>* tree);

};

#endif
