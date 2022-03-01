#include "semantic.hpp"

using tblIt = std::unordered_map<std::string, TypeWrapper *>::iterator;

void SemanticAnalyzer::run (Tree::NAryTree<AST::Node *> *tree,
                            const std::function<void (yy::location, const std::string &)> pushWarning,
                            const std::function<void (yy::location, const std::string &)> pushError)
{
    pushWarning_ = pushWarning;
    pushError_ = pushError;
    
    AnalyzeHiddenReturn (tree);

    AST::ScopeNode *startScope = static_cast<AST::ScopeNode *> (tree->getRoot ());
    if (startScope)
        AnalyzeScopes (globalScope_->getRoot (), startScope);

    AnalyzeReturnPosition (tree);
}

//===========================================================================================
//=================================HIDDEN RETURN ANALYZING===================================
//===========================================================================================

namespace {

    void SetHiddenReturn (AST::Node *curNode, AST::Node *oper)
    {
        AST::OperNode *returnNode = new AST::OperNode (AST::OperNode::OperType::RETURN);
        returnNode->addChild (oper);
        curNode->eraseChild (curNode->getChildrenNum () - 1);
        curNode->addChild (returnNode);
    }

    void SetFillerReturn (AST::Node *curNode)
    {
        AST::OperNode *returnNode = new AST::OperNode (AST::OperNode::OperType::RETURN);
        curNode->addChild (returnNode);
    }

    void pushChildrenIntoStack (AST::Node *curNode, std::stack<AST::Node *> &stack)
    {
        auto childrenFin = curNode->childEnd ();

        for (auto childrenSt = curNode->childBegin (); childrenSt != childrenFin; ++childrenSt)
            stack.push (*childrenSt);
    }

}  // namespace

void SemanticAnalyzer::AnalyzeHiddenReturn (Tree::NAryTree<AST::Node *> *tree)
{
    AST::Node *curNode = tree->getRoot ();
    if (!curNode)
        return;

    std::stack<AST::Node *> stack;
    stack.push (curNode);

    while (stack.size ()) {
        curNode = stack.top ();
        stack.pop ();

        AST::NodeT curType = curNode->getType ();
        AST::Node *parent = curNode->getParent ();

        if (curType == AST::NodeT::FUNCTION) {
            AST::FuncNode *funcNode = static_cast<AST::FuncNode *> (curNode);
            if (funcNode->getFuncCompType () == AST::FuncNode::FuncComponents::FUNC_DECL) {
                AST::Node *scopeNode = funcNode->getRightChild ();
                HiddenReturnNodesAnalyze (scopeNode);
            }
        }
        if (curType == AST::NodeT::SCOPE) {
            if (parent && parent->getType () == AST::NodeT::OPERATOR)
                if (static_cast<AST::OperNode *> (parent)->getOpType () == AST::OperNode::OperType::ASSIGN)
                    HiddenReturnNodesAnalyze (curNode);

            UselessStatementRecognizer (curNode);
        }

        pushChildrenIntoStack (curNode, stack);
    }
}

void SemanticAnalyzer::HiddenReturnNodesAnalyze (AST::Node *curNode)
{
    AST::Node *rightNode = curNode->getRightChild ();

    if (rightNode == nullptr) {
        SetFillerReturn (curNode);
    }
    else {
        switch (rightNode->getType ()) {
            case AST::NodeT::CONDITION: {
                SetFillerReturn (curNode);
                break;
            }
            case AST::NodeT::OPERATOR: {
                AST::OperNode *oper = static_cast<AST::OperNode *> (rightNode);
                if (oper->getOpType () == AST::OperNode::OperType::RETURN)
                    break;
                else if (oper->getOpType () != AST::OperNode::OperType::PRINT) {
                    SetHiddenReturn (curNode, oper);
                }
                else {
                    SetFillerReturn (curNode);
                }
                break;
            }
            case AST::NodeT::NUMBER:
            case AST::NodeT::VARIABLE: {
                SetHiddenReturn (curNode, rightNode);
                break;
            }
        }
    }
}

