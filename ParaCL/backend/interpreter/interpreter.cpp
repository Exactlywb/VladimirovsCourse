#include "interpreter.hpp"

namespace interpret {

    EvalApplyNode *buildApplyNodeFromCondition (const AST::CondNode *condNode, EvalApplyNode *parent)
    {
        if (condNode->getConditionType () == AST::CondNode::ConditionType::IF)
            return new EAIf (condNode, parent);

        return new EAWhile (condNode, parent);
    }

    EvalApplyNode *buildApplyNodeFromOperator (const AST::OperNode *opNode, EvalApplyNode *parent)
    {
        switch (opNode->getOpType ()) {
            case AST::OperNode::OperType::ASSIGN: return new EAAssign (opNode, parent);
            case AST::OperNode::OperType::CALL: return new EACall (opNode, parent);
            case AST::OperNode::OperType::RETURN: return new EAReturn (opNode, parent);
            case AST::OperNode::OperType::UNARY_M: return new EAUnOp<UnOpMinus> (opNode, parent);
            case AST::OperNode::OperType::UNARY_P: return new EAUnOp<UnOpPlus> (opNode, parent);
            case AST::OperNode::OperType::PRINT: return new EAUnOp<UnOpPrint> (opNode, parent);
            case AST::OperNode::OperType::ADD: return new EABinOp<BinOpAdd> (opNode, parent);
            case AST::OperNode::OperType::SUB: return new EABinOp<BinOpSub> (opNode, parent);
            case AST::OperNode::OperType::MUL: return new EABinOp<BinOpMul> (opNode, parent);
            case AST::OperNode::OperType::DIV: return new EABinOp<BinOpDiv> (opNode, parent);
            case AST::OperNode::OperType::MORE: return new EABinOp<BinOpMore> (opNode, parent);
            case AST::OperNode::OperType::LESS: return new EABinOp<BinOpLess> (opNode, parent);
            case AST::OperNode::OperType::EQ: return new EABinOp<BinOpEQ> (opNode, parent);
            case AST::OperNode::OperType::NEQ: return new EABinOp<BinOpNEQ> (opNode, parent);
            case AST::OperNode::OperType::GTE: return new EABinOp<BinOpGTE> (opNode, parent);
            case AST::OperNode::OperType::LTE: return new EABinOp<BinOpLTE> (opNode, parent);
            case AST::OperNode::OperType::OR: return new EABinOp<BinOpOr> (opNode, parent);
            case AST::OperNode::OperType::AND: return new EABinOp<BinOpAnd> (opNode, parent);
            case AST::OperNode::OperType::MOD: return new EABinOp<BinOpMod> (opNode, parent);
            case AST::OperNode::OperType::SCAN: return new EAScan (opNode, parent);
        }

        throw std::runtime_error ("Unexpected operator type");
    }

    EvalApplyNode *buildApplyNodeFromVariable (const AST::VarNode *varNode, EvalApplyNode *parent) { return (new EAVar (varNode, parent)); }

    EvalApplyNode * buildApplyNodeFromFunction (const AST::FuncNode* funcNode, EvalApplyNode *parent) { return (new EAFunc (funcNode, parent));      }

    EvalApplyNode *buildApplyNodeFromNumber (const AST::NumNode *numNode, EvalApplyNode *parent) { return new EANum (numNode, parent); }

    EvalApplyNode *buildApplyNodeFromScope (const AST::ScopeNode *scopeNode, EvalApplyNode *parent) { return new EAScope (scopeNode, parent); }

    EvalApplyNode *buildApplyNode (const AST::Node *node, EvalApplyNode *parent)
    {
        switch (node->getType ()) {
            case AST::NodeT::CONDITION: return buildApplyNodeFromCondition (static_cast<const AST::CondNode *> (node), parent);
            case AST::NodeT::FUNCTION: return buildApplyNodeFromFunction (static_cast<const AST::FuncNode*> (node), parent);
            case AST::NodeT::OPERATOR: return buildApplyNodeFromOperator (static_cast<const AST::OperNode *> (node), parent);
            case AST::NodeT::VARIABLE: return buildApplyNodeFromVariable (static_cast<const AST::VarNode *> (node), parent);
            case AST::NodeT::NUMBER: return buildApplyNodeFromNumber (static_cast<const AST::NumNode *> (node), parent);
            case AST::NodeT::SCOPE: return buildApplyNodeFromScope (static_cast<const AST::ScopeNode *> (node), parent);
        }

        throw std::runtime_error ("unexpected AST::Node type");
    }

