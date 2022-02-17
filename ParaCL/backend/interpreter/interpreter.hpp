#ifndef INTERPRET_H__
#define INTERPRET_H__

#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <algorithm>

#include "ast.hpp"
#include "location.hh"
#include "nAryTree.hpp"

namespace interpret {

    class ScopeWrapper {

        std::string name_;
    public:
        std::string getName () const { return name_ }
        virtual auto&& getData () const = 0;

    };

    class VarScope final: public ScopeWrapper {
        
        int val_;
    
    public:
        VarScope (const std::string& name): ScopeWrapper (name) {}
        VarScope (const std::string& name, int val): ScopeWrapper (name), 
                                                     val_ (val) {}

        auto&& getData () const override { return val_; }

    };

    class FuncObjScope final: public ScopeWrapper {

        AST::FuncNode* funcDecl_;
    public:
        FuncObjScope (const std::string& name, AST::FuncNode* funcDecl = nullptr): 
            ScopeWrapper (name), funcDecl_ (funcDecl) {}

        auto&& getData () const override { return funcDecl_; }

    };

    class Scope final {

        std::vector<ScopeWrapper*> tbl_;

    public:
        using tblIt = std::vector<ScopeWrapper*>::const_iterator;        

        void push (ScopeWrapper* obj) { tbl_.push_back (obj); }
        
        tblIt tblBegin () const { return tbl_.cbegin (); }
        tblIt tblEnd () const { return tbl_.cend (); }

        tblIt lookup (const std::string& name) {
        
            for (auto&& obj: tbl_) {

                if ((*obj)->getName () == name)
                    return obj;

            }

            return tbl_.cend ();

        }

        tblIt lookup (const std::string& name, tblIt end) {

            for (tblIt obj = tbl_.cbegin (); obj != end; ++obj) {
                
                if ((*obj)->getName () == name)
                    return obj;

            }

            return tbl_.cend ();

        }

    }; 

}  // namespace interpret

#endif