void SemanticAnalyzer::UselessStatementRecognizer (AST::Node *curNode)
{
    AST::Node *rightNode = curNode->getRightChild ();

    for (auto i = curNode->childBegin (); i != curNode->childEnd (); ++i) {
        if ((*i)->getType () == AST::NodeT::OPERATOR) {
            AST::OperNode *operNode = static_cast<AST::OperNode *> (*i);
            switch (operNode->getOpType ()) {
                case AST::OperNode::OperType::PRINT:
                case AST::OperNode::OperType::SCAN:
                case AST::OperNode::OperType::ASSIGN:
                case AST::OperNode::OperType::RETURN: break;
            }
        }
    }
}

//===========================================================================================
//======================================SCOPE ANALYZING======================================
//===========================================================================================

namespace {

    std::pair<AST::NodeT, AST::Node *> GetRValueType (AST::OperNode *node)
    {
        AST::Node *rightChild = node;

        while (node->getOpType () == AST::OperNode::OperType::ASSIGN) {
            rightChild = node->getRightChild ();

            if (rightChild->getType () == AST::NodeT::OPERATOR)
                node = static_cast<AST::OperNode *> (rightChild);
            else
                break;
        }

        return {rightChild->getType (), rightChild};
    }

    void SetFunctionNameIntoScopes (Scope *newScope, Scope *curScope, AST::FuncNode *preFuncName, AST::Node *rNode)
    {
        AST::VarNode *funcName = static_cast<AST::VarNode *> ((*preFuncName)[0]);
        FuncObject *insideFunc = new FuncObject (static_cast<AST::FuncNode *> (rNode));
        FuncObject *outsideFunc = new FuncObject (static_cast<AST::FuncNode *> (rNode));
        auto name = funcName->getName();
        newScope->add (name, insideFunc);
        curScope->add (name, outsideFunc); 
    }

    void SetArgumentsIntoNewScope (Scope *newScope, AST::FuncNode *funcArgs)
    {
        for (auto beginIt = funcArgs->childBegin (); beginIt != funcArgs->childEnd (); ++beginIt) {
            AST::VarNode *argASTRef = static_cast<AST::VarNode *> ((*beginIt));
            Variable *newVar = new Variable (argASTRef);
            newScope->add (argASTRef->getName (), newVar);
        }
    }

}  // namespace

void SemanticAnalyzer::AnalyzeScopes (Scope *curScope, AST::ScopeNode *node)
{
    for (auto stBegin = node->childBegin (); stBegin != node->childEnd (); ++stBegin) {
        AST::Node *nodeToCheck = *stBegin;
        switch (nodeToCheck->getType ()) {
            case AST::NodeT::OPERATOR: {
                CheckExprScope (curScope, static_cast<AST::OperNode *> (nodeToCheck));
                break;
            }
            case AST::NodeT::CONDITION: {
                CheckCondScope (curScope, static_cast<AST::CondNode *> (nodeToCheck));
                break;
            }
            default: pushError_ (nodeToCheck->getLocation (), "unexpected statement type");
        }
    }
}

void SemanticAnalyzer::AnalyzeNewScope (Scope *curScope, AST::Node *newScopeNode)
{
    Scope *newScope = new Scope;
    curScope->add (newScope);
    AnalyzeScopes (newScope, static_cast<AST::ScopeNode *> (newScopeNode));
}

void SemanticAnalyzer::CheckConditionScopeExpr (Scope *curScope, AST::CondNode *node)
{
    // HandleBinaryOperation (curScope, (*node)[0]);
    switch ((*node)[0]->getType ()) {
        case AST::NodeT::OPERATOR: CheckExprScope (curScope, static_cast<AST::OperNode *> ((*node)[0])); break;
        case AST::NodeT::VARIABLE: CheckScopeVarInExpr (curScope, static_cast<AST::VarNode *> ((*node)[0]));
    }

    AnalyzeNewScope (curScope, (*node)[1]);
}

void SemanticAnalyzer::CheckCondScope (Scope *curScope, AST::CondNode *node)
{
    switch (node->getConditionType ()) {
        case AST::CondNode::ConditionType::IF:
        case AST::CondNode::ConditionType::WHILE: {
            CheckConditionScopeExpr (curScope, node);
            break;
        }
        default: pushError_ (node->getLocation (), "unexpected condition type");
    }
}

