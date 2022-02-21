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

        std::vector<std::pair<std::string, std::shared_ptr<ScopeWrapper>>> tbl_;

    public:
        using tblIt = std::vector<std::pair<std::string, std::shared_ptr<ScopeWrapper>>>::const_iterator;        

        void push (std::pair<std::string, std::shared_ptr<ScopeWrapper>> obj) { tbl_.push_back (obj); }
        
        tblIt tblBegin () const { return tbl_.cbegin (); }
        tblIt tblEnd () const { return tbl_.cend (); }

        tblIt lookup (const std::string& name) {
        
            for (auto obj = tbl_.cbegin (), end = tbl_.cend (); obj != end; ++obj) {

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

    struct Context;

    class EvalApplyNode: public std::enable_shared_from_this<EvalApplyNode> {

        const AST::Node* node_;
        EvalApplyNode* parent_ = nullptr;

    public:
        EvalApplyNode (const AST::Node* node): node_ (node) {}

        const AST::Node* getNode () const { return node_; }
        virtual void eval (Context& context) const = 0;

    };

    struct ExecStack {

        std::vector<std::shared_ptr<const EvalApplyNode>> execStack_;
        
        void push_back (std::shared_ptr<const EvalApplyNode> node) {
            execStack_.push_back (node);
        }

        bool empty () { return execStack_.empty (); }
        size_t size () { return execStack_.size ();}

        std::shared_ptr<const EvalApplyNode> operator[] (size_t pos) { return execStack_ [pos];}
        void pop_back () {
            execStack_.pop_back (); 
        }

        std::shared_ptr<const EvalApplyNode> back () { return execStack_.back (); }
    
        void push_back (const AST::Node* node);

    };

    struct CalcStack {

        std::vector<std::shared_ptr<ScopeWrapper>> calcStack_;
        
        void push_back (std::shared_ptr<ScopeWrapper> node) { calcStack_.push_back (node); }
        void pop_back () { calcStack_.pop_back (); }
        std::shared_ptr<ScopeWrapper> back () { return calcStack_.back (); }

    };

    struct Context final { //!TODO
    
        Scope scope_;
        
        //std::vector<const EvalApplyNode*> execStack_;
        ExecStack execStack_;
        //std::vector<ScopeWrapper*> calcStack_;
        CalcStack calcStack_;
        const AST::Node *prev = nullptr;

    };

    class Interpreter final {

        AST::ScopeNode* root_;
    public:
        Interpreter (AST::ScopeNode* root): root_ (root) {}

        void run ();

    }; 

    class EAWhile final: public EvalApplyNode {

    public:
        EAWhile (const AST::CondNode* astCond): EvalApplyNode (astCond) {}
        void eval (Context& context) const override {} //!TODO

    };

    class EAIf final: public EvalApplyNode {

    public:
        EAIf (const AST::CondNode* astCond): EvalApplyNode (astCond) {}
        void eval (Context& context) const override {} //!TODO

    };

    template <typename operT>
    class EABinOp final: public EvalApplyNode {

        operT apply_;
    public:
        EABinOp (const AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override {

            const AST::Node* node = EvalApplyNode::getNode();
            const AST::Node* rhs  = (*node)[1];
            const AST::Node* lhs  = (*node)[0];
            
            if (context.prev == rhs) {
                
                context.prev = getNode(); 
                apply_(context);
                return;
            }

            context.execStack_.push_back(shared_from_this ());
            context.execStack_.push_back(rhs);
            context.execStack_.push_back(lhs);
        }

    };

    struct UnaryOpPrint {

        void operator() (Context& context) const {

            std::shared_ptr<VarScope> var = std::static_pointer_cast<VarScope>(context.calcStack_.back());
            std::cout << var->val_ << std::endl;
            context.calcStack_.pop_back();
        }

    };

    template <typename operT>
    class EAUnaryOp final: public EvalApplyNode {

        operT apply_;
    public:
        EAUnaryOp (const AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override
        {
            const AST::Node* node = EvalApplyNode::getNode();
            const AST::Node* rhs  = (*node)[0];
            
            if (context.prev == rhs) {
                
                context.prev = getNode(); 
                apply_(context);
                return;
            }

            context.execStack_.push_back(shared_from_this ());
            context.execStack_.push_back(rhs);
        }

    };

    class EAReturn final: public EvalApplyNode {

    public:
        EAReturn (const AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override {} //!TODO

    };

    class EAAssign final: public EvalApplyNode {
        
    public:
        EAAssign (const AST::OperNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override; //!TODO

    };

    class EAVar final: public EvalApplyNode {

    public:
        EAVar (const AST::VarNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override; //!TODO

    };

    class EAFunc final: public EvalApplyNode {

    public:
        EAFunc (const AST::FuncNode* astOper): EvalApplyNode (astOper) {}
        void eval (Context& context) const override {} //!TODO

    };

    class EANum final: public EvalApplyNode {
        
        int val_;
    public:
        EANum (const AST::NumNode* astOper): EvalApplyNode (astOper), val_(astOper->getValue()) {}
        void eval (Context& context) const override; //!TODO

    };

    struct BinOpAdd {

        int operator() (Context& context) const {

            std::shared_ptr<VarScope> lhs = std::static_pointer_cast<VarScope> (context.calcStack_.back());
            context.calcStack_.pop_back();
            std::shared_ptr<VarScope> rhs = std::static_pointer_cast<VarScope> (context.calcStack_.back());
            context.calcStack_.pop_back();
            context.calcStack_.push_back(std::make_shared<VarScope> (lhs->getData() + rhs->getData()));
        }
    };

    class EAScope final: public EvalApplyNode {

        std::vector<std::shared_ptr<EvalApplyNode>> children_;
    public:
        EAScope (AST::ScopeNode* astScope);
        void eval (Context& context) const override; //!TODO
    };
}  // namespace interpret

#endif