    const NumScope *getTopAndPopNum (Context &context)
    {
        const NumScope *res = static_cast<NumScope *> (context.calcStack_.back ());
        context.calcStack_.pop_back ();
        return res;
    }

    ScopeTblWrapper::~ScopeTblWrapper () = default;

    void Context::removeCurScope ()
    {
        if (scopeStack_.empty ())
            return;

        scopeStack_.pop_back ();
    }

    void Context::addScope ()
    {
        if (scopeStack_.empty ()) {
            scopeStack_.push_back (new Scope);
            return;
        }

        auto parent = scopeStack_.back ();
        bool noParent = true;

        for (auto curIt = scopeStack_.rbegin (), endIt = scopeStack_.rend (); curIt != endIt; ++curIt)
            if (!(*curIt)->tbl_empty ()) {
                noParent = false;
                parent = *curIt;
                break;
            }

        if (noParent)
            scopeStack_.push_back (new Scope);
        else
            scopeStack_.push_back (new Scope{parent, --(parent->tblEnd ())});
    }

    EAScope::EAScope (const AST::ScopeNode *astScope, EvalApplyNode *parent) : EvalApplyNode (astScope, parent)
    {
        auto st = getNode ()->childBegin ();
        auto fin = getNode ()->childEnd ();
        for (; st != fin; ++st) {
            EvalApplyNode *toPush = buildApplyNode (*st, this);
            children_.push_back (toPush);
        }
    }

    EAAssign::EAAssign (const AST::OperNode *astAssign, EvalApplyNode *parent) : EvalApplyNode (astAssign, parent)
    {
        const AST::VarNode *lhsID = static_cast<const AST::VarNode *> ((*astAssign)[0]);

        lhs_ = lhsID->getName ();
        rhs_ = buildApplyNode ((*astAssign)[1], this);
    }