void SemanticAnalyzer::HandleExistVarInScope (Scope *curScope,
                                              AST::VarNode *clearID,
                                              AST::OperNode *node,
                                              std::pair<Scope *, Scope::tblIt> findRes)
{
    Scope::tblIt foundedIt = findRes.second;
    TypeWrapper *foundedWrap = (*foundedIt).second;

    if (foundedWrap->type_ == TypeWrapper::DataType::FUNC)
        pushError_ (clearID->getLocation (), "function redefinition");
    else {
        std::pair<AST::NodeT, AST::Node *> rVal = GetRValueType (node);
        AST::NodeT rType = rVal.first;

        AST::Node *rightNode = node->getRightChild ();
        if (rightNode->getType () == AST::NodeT::OPERATOR)
            CheckExprScope (curScope, static_cast<AST::OperNode *> (rightNode));
        else {
            switch (rType) {
                case AST::NodeT::FUNCTION:
                    pushError_ (clearID->getLocation (), "variable can't be overriden by a function");
                    break;
                case AST::NodeT::VARIABLE:
                    CheckScopeVarInExpr (curScope, static_cast<AST::VarNode *> (rVal.second));
                    break;
                case AST::NodeT::SCOPE: AnalyzeNewScope (curScope, rVal.second); break;
            }
        }
    }
}

void SemanticAnalyzer::CheckAssignStatementScope (Scope *curScope, AST::OperNode *node)
{
    AST::Node *idNode = node->getLeftChild ();

    if (idNode->getType () != AST::NodeT::VARIABLE) {
        pushError_ (idNode->getLocation (), "variable name expected");
        return;
    }

    AST::VarNode *clearID = static_cast<AST::VarNode *> (idNode);
    const std::string name = clearID->getName ();

    std::pair<Scope *, Scope::tblIt> findRes = curScope->smartLookup (name);

    if (findRes.first == nullptr) {  // new var
        CreateNewObjectInScope (curScope, node, clearID);
    }
    else
        HandleExistVarInScope (curScope, clearID, node, findRes);
}

void SemanticAnalyzer::CheckScopeVarInExpr (Scope *curScope, AST::VarNode *node)
{
    std::pair<Scope *, Scope::tblIt> findRes = curScope->smartLookup (node->getName ());
    if (findRes.first == nullptr) {
        pushError_ (node->getLocation (), "undeclared variable in expression");
        return;
    }

    auto checkType = findRes.second;
    if ((*checkType).second->type_ == TypeWrapper::DataType::FUNC)
        pushError_ (node->getLocation (), "function can't be used as a variable");
}

void SemanticAnalyzer::CreateNewObjectInScope (Scope *curScope, AST::OperNode *node, AST::VarNode *clearID)
{
    std::pair<AST::NodeT, AST::Node *> rVal = GetRValueType (node);

    switch (rVal.first) {
        case AST::NodeT::FUNCTION: CreateNewFunctionInScope (curScope, node, clearID, rVal.second); break;
        case AST::NodeT::SCOPE: CreateNewVariableViaScope (curScope, node, clearID, rVal.second); break;
        default: CreateNewVariableInScope (curScope, node, clearID);
    }
}

void SemanticAnalyzer::CreateNewVariableViaScope (Scope *curScope,
                                                  AST::OperNode *node,
                                                  AST::VarNode *clearID,
                                                  AST::Node *rNode)
{
    CreateNewVariableInScope (curScope, node, clearID);
    AnalyzeNewScope (curScope, rNode);
}

void SemanticAnalyzer::CreateNewVariableInScope (Scope *curScope, AST::OperNode *node, AST::VarNode *clearID)
{
    Variable *newVar = new Variable (clearID);
    curScope->add (clearID->getName (), newVar);

    AST::Node *rightChild = node->getRightChild ();

    if (rightChild->getType () == AST::NodeT::OPERATOR) {
        AST::OperNode *operNode = static_cast<AST::OperNode *> (rightChild);
        CheckOperatorInExpr (curScope, operNode);
    }
}

void SemanticAnalyzer::CreateNewFunctionInScope (Scope *curScope,
                                                 AST::OperNode *node,
                                                 AST::VarNode *clearID,
                                                 AST::Node *rNode)
{
    FuncObject *newFunc = new FuncObject (static_cast<AST::FuncNode *> (rNode));
    curScope->add (clearID->getName (), newFunc);

    AST::Node *funcNode = node->getRightChild ();

    Scope *newScope = new Scope;
    AST::FuncNode *funcComp = static_cast<AST::FuncNode *> ((*funcNode)[0]);
    if (funcNode->getChildrenNum () == 3) {
        SetFunctionNameIntoScopes (newScope, curScope, funcComp, rNode);
        funcComp = static_cast<AST::FuncNode *> ((*funcNode)[1]);
    }

    SetArgumentsIntoNewScope (newScope, funcComp);

    curScope->add (newScope);
    AnalyzeScopes (newScope, static_cast<AST::ScopeNode *> (funcNode->getRightChild ()));
}

