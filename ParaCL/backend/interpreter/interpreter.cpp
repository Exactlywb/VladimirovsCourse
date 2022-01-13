#include "interpreter.hpp"

namespace interpret {

    ~Scope () {

        //!TODO

    }

    VarWrapper* Scope::lookup (const std::string& name) const {
        
        Scope* curScope = this;
        while (curScope) {

            auto elem = curScope->tbl_.find (name);
            if (elem != curScope->tbl_.end ())
                return elem.second;
        
            curScope = curScope->parent_;

        }

        return nullptr;

    }

    void Scope::add (const std::string& name, Variable* var) {
        tbl_.insert ({name, var}); 
    }

    void Scope::add (Scope* scope) {
    
        children_.push_back (scope);
        scope->parent_ = this;
    
    }
    
    void Interpreter::print (Scope* curScope, const std::string& name) {

        Variable* curVar = curScope->lookup (name);        
        std::cout << curVar->getVal () << std::endl;

    }

namespace {
    
    void ExecOperator (Interpreter* interpret, AST::OperNode* node, Scope* curScope) {
        
        switch (node->opType_) {

            case AST::OperNode::OperType::ASSIGN: {
                
                interpret->assignment (curScope, node->);
                break;
            }

        }

    }

}

    void Interpreter::run () {
        
        std::vector<AST::Node*> curNodes = tree_->getRoot->getChildren ();
        while (!curNodes.empty ()) {

            AST::Node* nodeToExec = curNodes [0];
            switch (nodeToExec->type_) {

                case OPERATOR: {
                    ExecOperator (nodeToExec);
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

