#ifndef INTERPRET_H__
#define INTERPRET_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "nAryTree.hpp"

namespace interpret {

    enum class DataType {

        INT,
        CHAR,   //
        DOUBLE  //useless yet

    };

    struct VarWrapper {  //To put VarWrapper* into unordered_map

        DataType type_ = DataType::INT;

        VarWrapper (const DataType type) : type_ (type) {}
        VarWrapper () {}

        virtual ~VarWrapper () = default;
    };

    template <typename T>
    class Variable final : public VarWrapper {
        T value_{};

    public:
        Variable (T value, DataType type = DataType::INT) : value_ (value),
                                                            VarWrapper (type) {}

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
        std::unordered_map<std::string, VarWrapper *> tbl_;

    public:
        Scope () = default;

        //Rule of 0
        Scope (const Scope &other) = delete;
        Scope (Scope &&other) = delete;

        Scope &operator= (const Scope &other) = delete;
        Scope &operator= (Scope &&other) = delete;

        ~Scope ();  //!TODO

        std::vector<Scope*>::const_iterator childBegin () const 
        {
            return children_.cbegin ();
        }

        std::vector<Scope*>::const_iterator childEnd () const 
        {
            return children_.cend ();
        }

        void nodeDump (std::ostream &out) const
        {
            out << "it is for compile";  //TODO: something with it
        }

        VarWrapper *lookup (const std::string &name) const;
        void add (const std::string &name, VarWrapper *var);
        void add (Scope *scope);
    };

    class Interpreter final {
        Tree::NAryTree<Scope *> *globalScope_;
        Tree::NAryTree<AST::Node *> *tree_;

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
        int CalcExpr (Scope *curScope, std::vector<AST::Node*>::const_iterator it);
        int CalcVar (Scope *curScope, AST::VarNode *var);
        int CalcOper (Scope *curScope, AST::OperNode *node);\

        void execScope (Scope *curScope, AST::ScopeNode *node);

        void execOper (Scope *curScope, AST::OperNode *node);
        void execCond (Scope *curScope, AST::CondNode *node);

        void print (Scope *curScope, AST::OperNode *node);
        int assignment (Scope *curScope, AST::OperNode *node);

        void execIf (Scope *curScope, AST::CondNode *node);
        void execWhile (Scope *curScope, AST::CondNode *node);
    };

}  // namespace interpret

#endif
