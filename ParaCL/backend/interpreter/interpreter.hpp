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

//=======================================================================
//===================== SCOPE TABLE IMPLEMENTATION ======================
//=======================================================================

    struct ScopeTblWrapper {

        enum class WrapperType {

            NUM,
            FUNC

        };

        WrapperType type_;
        ScopeTblWrapper (WrapperType type): type_ (type) {}
        virtual ~ScopeTblWrapper () = 0;

    };

    struct NumScope final: public ScopeTblWrapper {
        
        int val_;

        NumScope (const int val): ScopeTblWrapper (ScopeTblWrapper::WrapperType::NUM), val_ (val) {}

    };

    struct FuncScope final: public ScopeTblWrapper {

        const AST::FuncNode* name_ = nullptr;
        const AST::FuncNode* args_ = nullptr;
        const AST::ScopeNode* execScope_ = nullptr;

        FuncScope (const AST::FuncNode* funcDecl): 
            ScopeTblWrapper (ScopeTblWrapper::WrapperType::FUNC)
        {
            
            const AST::FuncNode* leftChild = static_cast<const AST::FuncNode*>((*funcDecl)[0]);
            if (leftChild->getFuncCompType () == AST::FuncNode::FuncComponents::FUNC_NAME) {

                name_ = leftChild;
                leftChild = static_cast<const AST::FuncNode*>((*funcDecl)[1]);

            }
            args_ = leftChild;
            execScope_ = static_cast<const AST::ScopeNode*>(funcDecl->getRightChild ());

        }
        
    };

    struct Scope final {
        using tblSource = std::pair<std::string, ScopeTblWrapper*>;
        using tblVec = std::vector<tblSource>;
        using tblIt = tblVec::const_iterator;

    private:
        tblVec tbl_;
        tblIt endOfVisibleInParent_;
        Scope *parent_;

    public:

        Scope (Scope *parent = nullptr, tblIt it = tblIt(nullptr)) : parent_ (parent), endOfVisibleInParent_ (it) {}

        void push (tblSource obj) { tbl_.push_back (obj); }

        tblIt tblBegin () const { return tbl_.cbegin (); }
        tblIt tblEnd () const { return tbl_.cend (); }
        bool tbl_empty () const { return tbl_.empty(); }

        tblIt lookup (const std::string& name) {
            
            for (tblIt curIt = tbl_.begin(), endIt = tbl_.end(); curIt != endIt; ++curIt)
                if ((*curIt).first == name)
                    return curIt;

            Scope *curScope = this;

            while (curScope->parent_ != nullptr) {
                for (tblIt curIt = curScope->parent_->tbl_.begin(), endIt = (curScope->endOfVisibleInParent_) + 1; curIt != endIt; ++curIt)
                    if ((*curIt).first == name)
                        return curIt;

                curScope = curScope->parent_;
            }

            return tblIt (nullptr);
        }
    };

//=======================================================================

    struct Context;

    struct EvalApplyNode {

        const AST::Node*    node_   = nullptr;
        EvalApplyNode*      parent_ = nullptr;

    public:
        EvalApplyNode (const AST::Node* node, EvalApplyNode* parent): node_ (node), parent_(parent) {}

        const AST::Node* getNode () const { return node_; }
        virtual std::pair<EvalApplyNode*, EvalApplyNode*> eval (Context& context) = 0;

    };

    template <typename T>
    class StackWrapper {

        std::vector<T> stack_;

    public:
        T push_back  (T* node) {
        
            stack_.push_back (node); 
            return node; 
        
        }

        void pop_back   () { stack_.pop_back (); }

        bool empty () const { return stack_.empty (); }
        size_t size () const { return stack_.size (); }

        T back () const { return stack_.back (); }
        
        T operator[] (const size_t pos) { return stack_ [pos]; }
        const T operator[] (const size_t pos) const { return stack_ [pos]; }        

    };

    class EAScope final: public EvalApplyNode {

        int curChildrenToExec_ = 0;
        std::vector<EvalApplyNode*> children_;
    public:
        EAScope (const AST::ScopeNode* astScope, EvalApplyNode* parent);

        std::pair<EvalApplyNode*, EvalApplyNode*> eval (Context& context) override;

        EvalApplyNode* getLastChildren () const { return children_.back (); }
    };

    class EAAssign final: public EvalApplyNode {

        std::string         lhs_;
        EvalApplyNode*      rhs_;
    public:
        EAAssign (const AST::OperNode* astAssign, EvalApplyNode* parent);

        std::pair<EvalApplyNode*, EvalApplyNode*> eval (Context& context) override;

        std::string getLhs () const {return lhs_;}
        EvalApplyNode* getRhs () const {return rhs_;} 
    };

    class EANum final: public EvalApplyNode {

        int val_;
    public:
        EANum (const AST::NumNode* astNum, EvalApplyNode* parent):
            EvalApplyNode (astNum, parent), val_ (astNum->getValue ()) {}

        int getVal () const { return val_; }
        
        std::pair<EvalApplyNode*, EvalApplyNode*> eval (Context& context) override;

    };

    class EAVar final: public EvalApplyNode {

        std::string name_;
    public:
        EAVar (const AST::VarNode* astVar, EvalApplyNode* parent):
            EvalApplyNode (astVar, parent), name_ (astVar->getName ()) {}

        std::string getName () const { return name_; }
        
        std::pair<EvalApplyNode*, EvalApplyNode*> eval (Context& context) override { return {nullptr, nullptr}; }

    };

    struct Context final {
        
        std::vector<Scope*>             scopeStack_;
        std::vector<ScopeTblWrapper*>   calcStack_;
        std::vector<EvalApplyNode*>     retStack_;

        EvalApplyNode* prev_ = nullptr;

        void addScope ();
        void replaceScope (Scope* scope, const EAScope* curEAScope);
        void removeCurScope ();

    };

    class Interpreter final {

        const AST::ScopeNode* root_;
    public:
        Interpreter (const AST::ScopeNode* root): root_ (root) {}

        void run ();

    };

}  // namespace interpret

#endif
