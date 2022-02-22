#include "interpreter.hpp"

namespace interpret {

namespace {

        std::shared_ptr<EvalApplyNode> buildApplyNodeFromCondition (const AST::CondNode* condNode) {

            if (condNode->getConditionType() == AST::CondNode::ConditionType::IF)
                return (std::make_shared<EAIf> (condNode));
                        
            return (std::make_shared<EAWhile> (condNode));
        }

        std::shared_ptr<EvalApplyNode> buildApplyNodeFromOperator (const AST::OperNode* opNode) { //TODO: Make return

            switch (opNode->getOpType ()) {

                case AST::OperNode::OperType::RETURN:
                    return (std::make_shared<EAReturn> (opNode));
                case AST::OperNode::OperType::ASSIGN:
                    return (std::make_shared<EAAssign> (opNode));
                case AST::OperNode::OperType::UNARY_M:
                    return (std::make_shared<EAUnaryOp<UnaryOpMinus>> (opNode));
                case AST::OperNode::OperType::UNARY_P: 
                    return (std::make_shared<EAUnaryOp<UnaryOpPlus>> (opNode));
                case AST::OperNode::OperType::PRINT: 
                    return (std::make_shared<EAUnaryOp<UnaryOpPrint>> (opNode));
                case AST::OperNode::OperType::ADD:
                    return std::make_shared<EABinOp<BinOpAdd>> (opNode);
                case AST::OperNode::OperType::SUB:
                    return std::make_shared<EABinOp<BinOpSub>> (opNode);
                case AST::OperNode::OperType::MUL:
                    return std::make_shared<EABinOp<BinOpMul>> (opNode);
                case AST::OperNode::OperType::DIV:
                    return std::make_shared<EABinOp<BinOpDiv>> (opNode);
                case AST::OperNode::OperType::MORE:
                    return std::make_shared<EABinOp<BinOpMore>> (opNode);
                case AST::OperNode::OperType::LESS:
                    return std::make_shared<EABinOp<BinOpLess>> (opNode);
                case AST::OperNode::OperType::EQ:
                    return std::make_shared<EABinOp<BinOpEQ>> (opNode);
                case AST::OperNode::OperType::NEQ:
                    return std::make_shared<EABinOp<BinOpNEQ>> (opNode);
                case AST::OperNode::OperType::GTE:
                    return std::make_shared<EABinOp<BinOpGTE>> (opNode);
                case AST::OperNode::OperType::LTE:
                    return std::make_shared<EABinOp<BinOpLTE>> (opNode);
                case AST::OperNode::OperType::OR:
                    return std::make_shared<EABinOp<BinOpOr>> (opNode);
                case AST::OperNode::OperType::AND:
                    return std::make_shared<EABinOp<BinOpAnd>> (opNode);
                case AST::OperNode::OperType::MOD: 
                    return (std::make_shared<EABinOp<BinOpMod>>  (opNode));
                case AST::OperNode::OperType::SCAN: 
                    return (std::make_shared<EAScan> (opNode));
            }

            throw std::runtime_error ("Unexpected operator type");
        }

        std::shared_ptr<EvalApplyNode> buildApplyNodeFromVariable (const AST::VarNode*  varNode)        { return (std::make_shared<EAVar> (varNode));       }
        std::shared_ptr<EvalApplyNode> buildApplyNodeFromFunction (const AST::FuncNode* funcNode)       { return (std::make_shared<EAFunc> (funcNode));     }
        std::shared_ptr<EvalApplyNode> buildApplyNodeFromNumber   (const AST::NumNode*  numNode)        { return (std::make_shared<EANum>  (numNode));      }
        std::shared_ptr<EvalApplyNode> buildApplyNodeFromScope    (const AST::ScopeNode*  scopeNode)    { return (std::make_shared<EAScope> (scopeNode));   }

        std::shared_ptr<EvalApplyNode> buildApplyNode (const AST::Node* node) {

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
                case AST::NodeT::SCOPE:
                    return buildApplyNodeFromScope (static_cast<const AST::ScopeNode*> (node));
            }
            
            throw std::runtime_error ("unexpected AST::Node type");
        }

    }

ScopeWrapper::~ScopeWrapper () = default;

void ExecStack::push_back (const AST::Node* node) {

    StackWrapper::push_back (buildApplyNode (node));

}

