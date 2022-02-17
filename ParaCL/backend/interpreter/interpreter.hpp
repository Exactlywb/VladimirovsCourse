#ifndef INTERPRET_H__
#define INTERPRET_H__

#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>

#include "ast.hpp"
#include "location.hh"
#include "nAryTree.hpp"

namespace interpret {

    class ScopeWrapper {

        std::string name_;
    public:
        ScopeWrapper (const std::string& name): name_ (name) {}

        std::string getName () const { return name_; }

        virtual ~ScopeWrapper () = 0;

    };

    struct VarScope final: public ScopeWrapper {
        
        int val_;
    
        VarScope (const std::string& name): ScopeWrapper (name) {}
        VarScope (const std::string& name, int val): ScopeWrapper (name), 
                                                     val_ (val) {}

        int getData () const { return val_; }

    };

    struct FuncObjScope final: public ScopeWrapper {
        class FuncComp {

            AST::FuncNode* funcDecl_ = nullptr;
            
            AST::FuncNode* funcName_ = nullptr;
            AST::FuncNode* funcArgs_ = nullptr;

            AST::ScopeNode* scope_ = nullptr;

        public:
            FuncComp (AST::FuncNode* funcDecl): funcDecl_ (funcDecl) {

                AST::FuncNode* lFuncChild = static_cast<AST::FuncNode*>((*funcDecl) [0]);
                if (lFuncChild->getFuncCompType () == AST::FuncNode::FuncComponents::FUNC_NAME) {

                    funcName_ = lFuncChild;
                    lFuncChild = static_cast<AST::FuncNode*>((*funcDecl) [1]);

                }

                funcArgs_ = lFuncChild;

                scope_ = static_cast<AST::ScopeNode*> (funcDecl->getRightChild ());

            }

        };

    private:
        std::unique_ptr<FuncComp> (funcDetails_);

    public:

        FuncObjScope (const std::string& name, AST::FuncNode* funcDecl = nullptr): 
            ScopeWrapper (name), 
            funcDetails_ (std::unique_ptr<FuncComp> (new FuncComp (funcDecl))) {}

        FuncComp* getData () const { return funcDetails_.get (); }

    };

    class Scope final {

        std::vector<ScopeWrapper*> tbl_;

    public:
        using tblIt = std::vector<ScopeWrapper*>::const_iterator;        

        void push (ScopeWrapper* obj) { tbl_.push_back (obj); }
        
        tblIt tblBegin () const { return tbl_.cbegin (); }
        tblIt tblEnd () const { return tbl_.cend (); }

        tblIt lookup (const std::string& name) {
        
            for (auto&& obj = tbl_.cbegin (), end = tbl_.cend (); obj != end; ++obj) {

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

    class Context final { //!TODO
    
        Scope scope_;
        
        std::vector<const AST::Node*> execStack_;
        const AST::Node *prev = nullptr;

    };

    class Interpreter final {
        
        Context context_;
        AST::ScopeNode* root_;
    public:
        Interpreter (AST::ScopeNode* root): root_ (root) {}

        void run ();

    }; 

    class EvalApplyNode {

        AST::Node* node_;
        EvalApplyNode* parent_ = nullptr;

    public:
        EvalApplyNode (AST::Node* node): node_ (node) {}

        AST::Node* getNode () const { return node_; }
        virtual const EvalApplyNode *eval (Context& context) const = 0;

    };

    class EAScope final: public EvalApplyNode {

    public:
        EAScope (AST::ScopeNode* astScope): EvalApplyNode (astScope) {}
        const EvalApplyNode *eval (Context& context) const override; //!TODO

    };

    class EAWhile final: public EvalApplyNode {

    public:
        EAWhile (AST::CondNode* astCond): EvalApplyNode (astCond) {}
        const EvalApplyNode *eval (Context& context) const override; //!TODO

    };

    class EAIf final: public EvalApplyNode {

    public:
        EAIf (AST::CondNode* astCond): EvalApplyNode (astCond) {}
        const EvalApplyNode *eval (Context& context) const override; //!TODO

    };

    template <typename operT>
    class EABinOp final: public EvalApplyNode {

    public:
        EABinOp (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        const EvalApplyNode *eval (Context& context) const override; //!TODO

    };

    template <typename operT>
    class EAUnaryOp final: public EvalApplyNode {

    public:
        EAUnaryOp (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        const EvalApplyNode *eval (Context& context) const override; //!TODO

    };

    class EAReturn final: public EvalApplyNode {

    public:
        EAReturn (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        const EvalApplyNode *eval (Context& context) const override; //!TODO

    };

    class EAAssign final: public EvalApplyNode {

    public:
        EAAssign (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        const EvalApplyNode *eval (Context& context) const override; //!TODO

    };

    struct BinOpAdd {

        int operator() (const VarScope& lhs, const VarScope& rhs) const {

            //!TODO

        }

    };

    struct UnaryOpPrint {

        int operator() (const VarScope& val) {

            //!TODO

        }

    };

}  // namespace interpret

#endif
