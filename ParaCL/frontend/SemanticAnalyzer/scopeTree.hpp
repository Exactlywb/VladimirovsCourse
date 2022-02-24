#ifndef SCOPE_TREE_H__
#define SCOPE_TREE_H__

#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ast.hpp"
#include "nAryTree.hpp"

struct TypeWrapper {  // To put TypeWrapper* into unordered_map

    enum class DataType {

        VAR,
        FUNC

    };

    DataType type_ = DataType::FUNC;

    TypeWrapper (const DataType type) : type_ (type) {}
    TypeWrapper () {}

    virtual ~TypeWrapper () = default;
};

class FuncObject final : public TypeWrapper {
    AST::FuncNode *node_;

public:
    FuncObject (AST::FuncNode *node) : node_ (node), TypeWrapper (DataType::FUNC) {}

    ~FuncObject () = default;

    void setNode (AST::FuncNode *node) { node_ = node; }

    AST::FuncNode *getNode () const { return node_; }
};

class Variable final : public TypeWrapper {
    AST::VarNode *node_;

public:
    Variable (AST::VarNode *node) : node_ (node), TypeWrapper (DataType::VAR) {}

    ~Variable () = default;

    AST::VarNode *getNode () const { return node_; }
};

class Scope final {
    std::vector<Scope *> children_;
    Scope *parent_ = nullptr;
    std::unordered_map<std::string, TypeWrapper *> tbl_;

public:
    using tblIt = std::unordered_map<std::string, TypeWrapper *>::iterator;
    using constScopeIt = std::vector<Scope *>::const_iterator;

    void nameCopy (Scope *other)
    {
        for (auto v : other->tbl_)
            std::cout << v.first << std::endl;
        tbl_ = other->tbl_;
    }

    Scope () = default;

    // Rule of 0
    Scope (const Scope &other) = delete;
    Scope (Scope &&other) = delete;

    Scope &operator= (const Scope &other) = delete;
    Scope &operator= (Scope &&other) = delete;

    ~Scope ()
    {
        for (auto i : tbl_)
            delete i.second;
    }

    constScopeIt childBegin () const { return children_.cbegin (); }

    constScopeIt childEnd () const { return children_.cend (); }

    void nodeDump (std::ostream &out) const
    {
        out << "it is for compile";  // TODO: something with it
    }

    std::pair<Scope *, tblIt> smartLookup (const std::string &name)
    {
        Scope *curScope = this;
        while (curScope) {
            auto elem = curScope->tbl_.find (name);
            if (elem != curScope->tbl_.end ())
                return {curScope, elem};

            curScope = curScope->parent_;
        }

        return {nullptr, tbl_.end ()};
    }

    void add (const std::string &name, TypeWrapper *var) { tbl_.insert ({name, var}); }

    void add (Scope *scope)
    {
        children_.push_back (scope);
        scope->parent_ = this;
    }
};

#endif