void SemanticAnalyzer::CheckExprScope (Scope *curScope, AST::OperNode *node)
{
    switch (node->getOpType ()) {
        case AST::OperNode::OperType::ASSIGN: {
            CheckAssignStatementScope (curScope, node);
            break;
        }
        case AST::OperNode::OperType::UNARY_M:
        case AST::OperNode::OperType::UNARY_P:
        case AST::OperNode::OperType::RETURN:
        case AST::OperNode::OperType::PRINT: {
            CheckUnaryOperScope (curScope, node);
            break;
        }
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
        case AST::OperNode::OperType::MOD: HandleBinaryOperation (curScope, node); break;
        case AST::OperNode::OperType::CALL: CheckCallOperatorInExpr (curScope, node); break;

        default: pushError_ (node->getLocation (), "unexpected operator type");
    }
}

void SemanticAnalyzer::CheckUnaryOperScope (Scope *curScope, AST::Node *node)
{
    if (!node->getChildrenNum())
        return;

    AST::Node *leftChild = node->getLeftChild ();

    if (leftChild->getType () == AST::NodeT::OPERATOR) {
        AST::OperNode *someOperator = static_cast<AST::OperNode *> (leftChild);
        if (someOperator->getOpType () == AST::OperNode::OperType::ASSIGN) {
            CheckAssignStatementScope (curScope, someOperator);
        }
    }

    HandleBinaryOperation (curScope, node->getRightChild ());
}

void SemanticAnalyzer::HandleBinaryOperation (Scope *curScope, AST::Node *node)
{
    std::vector<AST::Node *> execVector;
    BuildExecStackFromExpression (execVector, node);

    for (auto curNodeFromExpr : execVector) {
        switch (curNodeFromExpr->getType ()) {
            case AST::NodeT::OPERATOR: {
                CheckOperatorInExpr (curScope, static_cast<AST::OperNode *> (curNodeFromExpr));
                break;
            }
            case AST::NodeT::VARIABLE: {
                CheckScopeVarInExpr (curScope, static_cast<AST::VarNode *> (curNodeFromExpr));
                break;
            }
            case AST::NodeT::NUMBER: {
                break;
            }
            default: pushError_ (curNodeFromExpr->getLocation (), "unexpected operator in expression");
        }
    }
}

void SemanticAnalyzer::BuildExecStackFromExpression (std::vector<AST::Node *> &execVector,
                                                     AST::Node *node)  //! TODO stack?
{
    AST::Node *curNode = node;
    std::stack<AST::Node *> descent;

    descent.push (curNode);

    while (!descent.empty ()) {
        curNode = descent.top ();
        descent.pop ();

        switch (curNode->getType ()) {
            case AST::NodeT::NUMBER:
            case AST::NodeT::VARIABLE: {
                execVector.push_back (curNode);
                break;
            }
            case AST::NodeT::OPERATOR: {
                AST::OperNode *opNode = static_cast<AST::OperNode *> (curNode);
                switch (opNode->getOpType ()) {
                    case AST::OperNode::OperType::UNARY_M:
                    case AST::OperNode::OperType::UNARY_P: {
                        descent.push (opNode->getLeftChild ());
                        execVector.push_back (curNode);
                        break;
                    }
                    case AST::OperNode::OperType::CALL:
                    case AST::OperNode::OperType::SCAN: {
                        execVector.push_back (curNode);
                        break;
                    }
                    default:
                        execVector.push_back (curNode);
                        descent.push (opNode->getRightChild ());
                        descent.push (opNode->getLeftChild ());
                }

                break;
            }
            default: {
                pushError_ (curNode->getLocation (), "unexpected operator in expression");
                return;
            }
        }
    }
}

