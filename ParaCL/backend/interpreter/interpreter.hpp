#ifndef INTERPRET_H__
#define INTERPRET_H__

#include "ast.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

namespace interpret {

enum class DataType {

    INT,
    CHAR,   //
    DOUBLE  //useless yet

};

struct VarWrapper {     //To put VarWrapper* into unordered_map

    DataType type_ = DataType::INT;

    VarWrapper (const DataType type): type_ (type) {}
    VarWrapper () {}

    virtual ~VarWrapper () = default;

};

template <typename T>
class Variable final: public VarWrapper {

    T       value_ {};
public:
    Variable (T value, DataType type = DataType::INT):
        value_  (value),
        VarWrapper (type) {}

    ~Variable () = default;

    void setVal (T value) {
        value_ = value;
    }

    T getVal () const {
        return value_;
    }

};

struct Scope final {

    std::vector<Scope*>                             children_;
    Scope*                                          parent_ = nullptr;
    std::unordered_map<std::string, VarWrapper*>    tbl_;

public:
    Scope () = default;

//Rule of 0
    Scope (const Scope& other)              = delete;
    Scope (Scope&& other)                   = delete;

    Scope& operator = (const Scope& other)  = delete;
    Scope& operator = (Scope&& other)       = delete;

    ~Scope ();  //!TODO

    VarWrapper* lookup  (const std::string& name) const;
    void        add     (const std::string& name, VarWrapper* var);
    void        add     (Scope* scope);

};

namespace {
    
    template <typename T>
    VarWrapper* CreateNewVarUsingType   (const T value, 
                                         const DataType type = DataType::INT) {
        
        VarWrapper* newVar = nullptr;
        switch (type) {

            case DataType::INT:
                newVar = new Variable<int> (value, type);            
                return newVar;

        }
    
        return newVar;

    }
    
}

class Interpreter final {

    Scope* globalScope_;
    AST::Tree* tree_;

public:
    Interpreter (AST::Tree* tree): 
        globalScope_    (new Scope),
        tree_           (tree) {} 
    
    ~Interpreter () {
        delete globalScope_;
    }

    void run ();

//Rule of 0
    Interpreter (const Interpreter& other)              = delete;
    Interpreter (Interpreter&& other)                   = delete;
    
    Interpreter& operator = (const Interpreter& other)  = delete;
    Interpreter& operator = (Interpreter& other)        = delete;

//Implementin' functions
    void print      (Scope* curScope, const std::string& name);
    
    template <typename T = int>
    void assignment (Scope* curScope, const std::string& name,
                     const T value, const DataType type = DataType::INT) {

        VarWrapper* var = curScope->lookup (name);
        if (!var) {
            
            VarWrapper* newVar = CreateNewVarUsingType (name, type, value);
            curScope->add (name, newVar);

        } else {

            switch (var->type_) {

                case DataType::INT: {
                    Variable<int>* clearVar = static_cast<Variable<int>*>(var);
                    clearVar->setVal (value);
                    break;
                } 
                case DataType::DOUBLE: {
                    Variable<int>* clearVar = static_cast<Variable<double>*>(var);
                    clearVar->setVal (value);
                    break;
                }

            }

        }

    }

};

}

#endif

