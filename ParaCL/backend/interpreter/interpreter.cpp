#include "interpreter.hpp"

namespace interpret {

    Scope::~Scope () {

        //!TODO

    }

    VarWrapper* Scope::lookup (const std::string& name) const {
        
        const Scope* curScope = this;
        while (curScope) {

            auto elem = curScope->tbl_.find (name);
            if (elem != curScope->tbl_.end ())
                return elem->second;
        
            curScope = curScope->parent_;

        }

        return nullptr;

    }

    void Scope::add (const std::string& name, VarWrapper* var) {
        tbl_.insert ({name, var}); 
    }

    void Scope::add (Scope* scope) {
    
        children_.push_back (scope);
        scope->parent_ = this;
    
    }
    
    void Interpreter::print (Scope* curScope, const std::string& name) {

        VarWrapper* curVar = curScope->lookup (name);
        Variable<int>* clearVar = static_cast<Variable<int>*>(curVar);       
        std::cout << clearVar->getVal () << std::endl;

    }

namespace {
    
    int CalcExpr (AST::Node* node) {

        return 5;

    }

    void ExecOperator (Interpreter* interpret, AST::OperNode* node, Scope* curScope) {
        
        switch (node->getOpType ()) {

            case AST::OperNode::OperType::ASSIGN: {

                std::vector<AST::Node*> children = node->getChildren ();
                AST::VarNode* varNode = static_cast<AST::VarNode*>(children [0]);
                interpret->assignment  (curScope, varNode->getName (), 
                                        CalcExpr (children [1]));
                break;

            }
            case AST::OperNode::OperType::PRINT: {
                
                std::vector<AST::Node*> children = node->getChildren ();
                for (auto curChild: children)
                    std::cout << CalcExpr (curChild) << std::endl;
                break;

            }

        }

    }

}

    void Interpreter::run () {
        
        Scope* curScope = globalScope_;
        std::vector<AST::Node*> curNodes = tree_->getRoot ()->getChildren ();
        while (!curNodes.empty ()) {

            AST::Node* nodeToExec = curNodes [0];
            switch (nodeToExec->getType ()) {

                case AST::NodeT::OPERATOR: {
                    ExecOperator (this, static_cast<AST::OperNode*>(nodeToExec), curScope);
                    break;
                }
                default: {
                    throw std::runtime_error ("Unexpected node to execute");
                }

            }
            curNodes.erase (curNodes.begin ());

        }

    }

}

