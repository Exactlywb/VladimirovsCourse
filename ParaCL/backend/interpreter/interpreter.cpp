#include "interpreter.hpp"

namespace interpret {
/*
    std::shared_ptr<EvalApplyNode> buildApplyNodeFromCondition (const AST::CondNode* condNode, std::shared_ptr<EvalApplyNode> parent) {

        if (condNode->getConditionType() == AST::CondNode::ConditionType::IF)
            return (std::make_shared<EAIf> (condNode));
                    
        return (std::make_shared<EAWhile> (condNode));
    }*/

    EvalApplyNode* buildApplyNodeFromOperator (const AST::OperNode* opNode, EvalApplyNode* parent) {

        switch (opNode->getOpType ()) {

            case AST::OperNode::OperType::ASSIGN:
                return new EAAssign (opNode, parent);
            // case AST::OperNode::OperType::CALL:
                // return (std::make_shared<EACall> (opNode));
            // case AST::OperNode::OperType::RETURN:
            //     return (std::make_shared<EAUnaryOp<UnaryOpReturn>> (opNode));
            case AST::OperNode::OperType::UNARY_M:
                return new EAUnOp<UnOpMinus> (opNode, parent);
            case AST::OperNode::OperType::UNARY_P: 
                return new EAUnOp<UnOpPlus> (opNode, parent);
            case AST::OperNode::OperType::PRINT: 
                return new EAUnOp<UnOpPrint> (opNode, parent);
            case AST::OperNode::OperType::ADD:
                return new EABinOp<BinOpAdd> (opNode, parent);
            case AST::OperNode::OperType::SUB:
                return new EABinOp<BinOpSub> (opNode, parent);
            case AST::OperNode::OperType::MUL:
                return new EABinOp<BinOpMul> (opNode, parent);
            case AST::OperNode::OperType::DIV:
                return new EABinOp<BinOpDiv> (opNode, parent);
            case AST::OperNode::OperType::MORE:
                return new EABinOp<BinOpMore> (opNode, parent);
            case AST::OperNode::OperType::LESS:
                return new EABinOp<BinOpLess> (opNode, parent);
            case AST::OperNode::OperType::EQ:
                return new EABinOp<BinOpEQ> (opNode, parent);
            case AST::OperNode::OperType::NEQ:
                return new EABinOp<BinOpNEQ> (opNode, parent);
            case AST::OperNode::OperType::GTE:
                return new EABinOp<BinOpGTE> (opNode, parent);
            case AST::OperNode::OperType::LTE:
                return new EABinOp<BinOpLTE> (opNode, parent);
            case AST::OperNode::OperType::OR:
                return new EABinOp<BinOpOr> (opNode, parent);
            case AST::OperNode::OperType::AND:
                return new EABinOp<BinOpAnd> (opNode, parent);
            case AST::OperNode::OperType::MOD: 
                return new EABinOp<BinOpMod>  (opNode, parent);
            // case AST::OperNode::OperType::SCAN: 
                // return (std::make_shared<EAScan> (opNode));
        }

        throw std::runtime_error ("Unexpected operator type");
    }

    EvalApplyNode* buildApplyNodeFromVariable (const AST::VarNode* varNode, EvalApplyNode* parent) { 
        return (new EAVar (varNode, parent));
    }

/*
    std::shared_ptr<EvalApplyNode> buildApplyNodeFromFunction (const AST::FuncNode* funcNode,   std::shared_ptr<EvalApplyNode> parent)       
                                                                { return (std::make_shared<EAFunc> (funcNode, parent));      }
*/
    EvalApplyNode* buildApplyNodeFromNumber   (const AST::NumNode* numNode, EvalApplyNode* parent) { 
        return new EANum (numNode, parent);       
    }

    EvalApplyNode* buildApplyNodeFromScope  (const AST::ScopeNode* scopeNode, EvalApplyNode* parent) { 
        return new EAScope (scopeNode, parent);
    }

