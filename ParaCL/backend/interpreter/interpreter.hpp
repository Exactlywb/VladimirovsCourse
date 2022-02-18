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

    struct ScopeWrapper {

        virtual ~ScopeWrapper () = 0;
    };

    struct VarScope final: public ScopeWrapper {
        
        int val_;

        VarScope (int val): val_ (val) {}

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

        FuncObjScope (AST::FuncNode* funcDecl = nullptr): 
            funcDetails_ (std::unique_ptr<FuncComp> (new FuncComp (funcDecl))) {}

        FuncComp* getData () const { return funcDetails_.get (); }

    };

    class Scope final {

        std::vector<std::pair<std::string, ScopeWrapper*>> tbl_;

    public:
        using tblIt = std::vector<std::pair<std::string, ScopeWrapper*>>::const_iterator;        

        void push (std::pair<std::string, ScopeWrapper*> obj) { tbl_.push_back (obj); }
        
        tblIt tblBegin () const { return tbl_.cbegin (); }
        tblIt tblEnd () const { return tbl_.cend (); }

        tblIt lookup (const std::string& name) {
        
            for (auto&& obj = tbl_.cbegin (), end = tbl_.cend (); obj != end; ++obj) {

                if ((*obj).first == name)
                    return obj;
            }

            return tbl_.cend ();

        }

        tblIt lookup (const std::string& name, tblIt end) {

            for (tblIt obj = tbl_.cbegin (); obj != end; ++obj) {
                
                if ((*obj).first == name)
                    return obj;

            }

            return tbl_.cend ();

        }

    };

    class EvalApplyNode;

    struct Context final { //!TODO
    
        Scope scope_;
        
        std::vector<const EvalApplyNode*> execStack_;
        std::vector<ScopeWrapper*> calcStack_;
        const AST::Node *prev = nullptr;

    };

    class Interpreter final {

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
        virtual void eval (Context& context) const = 0;

    };

    

    class EAWhile final: public EvalApplyNode {

    public:
        EAWhile (AST::CondNode* astCond): EvalApplyNode (astCond) {}
        void eval (Context& context) const override {} //!TODO

    };

    class EAIf final: public EvalApplyNode {

    public:
        EAIf (AST::CondNode* astCond): EvalApplyNode (astCond) {}
        void eval (Context& context) const override {} //!TODO

    };

    template <typename operT>
    class EABinOp final: public EvalApplyNode {

    public:
        EABinOp (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override {} //!TODO

    };

    struct UnaryOpPrint {

        void operator() (Context& context) const {

            VarScope* var = static_cast<VarScope*> (context.calcStack_.back());
            std::cout << var->val_ << std::endl;
            context.calcStack_.pop_back();
        }

    };

    template <typename operT>
    class EAUnaryOp final: public EvalApplyNode {

        operT apply_;
    public:
        EAUnaryOp (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override
        {


            AST::Node* node = EvalApplyNode::getNode();
            AST::Node* rhs  = (*node)[0];
            
            if (context.prev == rhs) {
                
                context.prev = getNode(); 
                apply_(context);
                return;
            }

            context.execStack_.push_back(this);
            context.execStack_.push_back(buildApplyNode(rhs));
            
        }

    };

    class EAReturn final: public EvalApplyNode {

    public:
        EAReturn (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override {} //!TODO

    };

    class EAAssign final: public EvalApplyNode {
        
    public:
        EAAssign (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override; //!TODO

    };


    class EAPrint final: public EvalApplyNode { //Mom told me I'm special

    public:
        EAPrint (AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override; //!TODO

    };

    class EAVar final: public EvalApplyNode {

    public:
        EAVar (AST::VarNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override {} //!TODO

    };

    class EAFunc final: public EvalApplyNode {

    public:
        EAFunc (AST::FuncNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override {} //!TODO

    };

    class EANum final: public EvalApplyNode {
        
        int val_;
    public:
        EANum (AST::NumNode* astOper): EvalApplyNode (astOper), val_(astOper->getValue()) {}
        void eval (Context& context) const override; //!TODO

    };

    struct BinOpAdd {

        int operator() (const VarScope& lhs, const VarScope& rhs) const {

            //!TODO

        }

    };

    class EAScope final: public EvalApplyNode {

        std::vector<EvalApplyNode*> children_;
    public:
        EAScope (AST::ScopeNode* astScope);
        void eval (Context& context) const override; //!TODO
    };
}  // namespace interpret

#endif
