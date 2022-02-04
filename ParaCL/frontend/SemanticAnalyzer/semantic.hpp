#ifndef SEMANTIC_HPP__
#define SEMANTIC_HPP__

#include <functional>

#include "ast.hpp"
#include "nAryTree.hpp"

struct SemanticAnalyzer {
    enum class ContextType {

        UsualContext,
        FuncContext

    };

private:
    ContextType context_ = ContextType::UsualContext;

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

    void run (Tree::NAryTree<AST::Node *> *tree,
              const std::function<void (yy::location, const std::string &)> pushWarning);

    // void pushWarning (const std::string &err)
    // {

    //     std::string errMsg = std::string ("Warning: ") + err;
    //     warnings_.push_back (errMsg + std::string ("\n"));

    // }
};

#endif
