#include "interpreter.hpp"

namespace interpret {

namespace {
/*
        std::shared_ptr<EvalApplyNode> buildApplyNodeFromCondition (const AST::CondNode* condNode, std::shared_ptr<EvalApplyNode> parent) {

            if (condNode->getConditionType() == AST::CondNode::ConditionType::IF)
                return (std::make_shared<EAIf> (condNode));
                        
            return (std::make_shared<EAWhile> (condNode));
        }

        std::shared_ptr<EvalApplyNode> buildApplyNodeFromOperator (const AST::OperNode* opNode, std::shared_ptr<EvalApplyNode> parent) {

            switch (opNode->getOpType ()) {

                case AST::OperNode::OperType::ASSIGN:
                    return (std::make_shared<EAAssign> (opNode));
                case AST::OperNode::OperType::CALL:
                    return (std::make_shared<EACall> (opNode));
                // case AST::OperNode::OperType::RETURN:
                //     return (std::make_shared<EAUnaryOp<UnaryOpReturn>> (opNode));
                // case AST::OperNode::OperType::UNARY_M:
                //     return (std::make_shared<EAUnaryOp<UnaryOpMinus>> (opNode));
                // case AST::OperNode::OperType::UNARY_P: 
                //     return (std::make_shared<EAUnaryOp<UnaryOpPlus>> (opNode));
                // case AST::OperNode::OperType::PRINT: 
                //     return (std::make_shared<EAUnaryOp<UnaryOpPrint>> (opNode));
                // case AST::OperNode::OperType::ADD:
                //     return std::make_shared<EABinOp<BinOpAdd>> (opNode);
                // case AST::OperNode::OperType::SUB:
                //     return std::make_shared<EABinOp<BinOpSub>> (opNode);
                // case AST::OperNode::OperType::MUL:
                //     return std::make_shared<EABinOp<BinOpMul>> (opNode);
                // case AST::OperNode::OperType::DIV:
                //     return std::make_shared<EABinOp<BinOpDiv>> (opNode);
                // case AST::OperNode::OperType::MORE:
                //     return std::make_shared<EABinOp<BinOpMore>> (opNode);
                // case AST::OperNode::OperType::LESS:
                //     return std::make_shared<EABinOp<BinOpLess>> (opNode);
                // case AST::OperNode::OperType::EQ:
                //     return std::make_shared<EABinOp<BinOpEQ>> (opNode);
                // case AST::OperNode::OperType::NEQ:
                //     return std::make_shared<EABinOp<BinOpNEQ>> (opNode);
                // case AST::OperNode::OperType::GTE:
                //     return std::make_shared<EABinOp<BinOpGTE>> (opNode);
                // case AST::OperNode::OperType::LTE:
                //     return std::make_shared<EABinOp<BinOpLTE>> (opNode);
                // case AST::OperNode::OperType::OR:
                //     return std::make_shared<EABinOp<BinOpOr>> (opNode);
                // case AST::OperNode::OperType::AND:
                //     return std::make_shared<EABinOp<BinOpAnd>> (opNode);
                // case AST::OperNode::OperType::MOD: 
                //     return (std::make_shared<EABinOp<BinOpMod>>  (opNode));
                case AST::OperNode::OperType::SCAN: 
                    return (std::make_shared<EAScan> (opNode));
            }

            throw std::runtime_error ("Unexpected operator type");
        }
/*
        std::shared_ptr<EvalApplyNode> buildApplyNodeFromVariable (const AST::VarNode*  varNode,    std::shared_ptr<EvalApplyNode> parent)        
                                                                  { return (std::make_shared<EAVar> (varNode, parent));        }

        std::shared_ptr<EvalApplyNode> buildApplyNodeFromFunction (const AST::FuncNode* funcNode,   std::shared_ptr<EvalApplyNode> parent)       
                                                                  { return (std::make_shared<EAFunc> (funcNode, parent));      }

        std::shared_ptr<EvalApplyNode> buildApplyNodeFromNumber   (const AST::NumNode* numNode,     std::shared_ptr<EvalApplyNode> parent)        
                                                                  { return (std::make_shared<EANum>  (numNode, parent));       }
                                                                  */
        std::shared_ptr<EvalApplyNode> buildApplyNodeFromScope    (const AST::ScopeNode* scopeNode, std::shared_ptr<EvalApplyNode> parent)    
                                                                  { return (std::make_shared<EAScope> (scopeNode, parent));    }

