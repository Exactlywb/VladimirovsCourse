#ifndef INTERPRET_H__
#define INTERPRET_H__

#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ast.hpp"
#include "location.hh"
#include "nAryTree.hpp"

namespace interpret {

    using vecASTIt = std::vector<AST::Node *>::const_iterator;

    enum class DataType {

        VAR,
        FUNC

    };

    struct Wrapper {  // To put Wrapper* into unordered_map

        DataType type_ = DataType::FUNC;

        Wrapper (const DataType type) : type_ (type) {}
        Wrapper () {}

        virtual ~Wrapper () = default;
    };

    class FuncObject final : public Wrapper {
        AST::FuncNode *node_;  // TODO: Template pointer

    public:
        FuncObject (AST::FuncNode *node) : node_ (node), Wrapper (DataType::FUNC) {}

        ~FuncObject () = default;

        void setNode (AST::FuncNode *node) { node_ = node; }

        AST::FuncNode *getNode () const { return node_; }
    };

    template <typename T>
    class Variable final : public Wrapper {
        T value_{};

    public:
        Variable (T value) : value_ (value), Wrapper (DataType::VAR) {}

        ~Variable () = default;

        void setVal (T value) { value_ = value; }

        T getVal () const { return value_; }
    };

    struct Scope final {
        std::vector<Scope *> children_;
        Scope *parent_ = nullptr;
        std::unordered_map<std::string, Wrapper *> tbl_;
        using tblIt = std::unordered_map<std::string, Wrapper *>::iterator;
        using constScopeIt = std::vector<Scope *>::const_iterator;

    public:
        Scope () = default;

        // Rule of 0
        Scope (const Scope &other) = delete;
        Scope (Scope &&other) = delete;

        Scope &operator= (const Scope &other) = delete;
        Scope &operator= (Scope &&other) = delete;

        ~Scope ();  //! TODO

        constScopeIt childBegin () const { return children_.cbegin (); }

        constScopeIt childEnd () const { return children_.cend (); }

        void nodeDump (std::ostream &out) const
        {
            out << "it is for compile";  // TODO: something with it
        }

        std::pair<Scope *, tblIt> smartLookup (const std::string &name);
        void add (const std::string &name, Wrapper *var);
        void add (Scope *scope);
    };

    class ErrorDetector final : public std::runtime_error {
        yy::location location_;

    public:
        ErrorDetector (const char *err, yy::location loc) : std::runtime_error (err), location_ (loc) {}

        yy::location getLocation () const { return location_; }
    };

    class Interpreter final {
        Tree::NAryTree<Scope *> *globalScope_;
        Tree::NAryTree<AST::Node *> *tree_;

        struct RecursionController final {
            const int maxDeepth_ = 1000;
            int callDeepth_ = 0;

            RecursionController &operator++ () noexcept
            {  // Open the window, pls...
                ++callDeepth_;
                return *this;
            }

            RecursionController operator++ (int) noexcept
            {
                callDeepth_++;
                return *this;
            }

            RecursionController &operator-- () noexcept
            {
                --callDeepth_;
                return *this;
            }

            RecursionController operator-- (int) noexcept
            {
                callDeepth_--;
                return *this;
            }

            bool isOverflow () const noexcept { return callDeepth_ >= maxDeepth_; }
        };

        RecursionController recursionController_;
        std::stack<int> callStack_;
        bool scopeExecution_ = true;

    public:
        Interpreter (Tree::NAryTree<AST::Node *> *tree) : globalScope_ (new Tree::NAryTree<Scope *> (new Scope)), tree_ (tree) {}

        ~Interpreter () { delete globalScope_; }

        void run ();

        // Rule of 0
        Interpreter (const Interpreter &other) = delete;
        Interpreter (Interpreter &&other) = delete;

        Interpreter &operator= (const Interpreter &other) = delete;
        Interpreter &operator= (Interpreter &other) = delete;

        // Implementin' functions
    private:
        int CalcExpr (Scope *curScope, vecASTIt it);
        int CalcVar (Scope *curScope, AST::VarNode *var);
        int CalcOper (Scope *curScope, AST::OperNode *node);

        void execScope (Scope *curScope, AST::ScopeNode *node);
        int execRealCall (Scope *curScope, Wrapper *obj, AST::OperNode *callNode);
        int execCallUsingStack (Scope *newScope, AST::FuncNode *funcDecl);
        int execCallUsingStack (Scope *newScope, AST::ScopeNode *ASTScope);
        int CalcScope (AST::ScopeNode *node);
        int getCallValue ();

        void createNewScope (
            Scope *newScope, AST::FuncNode *funcName, AST::FuncNode *funcArgs, AST::FuncNode *funcDecl, AST::OperNode *callNode, Scope *curScope);
        int execCall (Scope *curScope, AST::OperNode *callNode);

        void execOper (Scope *curScope, AST::OperNode *node);
        void execCond (Scope *curScope, AST::CondNode *node);

        void print (Scope *curScope, AST::OperNode *node);
        void ret (Scope *curScope, AST::OperNode *node);
        int assignment (Scope *curScope, AST::OperNode *node);

        void execIf (Scope *curScope, AST::CondNode *node);
        void execWhile (Scope *curScope, AST::CondNode *node);
    };

}  // namespace interpret

#endif
