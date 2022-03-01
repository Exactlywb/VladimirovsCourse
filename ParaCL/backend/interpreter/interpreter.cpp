#include "interpreter.hpp"

namespace interpret {

    namespace {

        EvalApplyNode *buildApplyNodeFromCondition (const AST::CondNode *condNode, EvalApplyNode *parent)
        {
            if (condNode->getConditionType () == AST::CondNode::ConditionType::IF)
                return new EAIf (condNode, parent);

            return new EAWhile (condNode, parent);
        }

        EvalApplyNode *buildApplyNodeFromOperator (const AST::OperNode *opNode, EvalApplyNode *parent, Context &context)
        {
            switch (opNode->getOpType ()) {
                case AST::OperNode::OperType::ASSIGN: return new EAAssign (opNode, parent, context);
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

        EvalApplyNode *buildApplyNodeFromVariable (const AST::VarNode *varNode, EvalApplyNode *parent)
        {
            return (new EAVar (varNode, parent));
        }

        EvalApplyNode *buildApplyNodeFromFunction (const AST::FuncNode *funcNode, EvalApplyNode *parent)
        {
            return (new EAFunc (funcNode, parent));
        }

        EvalApplyNode *buildApplyNodeFromNumber (const AST::NumNode *numNode, EvalApplyNode *parent)
        {
            return new EANum (numNode, parent);
        }

        EvalApplyNode *buildApplyNodeFromScope (const AST::ScopeNode *scopeNode,
                                                EvalApplyNode *parent,
                                                Context &context)
        {
            return new EAScope (scopeNode, parent, context);
        }

        EvalApplyNode *buildApplyNodeFromInlineScope (const AST::InlineScopeNode *inlineScopeNode,
                                                      EvalApplyNode *parent)
        {
            return new EAInlineScope (inlineScopeNode, parent);
        }
    }  // namespace

    EvalApplyNode *Context::buildApplyNode (const AST::Node *node, EvalApplyNode *parent)
    {
        switch (node->getType ()) {
            case AST::NodeT::CONDITION:
                rubbishEANodeStack_.push_back (
                    buildApplyNodeFromCondition (static_cast<const AST::CondNode *> (node), parent));
                return rubbishEANodeStack_.back ();
            case AST::NodeT::FUNCTION:
                rubbishEANodeStack_.push_back (
                    buildApplyNodeFromFunction (static_cast<const AST::FuncNode *> (node), parent));
                return rubbishEANodeStack_.back ();
            case AST::NodeT::OPERATOR:
                rubbishEANodeStack_.push_back (
                    buildApplyNodeFromOperator (static_cast<const AST::OperNode *> (node), parent, *this));
                return rubbishEANodeStack_.back ();
            case AST::NodeT::VARIABLE:
                rubbishEANodeStack_.push_back (
                    buildApplyNodeFromVariable (static_cast<const AST::VarNode *> (node), parent));
                return rubbishEANodeStack_.back ();
            case AST::NodeT::NUMBER:
                rubbishEANodeStack_.push_back (
                    buildApplyNodeFromNumber (static_cast<const AST::NumNode *> (node), parent));
                return rubbishEANodeStack_.back ();
            case AST::NodeT::SCOPE:
                rubbishEANodeStack_.push_back (
                    buildApplyNodeFromScope (static_cast<const AST::ScopeNode *> (node), parent, *this));
                return rubbishEANodeStack_.back ();
            case AST::NodeT::INLINESCOPE:
                rubbishEANodeStack_.push_back (
                    buildApplyNodeFromInlineScope (static_cast<const AST::InlineScopeNode *> (node), parent));
                return rubbishEANodeStack_.back ();
        }

        throw std::runtime_error ("unexpected AST::Node type");
    }

    int getTopAndPopNum (Context &context)
    {
        NumScope *res = static_cast<NumScope *> (context.calcStack_.back ());
        context.calcStack_.pop_back ();

        int val = res->val_;
        return val;
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
            scopeStack_.push_back (buildScope ());
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
            scopeStack_.push_back (buildScope ());
        else
            scopeStack_.push_back (buildScope (parent, --(parent->tblEnd ())));
    }

    EAScope::EAScope (const AST::ScopeNode *astScope, EvalApplyNode *parent, Context &context)
        : EvalApplyNode (astScope, parent)
    {
        auto st = getNode ()->childBegin ();
        auto fin = getNode ()->childEnd ();
        for (; st != fin; ++st) {
            EvalApplyNode *toPush = context.buildApplyNode (*st, this);
            children_.push_back (toPush);
        }
    }

    EAAssign::EAAssign (const AST::OperNode *astAssign, EvalApplyNode *parent, Context &context)
        : EvalApplyNode (astAssign, parent)
    {
        const AST::VarNode *lhsID = static_cast<const AST::VarNode *> ((*astAssign)[0]);

        lhs_ = lhsID->getName ();
        rhs_ = context.buildApplyNode ((*astAssign)[1], this);
    }

    void Interpreter::run ()
    {
        Context context;
        context.addScope ();

        EvalApplyNode *expr = context.buildApplyNode (root_, nullptr);
        while (expr) {
            // next to implement and prev
            std::pair<EvalApplyNode *, EvalApplyNode *> res = expr->eval (context);

            context.prev_ = res.second;

            auto pred = res.first;
            if (pred) {
                expr = pred;
            }
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

    namespace {

        void ChangeExistedVarValue (Scope::tblIt find, const ScopeTblWrapper *res)
        {
            auto val = (*find).second;
            NumScope *curNum = static_cast<NumScope *> (val);
            curNum->val_ = static_cast<const NumScope *> (res)->val_;
        }

        void PushNewVarIntoScopes (Context &context,
                                   ScopeTblWrapper *res,
                                   Scope *curScopeToFind,
                                   const std::string &lhs_)
        {
            if (res->type_ == ScopeTblWrapper::WrapperType::NUM)
                curScopeToFind->push ({lhs_, context.buildScopeWrapper (static_cast<NumScope *> (res)->val_)});
            else {
                FuncScope *funcToPush = static_cast<FuncScope *> (res);
                const AST::FuncNode *predFuncName = funcToPush->name_;
                if (predFuncName) {
                    const AST::VarNode *getFuncName = static_cast<const AST::VarNode *> ((*predFuncName)[0]);
                    curScopeToFind->push ({getFuncName->getName (), funcToPush});
                }

                curScopeToFind->push ({lhs_, funcToPush});
            }
        }
    }  // namespace

    std::pair<EvalApplyNode *, EvalApplyNode *> EAAssign::eval (Context &context)
    {
        if (context.prev_ == rhs_) {
            auto next = parent_;
            auto res = context.calcStack_.back ();
            if (parent_->getNode ()->getType () == AST::NodeT::SCOPE)
                context.calcStack_.pop_back ();

            Scope *curScopeToFind = context.scopeStack_.back ();
            auto find = curScopeToFind->lookup (lhs_).second;
            if (find == curScopeToFind->tblEnd ())
                PushNewVarIntoScopes (context, res, curScopeToFind, lhs_);
            else
                ChangeExistedVarValue (find, res);

            return {next, this};
        }

        return {rhs_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EANum::eval (Context &context)
    {
        context.calcStack_.push_back (context.buildScopeWrapper (val_));
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
        auto curScope = context.scopeStack_.back ();

        context.calcStack_.push_back (
            context.buildScopeWrapper (static_cast<const AST::FuncNode *> (EvalApplyNode::getNode ())));
        return {parent_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAIf::eval (Context &context)
    {
        const AST::Node *prevASTExecuted = context.prev_->getNode ();
        if (prevASTExecuted == parent_->getNode ())
            return {context.buildApplyNode (expr_, this), this};

        if (prevASTExecuted == expr_) {
            if (getTopAndPopNum (context)) {
                context.addScope ();
                return {context.buildApplyNode (scope_, parent_), this};
            }
        }

        return {parent_, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAWhile::eval (Context &context)
    {
        const AST::Node *prevASTExecuted = context.prev_->getNode ();
        if (prevASTExecuted == parent_->getNode ())
            return {context.buildApplyNode (expr_, this), this};

        if (prevASTExecuted == expr_) {
            if (getTopAndPopNum (context)) {
                context.addScope ();
                return {context.buildApplyNode (scope_, this), this};
            }
        }

        if (prevASTExecuted == scope_)
            return {context.buildApplyNode (expr_, this), this};

        return {parent_, this};
    }

    namespace {

        void FillFunctionWithArguments (Scope *funcScope,
                                        std::vector<ScopeTblWrapper *> &calcStack,
                                        const AST::FuncNode *args,
                                        Scope::tblIt funcFoundIt)
        {
            auto funcNode = static_cast<const FuncScope *> (funcFoundIt->second);
            auto beginNameArgs = funcNode->args_->childEnd ();

            for (int curChild = 0; curChild < args->getChildrenNum (); ++curChild) {
                auto varName = static_cast<const AST::VarNode *> (*(--beginNameArgs))->getName ();
                NumScope *valueNode = static_cast<NumScope *> (calcStack.back ());
                calcStack.pop_back ();
                funcScope->push ({varName, valueNode});
            }
        }

    }  // namespace

    std::pair<EvalApplyNode *, EvalApplyNode *> EACall::eval (Context &context)
    {
        const AST::Node *node = EvalApplyNode::getNode ();
        const AST::VarNode *nameNode = static_cast<const AST::VarNode *> ((*node)[0]);
        const AST::FuncNode *args = static_cast<const AST::FuncNode *> ((*node)[1]);

        std::vector<std::pair<int, EvalApplyNode *>> &retStack = context.retStack_;

        if (!retStack.empty () && retStack.back ().second == this) {
            retStack.pop_back ();
            return {parent_, this};
        }

        if (curArgsToCalc_ != args->getChildrenNum ())
            return {context.buildApplyNode ((*args)[curArgsToCalc_++], this), this};

        std::vector<Scope *> &scopeStack = context.scopeStack_;

        retStack.push_back ({scopeStack.size (), this});

        auto funcInfo = scopeStack.back ()->lookup (nameNode->getName ());
        auto funcFoundIt = funcInfo.second;

        auto funcScope = context.buildScope (funcInfo.first, funcFoundIt);
        FillFunctionWithArguments (funcScope, context.calcStack_, args, funcFoundIt);

        scopeStack.push_back (funcScope);
        const AST::ScopeNode *scope = (static_cast<const FuncScope *> (funcFoundIt->second))->execScope_;

        return {context.buildApplyNode (scope, parent_), this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAReturn::eval (Context &context)
    {
        const AST::Node *node = EvalApplyNode::getNode ();
        const AST::Node *lhs = nullptr;
        
        if (node->getChildrenNum())
            lhs = (*node)[0];

        const AST::Node *prevExec = context.prev_->getNode ();
        if (lhs == nullptr || prevExec == lhs) {
            auto retAddress = context.retStack_.back ();
            std::vector<Scope *> &scopeStack = context.scopeStack_;
            scopeStack.erase (scopeStack.begin () + retAddress.first, scopeStack.end ());

            return {retAddress.second, this};
        }

        EvalApplyNode *lhsToExec = context.buildApplyNode (lhs, this);
        return {lhsToExec, this};
    }

    std::pair<EvalApplyNode *, EvalApplyNode *> EAInlineScope::eval (Context &context)
    {
        const AST::Node *node = EvalApplyNode::getNode ();
        const AST::ScopeNode *scopeNode = static_cast<const AST::ScopeNode *> ((*node)[0]);

        std::vector<std::pair<int, EvalApplyNode *>> &retStack = context.retStack_;
        if (!retStack.empty () && retStack.back ().second == this) {
            retStack.pop_back ();
            return {parent_, this};
        }

        retStack.push_back ({context.scopeStack_.size (), this});
        context.addScope ();

        return {context.buildApplyNode (scopeNode, parent_), this};
    }

}  // namespace interpret