    void Interpreter::run ()
    {
        Context context;
        context.addScope ();

        EvalApplyNode *expr = buildApplyNode (root_, nullptr);

        while (expr) {
            // std::cout << "here " << std::endl;
            // next to implement and prev
            std::pair<EvalApplyNode *, EvalApplyNode *> res = expr->eval (context);

            context.prev_ = res.second;

            auto pred = res.first;
            if (pred)
                expr = pred;
            else
                break;
        }
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAScope::eval (Context &context)
    {
        const AST::ScopeNode *scope = static_cast<const AST::ScopeNode *> (getNode ());
        if (scope->getLeftChild ()) {
            EvalApplyNode *next = nullptr;
            if (curChildrenToExec_ == children_.size ()) {
                context.removeCurScope ();
                next = parent_;
            }
            else
                next = children_[curChildrenToExec_++];

            return {next, this};
        }

        auto nextToExec = context.retStack_.back ();
        
        return {nextToExec.second, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAAssign::eval (Context &context)
    {
        if (context.prev_ == rhs_) {
            std::cout << "second assign " << std::endl;

            auto next = parent_;
            auto res = context.calcStack_.back ();

            Scope *curScopeToFind = context.scopeStack_.back ();
            auto find = curScopeToFind->lookup (lhs_).second;
            if (find == curScopeToFind->tblEnd ()) {
                if (res->type_ == ScopeTblWrapper::WrapperType::NUM)
                    curScopeToFind->push ({lhs_, new NumScope (static_cast<NumScope *> (res)->val_)});
                else {
                    std::cout << "find func " << std::endl;
                    FuncScope *funcToPush = static_cast<FuncScope *> (res);
                    const AST::FuncNode *predFuncName = funcToPush->name_;
                    if (predFuncName) {
                        const AST::VarNode *getFuncName = static_cast<const AST::VarNode *> ((*predFuncName)[0]);
                        curScopeToFind->push ({getFuncName->getName (), funcToPush});
                        
                    } 
                    
                    curScopeToFind->push ({lhs_, funcToPush});
                }
            }
            else {
                auto val = (*find).second;
                NumScope *curNum = static_cast<NumScope *> (val);
                curNum->val_ = static_cast<NumScope *> (res)->val_;
            }

            return {next, this};
        }
        std::cout << "first assign " << std::endl;

        return {rhs_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EANum::eval (Context &context)
    {
        context.calcStack_.push_back (new NumScope (val_));
        return {parent_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAVar::eval (Context &context)
    {
        auto curScope = context.scopeStack_.back ();

        auto findRes = curScope->lookup (name_).second;

        context.calcStack_.push_back (findRes->second);
        return {parent_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAFunc::eval (Context &context)
    {
        std::cout << "EAFunc" << std::endl;
        auto curScope = context.scopeStack_.back ();

        context.calcStack_.push_back (new FuncScope (static_cast <const AST::FuncNode *> (EvalApplyNode::getNode())));
        return {parent_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAIf::eval (Context &context)
    {
        const AST::Node *prevASTExecuted = context.prev_->getNode ();
        if (prevASTExecuted == parent_->getNode ())
            return {buildApplyNode (expr_, this), this};

        if (prevASTExecuted == expr_) {
            const NumScope *boolRes = getTopAndPopNum (context);
            if (boolRes->val_) {
                context.addScope ();
                return {buildApplyNode (scope_, parent_), this};  //! TODO think about parent
            }
        }

        return {parent_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAWhile::eval (Context &context)
    {
        const AST::Node *prevASTExecuted = context.prev_->getNode ();
        if (prevASTExecuted == parent_->getNode ())
            return {buildApplyNode (expr_, this), this};

        if (prevASTExecuted == expr_) {
            const NumScope *boolRes = getTopAndPopNum (context);
            if (boolRes->val_) {
                context.addScope ();
                return {buildApplyNode (scope_, this), this};  //! TODO think about parent
            }
        }

        if (prevASTExecuted == scope_)
            return {buildApplyNode (expr_, this), this};

        return {parent_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EACall::eval (Context &context)
    {
        const AST::Node *node = EvalApplyNode::getNode ();

        if (!context.retStack_.empty() && context.retStack_.back ().second == this) {
            context.retStack_.pop_back ();
            return {parent_, this};
        }

        context.retStack_.push_back ({context.scopeStack_.size(), this});
        std::cout << "in call " << context.retStack_.size() << std::endl;

        const AST::VarNode *nameNode = static_cast<const AST::VarNode *> ((*node)[0]);
        std::cout << "in call" << std::endl;
        auto funcInfo = context.scopeStack_.back()->lookup(nameNode->getName());
        auto funcNode = static_cast<FuncScope *> (funcInfo.second->second);
        std::cout << "in call after " << funcNode << std::endl;
        auto beginNameArgs = funcNode->args_->childBegin();
        auto funcScope = new Scope (funcInfo.first, funcInfo.second);

        for (auto begin = ((*node)[1])->childBegin(), end = ((*node)[1])->childEnd(); begin != end; ++begin) {  //TODO: we want calculate args
            auto varName = static_cast<const AST::VarNode *> (*beginNameArgs)->getName();
            auto valueNode = new NumScope (static_cast<AST::NumNode *> (*begin)->getValue());
            funcScope->push({varName, valueNode});
        }

        if (funcNode->name_) {
            std::string name = static_cast<AST::VarNode *> (*(funcNode->name_->childBegin()))->getName();
            // std::cout << name << std::endl;
            funcScope->push({name, funcNode});
        }

        context.scopeStack_.push_back (funcScope);

        const AST::ScopeNode *scope = (static_cast<FuncScope *> (funcInfo.second->second))->execScope_;

        return {new EAScope (scope, parent_), this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAReturn::eval (Context &context)
    {
        std::cout << "in return " << std::endl;

        const AST::Node *node = EvalApplyNode::getNode ();
        const AST::Node *lhs = (*node)[0];

        const AST::Node *prevExec = context.prev_->getNode ();

        if (prevExec == lhs) {
            auto retAddress = context.retStack_.back ();
            context.scopeStack_.erase (context.scopeStack_.begin() + retAddress.first, context.scopeStack_.end());

            return {retAddress.second, this};
        }

        EvalApplyNode *lhsToExec = buildApplyNode (lhs, this);
        return {lhsToExec, this};
    }

}  // namespace interpret