    EvalApplyNode* buildApplyNode (const AST::Node* node, EvalApplyNode* parent) {

        switch (node->getType()) {
/*
            case AST::NodeT::CONDITION:
                return buildApplyNodeFromCondition (static_cast<const AST::CondNode*> (node), parent);
            case AST::NodeT::FUNCTION:
                return buildApplyNodeFromFunction (static_cast<const AST::FuncNode*> (node), parent);*/
            case AST::NodeT::OPERATOR:
                return buildApplyNodeFromOperator (static_cast<const AST::OperNode*> (node), parent);
            case AST::NodeT::VARIABLE:
                return buildApplyNodeFromVariable (static_cast<const AST::VarNode*> (node), parent);
            case AST::NodeT::NUMBER:
                return buildApplyNodeFromNumber (static_cast<const AST::NumNode*> (node), parent);
            case AST::NodeT::SCOPE:
                return buildApplyNodeFromScope (static_cast<const AST::ScopeNode*> (node), parent);
        }
        
        throw std::runtime_error ("unexpected AST::Node type");
    }

ScopeTblWrapper::~ScopeTblWrapper () = default;

void Context::replaceScope (Scope* scope, const EAScope* curEAScope) {

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

void Context::addScope ()
{
    if (scopeStack_.empty()) {
        scopeStack_.push_back (new Scope);
        return;
    }

    auto parent = scopeStack_.back();
    bool noParent = true;

    for (auto curIt = scopeStack_.rbegin(), endIt = scopeStack_.rend (); curIt != endIt; ++curIt) {
        if ((*curIt)->tbl_empty())
            continue;

        noParent = false;
        parent = *curIt;
    }

    if (noParent)
        scopeStack_.push_back (new Scope);
    else 
        scopeStack_.push_back (new Scope {parent, --(parent->tblEnd())});
}

EAScope::EAScope (const AST::ScopeNode* astScope, EvalApplyNode* parent): EvalApplyNode (astScope, parent) {

    auto st  = getNode()->childBegin();
    auto fin = getNode()->childEnd();
    for (; st != fin; ++st) 
        children_.push_back(buildApplyNode(*st, this));

}

EAAssign::EAAssign (const AST::OperNode* astAssign, EvalApplyNode* parent): EvalApplyNode (astAssign, parent) {

    const AST::VarNode* lhsID = static_cast<const AST::VarNode*>((*astAssign)[0]);

    lhs_ = lhsID->getName ();
    rhs_ = buildApplyNode ((*astAssign)[1], this);

}

void Interpreter::run () {

    Context context;
    context.addScope ();

    EvalApplyNode* expr = buildApplyNode (root_, nullptr);

    while (expr) {
        // std::cout << "here" << std::endl;
        //next to implement and prev
        std::pair<EvalApplyNode*, EvalApplyNode*> res = expr->eval (context);

        context.prev_ = res.second;
        
        auto pred = res.first;
        if (pred)
            expr = pred;
        else
            break;

    }

}

std::pair<EvalApplyNode*, EvalApplyNode*> EAScope::eval (Context& context)
{
    const AST::ScopeNode* scope = static_cast<const AST::ScopeNode*> (getNode ());
    if (scope->getLeftChild ()) {
        
        EvalApplyNode* next = nullptr;
        if (curChildrenToExec_ == children_.size ())
            next = parent_;
        else
            next = children_ [curChildrenToExec_++];
        
        return {next, this};

    }

    auto nextToExec = context.retStack_.back ();
    return {nextToExec, this};
}

std::pair<EvalApplyNode*, EvalApplyNode*> EAAssign::eval (Context& context)
{
    if (context.prev_ == rhs_) {

        auto next = parent_;
        auto res = context.calcStack_.back ();

        Scope* curScopeToFind = context.scopeStack_.back ();
        auto find = curScopeToFind->lookup (lhs_);
        if (find == curScopeToFind->tblEnd ()) {

            if (res->type_ == ScopeTblWrapper::WrapperType::NUM) 
                curScopeToFind->push({lhs_, res});
            else {
                
                FuncScope* funcToPush = static_cast<FuncScope*> (res);
                const AST::FuncNode* predFuncName = funcToPush->name_;
                if (predFuncName) {

                    const AST::VarNode* getFuncName = static_cast<const AST::VarNode*> ((*predFuncName) [0]);
                    curScopeToFind->push ({getFuncName->getName (), funcToPush});
                }

                curScopeToFind->push ({lhs_, funcToPush});

            }

        } 
        else {

            auto val = (*find).second;
            NumScope* curNum = static_cast<NumScope*> (val);
            curNum->val_ = static_cast<NumScope*>(res)->val_;
            
        }

        return {next, this};
        
    }

    return {rhs_, this};

}

std::pair<EvalApplyNode*, EvalApplyNode*> EANum::eval (Context& context)
{
    context.calcStack_.push_back (new NumScope (val_));
    return {parent_, this};

}

std::pair<EvalApplyNode*, EvalApplyNode*> EAVar::eval (Context& context)
{ 
    auto curScope = context.scopeStack_.back();

    auto findRes = curScope->lookup(name_);

    context.calcStack_.push_back (findRes->second);

    return {parent_, this};
}

}