void SemanticAnalyzer::CheckCallOperatorInExpr (Scope *curScope, AST::OperNode *node)
{
    AST::VarNode *funcID = static_cast<AST::VarNode *> (node->getLeftChild ());
    AST::FuncNode *callArgs = static_cast<AST::FuncNode *> (node->getRightChild ());

    std::pair<Scope *, Scope::tblIt> findFunc = curScope->smartLookup (funcID->getName ());
    if (findFunc.first == nullptr)
        pushError_ (funcID->getLocation (), "undeclared function to call");
    else {
        Scope::tblIt scopeIt = findFunc.second;
        TypeWrapper *scopeFoundElem = (*scopeIt).second;

        if (scopeFoundElem->type_ == TypeWrapper::DataType::FUNC) {
            FuncObject *funcTransform = static_cast<FuncObject *> (scopeFoundElem);

            AST::FuncNode *funcDecl = funcTransform->getNode ();
            AST::FuncNode *leftFuncChild = static_cast<AST::FuncNode *> (funcDecl->getLeftChild ());
            AST::FuncNode *funcArgs = nullptr;
            if (leftFuncChild->getFuncCompType () == AST::FuncNode::FuncComponents::FUNC_ARGS)
                funcArgs = leftFuncChild;
            else
                funcArgs = static_cast<AST::FuncNode *> ((*funcDecl)[1]);

            CheckArgsAmmountForCall (funcArgs, callArgs);
        }
        else
            pushError_ (funcID->getLocation (), "not a function to call");
    }
}

void SemanticAnalyzer::CheckArgsAmmountForCall (AST::FuncNode *funcArgs, AST::FuncNode *callArgs)
{
    if (funcArgs->getChildrenNum () != callArgs->getChildrenNum ())
        pushError_ (callArgs->getLocation (), "wrong number of arguments for a call");
}

void SemanticAnalyzer::CheckOperatorInExpr (Scope *curScope, AST::OperNode *node)
{
    AST::OperNode::OperType opType = node->getOpType ();

    switch (opType) {  //! TODO is it all?

        case AST::OperNode::OperType::CALL: {
            CheckCallOperatorInExpr (curScope, node);
            break;
        }
        case AST::OperNode::OperType::ASSIGN: {
            CheckAssignStatementScope (curScope, node);
            break;
        }

            // for the future...
    }
}

namespace {

    void BuildQueueAfterDFS (std::vector<AST::Node *> &queueOnSearchReturn, AST::Node *curNode)
    {
        std::stack<AST::Node *> stack;

        stack.push (curNode);

        while (stack.size ()) {
            curNode = stack.top ();
            stack.pop ();
            queueOnSearchReturn.push_back (curNode);

            auto childrenSt = curNode->childBegin ();
            auto childrenFin = curNode->childEnd ();

            while (childrenSt != childrenFin) {
                stack.push (*childrenSt);
                childrenSt = std::next (childrenSt, 1);
            }
        }
    }

}  // namespace

void SemanticAnalyzer::AnalyzeReturnPosition (Tree::NAryTree<AST::Node *> *tree)
{
    AST::Node *curNode = tree->getRoot ();
    if (!curNode)
        return;

    std::vector<AST::Node *> queueOnSearchReturn;
    BuildQueueAfterDFS (queueOnSearchReturn, curNode);

    for (auto curIt = queueOnSearchReturn.rbegin (), endIt = queueOnSearchReturn.rend (); curIt != endIt; ++curIt) {
        AST::NodeT type = (*curIt)->getType ();
        if (type == AST::NodeT::OPERATOR) {
            AST::OperNode::OperType operType = static_cast<AST::OperNode *> (*curIt)->getOpType ();
            if (operType == AST::OperNode::OperType::RETURN) {
                AST::Node *parent = (*curIt)->getParent ();

                while (parent) {
                    AST::NodeT parentType = parent->getType ();
                    if (parentType == AST::NodeT::FUNCTION)
                        break;

                    if (parentType == AST::NodeT::OPERATOR) {  // only assign
                        AST::InlineScopeNode *inlineScope = new AST::InlineScopeNode (parent);
                        inlineScope->addChild ((*parent)[1]);
                        parent->eraseChild (1);
                        parent->addChild (inlineScope);
                        break;
                    }

                    parent = parent->getParent ();
                }

                if (!parent) {
                    pushError_ ((*curIt)->getLocation (), "return is not in function");
                }
            }
        }
    }
}