        std::shared_ptr<EvalApplyNode> buildApplyNode (const AST::Node* node, std::shared_ptr<EvalApplyNode> parent) {

            switch (node->getType()) {
/*
                case AST::NodeT::CONDITION:
                    return buildApplyNodeFromCondition (static_cast<const AST::CondNode*> (node), parent);
                case AST::NodeT::FUNCTION:
                    return buildApplyNodeFromFunction (static_cast<const AST::FuncNode*> (node), parent);
                case AST::NodeT::OPERATOR:
                    return buildApplyNodeFromOperator (static_cast<const AST::OperNode*> (node), parent);
                case AST::NodeT::VARIABLE:
                    return buildApplyNodeFromVariable (static_cast<const AST::VarNode*> (node), parent);
                case AST::NodeT::NUMBER:
                    return buildApplyNodeFromNumber (static_cast<const AST::NumNode*> (node), parent);
                    */
                case AST::NodeT::SCOPE:
                    return buildApplyNodeFromScope (static_cast<const AST::ScopeNode*> (node), parent);
            }
            
            throw std::runtime_error ("unexpected AST::Node type");
        }

}

ScopeWrapper::~ScopeWrapper () = default;

void Context::replaceScope (std::shared_ptr<Scope> scope, std::shared_ptr<const EAScope> curEAScope) {

    // if (scope_ != nullptr) {
    //     scopeStack_.push_back (scope_);
    // }

    // if (scope)
    //     scope_ = scope;
    // else
    //     scope_ = std::shared_ptr<Scope>(new Scope);
    
    // curEAScope_ = curEAScope;
}

void Context::removeCurScope () {
    // if (scopeStack_.empty ())
    //     return;

    // scope_ = scopeStack_.back ();
    // scopeStack_.pop_back ();

}

EAScope::EAScope (const AST::ScopeNode* astScope, std::shared_ptr<EvalApplyNode> parent): EvalApplyNode (astScope, parent) {

    auto st  = getNode()->childBegin();
    auto fin = getNode()->childEnd();

    for (; st != fin; ++st) 
        children_.push_back(buildApplyNode(*st, shared_from_this()));

}

EAAssign::EAAssign (const AST::OperNode* astAssign, std::shared_ptr<EvalApplyNode> parent): EvalApplyNode (astAssign, parent) {

    const AST::VarNode* lhsID = static_cast<const AST::VarNode*>((*astAssign)[0]);

    lhs_ = std::make_shared<IDScope> (lhsID->getName ());
    rhs_ = buildApplyNode ((*astAssign)[1], shared_from_this ());

}

void Interpreter::run () {

    Context context;
    std::shared_ptr<EvalApplyNode> globalScope = buildApplyNode (root_, nullptr);
    context.scopeStack_.push_back (std::make_shared<Scope> ());

    std::shared_ptr<EvalApplyNode> expr = globalScope;

    while (expr) {

        //next to implement and prev
        std::pair<std::shared_ptr<EvalApplyNode>, std::shared_ptr<EvalApplyNode>> res = expr->eval (context);

        context.prev_ = res.second;
        
        auto pred = res.first;
        if (pred->getNode ())
            expr = res.first;
        else
            break;

    }

}

std::pair<std::shared_ptr<EvalApplyNode>, std::shared_ptr<EvalApplyNode>> EAScope::eval (Context& context) {

    const AST::ScopeNode* scope = static_cast<const AST::ScopeNode*> (getNode ());
    if (scope->getLeftChild ()) {
        
        std::shared_ptr<EvalApplyNode> next;
        if (curChildrenToExec_ == children_.size ())
            next = parent_;
        else
            next = children_ [curChildrenToExec_++];
        
        return {next, shared_from_this ()};

    }

    auto nextToExec = context.retStack_.back ();
    return {nextToExec, shared_from_this ()};
}

std::pair<std::shared_ptr<EvalApplyNode>, std::shared_ptr<EvalApplyNode>> EAAssign::eval (Context& context) {

    if (context.prev_ == rhs_) {

        auto next = parent_;
        auto res = context.calcStack_.back ();

        std::shared_ptr<Scope> curScopeToFind = context.scopeStack_.back ();
        auto find = curScopeToFind->lookup (lhs_->name_);
        if (find == curScopeToFind->tblEnd ()) {

            if (res->type_ == ScopeWrapper::WrapperType::NUM) 
                curScopeToFind->push({lhs_->name_, res});
            else {
                
                std::shared_ptr<FuncScope> funcToPush = std::static_pointer_cast<FuncScope> (res);
                const AST::FuncNode* predFuncName = funcToPush->name_;
                if (predFuncName) {

                    const AST::VarNode* getFuncName = static_cast<const AST::VarNode*> ((*predFuncName) [0]);
                    curScopeToFind->push ({getFuncName->getName (), funcToPush});
                }

                curScopeToFind->push ({lhs_->name_, funcToPush});

            }

        } 
        else {

            auto val = (*find).second;
            std::shared_ptr<NumScope> curNum = std::static_pointer_cast<NumScope> (val);
            curNum->val_ = std::static_pointer_cast<NumScope> (res)->val_;
            
        }

        return {next, shared_from_this ()};
        
    }

    return {rhs_, shared_from_this ()};

}

}
