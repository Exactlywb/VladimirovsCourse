#include "interpreter.hpp"

namespace interpret {

namespace {

        EvalApplyNode* buildApplyNodeFromCondition (const AST::CondNode* condNode) {

            if (condNode->getConditionType() == AST::CondNode::ConditionType::IF)
                return (new EAIf (condNode));
                        
            return (new EAWhile (condNode));
        }

        EvalApplyNode* buildApplyNodeFromOperator (const AST::OperNode* opNode) { //TODO: Make return

            switch (opNode->getOpType ()) {

                case AST::OperNode::OperType::RETURN:
                    return (new EAReturn (opNode));
                case AST::OperNode::OperType::ASSIGN:
                    return (new EAAssign (opNode));
                case AST::OperNode::OperType::UNARY_M:
                case AST::OperNode::OperType::UNARY_P: 
                case AST::OperNode::OperType::PRINT: 
                    return (new EAUnaryOp<UnaryOpPrint> (opNode));

                case AST::OperNode::OperType::ADD:
                case AST::OperNode::OperType::SUB:
                case AST::OperNode::OperType::MUL:
                case AST::OperNode::OperType::DIV:
                case AST::OperNode::OperType::MORE:
                case AST::OperNode::OperType::LESS:
                case AST::OperNode::OperType::EQ:
                case AST::OperNode::OperType::NEQ:
                case AST::OperNode::OperType::GTE:
                case AST::OperNode::OperType::LTE:
                case AST::OperNode::OperType::OR:
                case AST::OperNode::OperType::AND:
                case AST::OperNode::OperType::MOD: 
                    return (new EABinOp<BinOpAdd>  (opNode));
            }
        }

        EvalApplyNode* buildApplyNodeFromVariable (const AST::VarNode*  varNode)   { return (new EAVar  (varNode));  }
        EvalApplyNode* buildApplyNodeFromFunction (const AST::FuncNode* funcNode)  { return (new EAFunc (funcNode)); }
        EvalApplyNode* buildApplyNodeFromNumber   (const AST::NumNode*  numNode)   { return (new EANum  (numNode));  }

        EvalApplyNode* buildApplyNode (const AST::Node* node) {

            switch (node->getType()) {

                case AST::NodeT::CONDITION:
                    return buildApplyNodeFromCondition (static_cast<const AST::CondNode*> (node));
                case AST::NodeT::FUNCTION:
                    return buildApplyNodeFromFunction (static_cast<const AST::FuncNode*> (node));
                case AST::NodeT::OPERATOR:
                    return buildApplyNodeFromOperator (static_cast<const AST::OperNode*> (node));
                case AST::NodeT::VARIABLE:
                    return buildApplyNodeFromVariable (static_cast<const AST::VarNode*> (node));
                case AST::NodeT::NUMBER:
                    return buildApplyNodeFromNumber (static_cast<const AST::NumNode*> (node));
            }
        }

    }

ScopeWrapper::~ScopeWrapper () = default;

void ExecStack::push_back (const AST::Node* node) {

    execStack_.push_back (buildApplyNode (node));

}

void Interpreter::run () {

    Context context;

    context.execStack_.push_back(new EAScope (root_));
    while (!context.execStack_.empty()) {

        const EvalApplyNode* curNode = context.execStack_[context.execStack_.size() - 1];
        context.execStack_.pop_back();
        curNode->eval(context);

        context.prev = curNode->getNode();
    }
}

EAScope::EAScope (AST::ScopeNode* astScope): EvalApplyNode (astScope) {

    auto&& st  = getNode()->childBegin();
    auto&& fin = getNode()->childEnd();

    for (; st != fin; ++st)      
        children_.push_back(buildApplyNode(*st));
    
}

void EAScope::eval (Context& context) const {
    
    auto&& fin = children_.rend();
    auto&& st  = children_.rbegin();

    for (; fin != st; ++st) 
        context.execStack_.push_back(*st); 
}   


void EAAssign::eval (Context& context) const {

    const AST::Node* node = EvalApplyNode::getNode();
    const AST::Node* lhs  = (*node)[0];
    const AST::Node* rhs  = (*node)[1];

    if (context.prev == rhs) {
        
        // std::cout << "XACKMAJSDJ" << std::endl;
        context.prev = getNode(); 
        auto id = context.execStack_.back();
        context.execStack_.pop_back();
        auto val = context.calcStack_.back();

        const AST::VarNode* varNode = static_cast<const AST::VarNode*>(id->getNode());
        std::string name = varNode->getName();

        auto findRes = context.scope_.lookup(name);

        if (findRes == context.scope_.tblEnd())
            context.scope_.push({name, val});
        else {

            VarScope* curVal = static_cast<VarScope*>(val);
            VarScope* var = static_cast<VarScope*>((*findRes).second);
            var->val_ = curVal->val_;
        }

        return;
    }

    context.execStack_.push_back(lhs);
    context.execStack_.push_back(this);
    context.execStack_.push_back(rhs);

    // std::cout << "num = " << buildApplyNode(rhs) << std::endl;
    
    // std::cout << "Size = " << context.execStack_.size() << std::endl;
    
}

void EAVar::eval (Context& context) const {

    const AST::VarNode* varNode = static_cast<const AST::VarNode*> (EvalApplyNode::getNode());
    auto findRes = context.scope_.lookup(varNode->getName());


    VarScope* var = static_cast<VarScope*> ((*findRes).second);
    context.calcStack_.push_back(var);
} 

void EANum::eval (Context& context) const { 

    context.calcStack_.push_back(new VarScope(val_));
    // std::cout << "Popal wefw?" << std::endl;
}


}
