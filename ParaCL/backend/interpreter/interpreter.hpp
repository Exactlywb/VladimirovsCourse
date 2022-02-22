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

        enum class WrapperType {

            ID,
            NUM,
            FUNC

        };

        WrapperType type_;
        ScopeWrapper (WrapperType type): type_ (type) {}
        virtual ~ScopeWrapper () = 0;

    };

//=======================================================================
//======================== SCOPE IMPLEMENTATION =========================
//=======================================================================

    struct NumScope final: public ScopeWrapper {
        
        int val_;

        NumScope (const int val): ScopeWrapper (ScopeWrapper::WrapperType::NUM), val_ (val) {}

    };

    struct IDScope final: public ScopeWrapper { //!TODO remove?

        const std::string name_;

        IDScope (const std::string& name): ScopeWrapper (ScopeWrapper::WrapperType::ID), name_ (name) {}

    };

    struct FuncScope final: public ScopeWrapper {

        const AST::FuncNode* name_ = nullptr;
        const AST::FuncNode* args_ = nullptr;
        const AST::ScopeNode* execScope_ = nullptr;

        FuncScope (const AST::FuncNode* funcDecl): 
            ScopeWrapper (ScopeWrapper::WrapperType::FUNC)
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

//=======================================================================

    struct Context;

    struct EvalApplyNode: public std::enable_shared_from_this<EvalApplyNode> {

        const AST::Node* node_ = nullptr;
        std::shared_ptr<EvalApplyNode> parent_ = nullptr;

    public:
        EvalApplyNode (const AST::Node* node, std::shared_ptr<EvalApplyNode> parent): node_ (node), parent_(parent) {}

        const AST::Node* getNode () const { return node_; }
        virtual std::pair<std::shared_ptr<EvalApplyNode>, std::shared_ptr<EvalApplyNode>> eval (Context& context) = 0;

    };

    template <typename T>
    class StackWrapper {

        std::vector<std::shared_ptr<T>> stack_;

    public:
        std::shared_ptr<T> push_back  (std::shared_ptr<T> node) {
        
            stack_.push_back (node); 
            return node; 
        
        }

        void pop_back   () { stack_.pop_back (); }

        bool empty () const { return stack_.empty (); }
        size_t size () const { return stack_.size (); }

        std::shared_ptr<T> back () const { return stack_.back (); }
        
        std::shared_ptr<T> operator[] (const size_t pos) { return stack_ [pos]; }
        const std::shared_ptr<T> operator[] (const size_t pos) const { return stack_ [pos]; }        

    };

    class Dummy final: public EvalApplyNode {

    public:
        std::pair<std::shared_ptr<EvalApplyNode>, std::shared_ptr<EvalApplyNode>> eval (Context& context) override {
            throw std::runtime_error ("You've called Dummy class to execute");
        }

        Dummy (): EvalApplyNode (nullptr, nullptr) {}

    };

    class EAScope final: public EvalApplyNode {

        int curChildrenToExec_ = 0;
        std::vector<std::shared_ptr<EvalApplyNode>> children_;
    public:
        EAScope (const AST::ScopeNode* astScope, std::shared_ptr<EvalApplyNode>);

        std::pair<std::shared_ptr<EvalApplyNode>, std::shared_ptr<EvalApplyNode>> eval (Context& context) override;

        std::shared_ptr<EvalApplyNode> getLastChildren () const { return children_.back (); }
    };

    class EAAssign final: public EvalApplyNode {

        std::shared_ptr<IDScope>        lhs_;
        std::shared_ptr<EvalApplyNode>  rhs_;
    public:
        EAAssign (const AST::OperNode* astAssign, std::shared_ptr<EvalApplyNode>);

        std::pair<std::shared_ptr<EvalApplyNode>, std::shared_ptr<EvalApplyNode>> eval (Context& context) override;

        std::shared_ptr<IDScope> getLhs () const {return lhs_;}
        std::shared_ptr<EvalApplyNode> getRhs () const {return rhs_;} 
    };

    struct Context final {
        
        std::vector<std::shared_ptr<Scope>>             scopeStack_;
        std::vector<std::shared_ptr<ScopeWrapper>>      calcStack_;
        std::vector<std::shared_ptr<EvalApplyNode>>     retStack_;

        std::shared_ptr<EvalApplyNode> prev_ = nullptr;

        void replaceScope (std::shared_ptr<Scope> scope, std::shared_ptr<const EAScope> curEAScope);
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