void Interpreter::run () {

    Context context;

    std::shared_ptr<EAScope> globalScope = std::make_shared<EAScope> (root_);

    context.execStack_.StackWrapper::push_back(globalScope);
    context.scopeStack_.push_back(globalScope);

    while (!context.execStack_.empty()) {

        std::shared_ptr<const EvalApplyNode> curNode = context.execStack_[context.execStack_.size() - 1];
        context.execStack_.pop_back();
        curNode->eval(context);

        std::cout << "Size = " << context.scopeStack_.size() << std::endl;

        context.prev = curNode->getNode();
    }
}

EAScope::EAScope (const AST::ScopeNode* astScope): EvalApplyNode (astScope) {

    auto st  = getNode()->childBegin();
    auto fin = getNode()->childEnd();

    for (; st != fin; ++st)      
        children_.push_back(buildApplyNode(*st));
    
}

void EAScope::eval (Context& context) const {
    
    auto&& fin = children_.rend();
    auto&& st  = children_.rbegin();

    for (; fin != st; ++st) 
        context.execStack_.StackWrapper::push_back(*st); 
}   


void EAAssign::eval (Context& context) const {

    const AST::Node* node = EvalApplyNode::getNode();
    const AST::Node* lhs  = (*node)[0];
    const AST::Node* rhs  = (*node)[1];

    if (context.prev == rhs) {
        
        context.prev = getNode(); 
        auto id = context.execStack_.back();
        context.execStack_.pop_back();
        auto val = context.calcStack_.back();

        const AST::VarNode* varNode = static_cast<const AST::VarNode*>(id->getNode());
        std::string name = varNode->getName();

        auto findRes = context.scope_.lookup(name);

        std::shared_ptr<const VarScope> curVal = std::static_pointer_cast<const VarScope>(val);
        
        if (findRes == context.scope_.tblEnd()) {
            context.scope_.push({name, std::make_shared<VarScope> (curVal->getData())});
        }
        else {
            std::shared_ptr<const VarScope> var = std::static_pointer_cast<const VarScope>((*findRes).second);
            // var->val_ = curVal->val_;
        }

        return;
    }

    context.execStack_.push_back(lhs);
    context.execStack_.StackWrapper::push_back(shared_from_this ());
    context.execStack_.push_back(rhs);
}

void EAScan::eval (Context& context) const {
    
    int var;
    std::cin >> var;
    context.calcStack_.push_back(std::make_shared<VarScope> (var));
}

void EAIf::eval (Context& context) const {

    const AST::Node* node = EvalApplyNode::getNode();
    const AST::Node* lhs  = (*node)[0];
    
    if (context.prev == lhs) {
        std::shared_ptr<const VarScope> cond = std::static_pointer_cast <const VarScope> (context.calcStack_.back ());
        
        if (cond->getData()) {
            context.execStack_.push_back ((*node)[1]);
        }
        context.calcStack_.pop_back ();
        return;
    }

    context.execStack_.StackWrapper::push_back (shared_from_this ());
    context.execStack_.push_back (lhs);
}

void EAWhile::eval (Context& context) const {

    const AST::Node* node = EvalApplyNode::getNode();
    const AST::Node* lhs  = (*node)[0];
    
    if (context.prev == lhs) {
        std::shared_ptr<const VarScope> cond = std::static_pointer_cast <const VarScope> (context.calcStack_.back ());
        
        if (cond->getData()) {

            
            // std::shared_ptr<EAWhile> newScope = std::make_shared<EAWhile> (static_cast<const AST::CondNode*>(node)); 
            // context.scopeStack_.push_back (newScope);
            //context.execStack_.push_back (shared_from_this ()); //It's wrong! We need to make a new scope
            context.execStack_.push_back ((*node)[1]);
        }
        context.calcStack_.pop_back ();
        return;
    }

    context.execStack_.StackWrapper::push_back (shared_from_this ());
    context.execStack_.push_back (lhs);
}

void EAVar::eval (Context& context) const {

    const AST::VarNode* varNode = static_cast<const AST::VarNode*> (EvalApplyNode::getNode());
    auto findRes = context.scope_.lookup(varNode->getName());


    std::shared_ptr<const VarScope> var = std::static_pointer_cast<const VarScope> ((*findRes).second);
    context.calcStack_.push_back(var);
} 

void EANum::eval (Context& context) const { 

    context.calcStack_.push_back(std::make_shared<VarScope> (val_));
}


}
