#ifndef INTERPRET_H__
#define INTERPRET_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>

#include "ast.hpp"
#include "nAryTree.hpp"

namespace interpret {

    enum class DataType {

        VAR,
        FUNC // ???

    };

    struct Wrapper {  //To put Wrapper* into unordered_map

        DataType type_ = DataType::FUNC;

        Wrapper (const DataType type) : type_ (type) {}
        Wrapper () {}

        virtual ~Wrapper () = default;
    };

    class FuncObject final : public Wrapper {
        AST::FuncNode* node_; // TODO: Template pointer 

    public:
        FuncObject (AST::FuncNode* node) : node_ (node), Wrapper (DataType::FUNC) {}

        ~FuncObject () = default;

        void setNode (AST::FuncNode* node)
        {
            node_ = node;
        }

        AST::FuncNode* getNode () const
        {
            return node_;
        }
    };

    template <typename T>
    class Variable final : public Wrapper {
        T value_{};

    public:
        Variable (T value) : value_ (value), Wrapper (DataType::VAR) {}

        ~Variable () = default;

        void setVal (T value)
        {
            value_ = value;
        }

        T getVal () const
        {
            return value_;
        }
    };

    struct Scope final {
        std::vector<Scope *> children_;
        Scope *parent_ = nullptr;
        std::unordered_map<std::string, Wrapper *> tbl_;

    public:
        Scope () = default;

        //Rule of 0
        Scope (const Scope &other) = delete;
        Scope (Scope &&other) = delete;

        Scope &operator= (const Scope &other) = delete;
        Scope &operator= (Scope &&other) = delete;

        ~Scope ();  //!TODO

        std::vector<Scope *>::const_iterator childBegin () const
        {
            return children_.cbegin ();
        }

        std::vector<Scope *>::const_iterator childEnd () const
        {
            return children_.cend ();
        }

        void nodeDump (std::ostream &out) const
        {
            out << "it is for compile";  //TODO: something with it
        }

        Wrapper *lookup (const std::string &name) const;
        void add (const std::string &name, Wrapper *var);
        void add (Scope *scope);
    };

    class Interpreter final {
        Tree::NAryTree<Scope *> *globalScope_;
        Tree::NAryTree<AST::Node *> *tree_;

        std::stack<int> callStack_;
        bool scopeExecution_ = true;

    public:
        Interpreter (Tree::NAryTree<AST::Node *> *tree) : globalScope_ (new Tree::NAryTree<Scope *> (new Scope)),
                                                          tree_ (tree) {}

        ~Interpreter ()
        {
            delete globalScope_;
        }

        void run ();

        //Rule of 0
        Interpreter (const Interpreter &other) = delete;
        Interpreter (Interpreter &&other) = delete;

        Interpreter &operator= (const Interpreter &other) = delete;
        Interpreter &operator= (Interpreter &other) = delete;

        //Implementin' functions
    private:
        int CalcExpr (Scope *curScope, std::vector<AST::Node *>::const_iterator it);
        int CalcVar (Scope *curScope, AST::VarNode *var);
        int CalcOper (Scope *curScope, AST::OperNode *node);

        void execScope (Scope *curScope, AST::ScopeNode *node);
        int execCall (Scope *curScope, AST::OperNode* callNode);

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
