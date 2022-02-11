#include "semantic.hpp"

using tblIt = std::unordered_map<std::string, TypeWrapper *>::iterator;

namespace {

    void pushChildrenIntoStack (AST::Node *curNode, std::stack<AST::Node *> &stack)
    {
        auto childrenFin = curNode->childEnd ();

        for (auto childrenSt = curNode->childBegin (); childrenSt != childrenFin; ++childrenSt)
            stack.push (*childrenSt);
    }

    void uselessStatementRecognizer (SemanticAnalyzer *analyzer_,
                                     AST::Node *curNode,
                                     const std::function<void (yy::location, const std::string &)> pushWarning)
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
                    default: pushWarning (operNode->getLocation (), "useless statement"); break;
                }
            }
        }
    }

    void SetHiddenReturn (AST::Node *curNode, AST::Node *oper)
    {
        AST::OperNode *returnNode = new AST::OperNode (AST::OperNode::OperType::RETURN);
        returnNode->addChild (oper);
        curNode->eraseChild (curNode->getChildrenNum () - 1);
        curNode->addChild (returnNode);
    }

    void SetPoisonReturn (AST::Node *curNode)
    {
        const int poison = -666;

        AST::OperNode *returnNode = new AST::OperNode (AST::OperNode::OperType::RETURN);
        AST::NumNode *numNode = new AST::NumNode (poison);
        returnNode->addChild (numNode);
        curNode->addChild (returnNode);
    }

    void HiddenReturnNodesAnalyze (SemanticAnalyzer *analyzer_,
                                   AST::Node *curNode,
                                   const std::function<void (yy::location, const std::string &)> pushWarning)
    {
        AST::Node *rightNode = curNode->getRightChild ();

        if (rightNode == nullptr) {
            pushWarning (static_cast<AST::ScopeNode *> (curNode)->getLocation (), "empty scope");
            SetPoisonReturn (curNode);
        }
        else {
            switch (rightNode->getType ()) {
                case AST::NodeT::CONDITION: {
                    pushWarning (static_cast<AST::CondNode *> (rightNode)->getLocation (), "can't find return after condition statement");
                    SetPoisonReturn (curNode);
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
                        pushWarning (oper->getLocation (), "unexpected statement in the scope end");
                        SetPoisonReturn (curNode);
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

    void AnalyzeHiddenReturn (SemanticAnalyzer *analyzer_,
                              Tree::NAryTree<AST::Node *> *tree,
                              const std::function<void (yy::location, const std::string &)> pushWarning)
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
                    HiddenReturnNodesAnalyze (analyzer_, scopeNode, pushWarning);
                }
            }
            if (curType == AST::NodeT::SCOPE) {
                if (parent && parent->getType () == AST::NodeT::OPERATOR)
                    if (static_cast<AST::OperNode *> (parent)->getOpType () == AST::OperNode::OperType::ASSIGN)
                        HiddenReturnNodesAnalyze (analyzer_, curNode, pushWarning);

                uselessStatementRecognizer (analyzer_, curNode, pushWarning);
            }

            pushChildrenIntoStack (curNode, stack);
        }
    }

    void BuildExecStackFromExpression (std::vector<AST::Node *> &execVector,
                                       AST::Node *node,
                                       const std::function<void (yy::location, const std::string &)> pushWarning,
                                       const std::function<void (yy::location, const std::string &)> pushError)
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
                    pushError (curNode->getLocation (), "unexpected operator in expression");
                    return;
                }
            }
        }
    }

    void CheckVarInExpr (Scope *curScope,
                         AST::VarNode *node,
                         const std::function<void (yy::location, const std::string &)> pushWarning,
                         const std::function<void (yy::location, const std::string &)> pushError)
    {
        std::pair<Scope *, Scope::tblIt> findRes = curScope->smartLookup (node->getName ());
        if (findRes.first == nullptr) {
            pushError (node->getLocation (), "undeclared variable in expression");
            return;
        }

        auto checkType = findRes.second;
        if ((*checkType).second->type_ == TypeWrapper::DataType::FUNC)
            pushError (node->getLocation (), "function can't be used as a variable");
    }

    void CheckArgsAmmountForCall (AST::FuncNode *funcArgs,
                                  AST::FuncNode *callArgs,
                                  const std::function<void (yy::location, const std::string &)> pushWarning,
                                  const std::function<void (yy::location, const std::string &)> pushError)
    {
        if (funcArgs->getChildrenNum () != callArgs->getChildrenNum ())
            pushError (callArgs->getLocation (), "wrong number of arguments for a call");
    }

    void CheckCallOperatorInExpr (Scope *curScope,
                                  AST::OperNode *node,
                                  const std::function<void (yy::location, const std::string &)> pushWarning,
                                  const std::function<void (yy::location, const std::string &)> pushError)
    {
        AST::VarNode *funcID = static_cast<AST::VarNode *> (node->getLeftChild ());
        AST::FuncNode *callArgs = static_cast<AST::FuncNode *> (node->getRightChild ());

        std::pair<Scope *, Scope::tblIt> findFunc = curScope->smartLookup (funcID->getName ());
        if (findFunc.first == nullptr)
            pushError (funcID->getLocation (), "undeclared function to call");
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

                CheckArgsAmmountForCall (funcArgs, callArgs, pushWarning, pushError);
            }
            else
                pushError (funcID->getLocation (), "not a function to call");
        }
    }

    void CheckOperatorInExpr (Scope *curScope,
                              AST::OperNode *node,
                              const std::function<void (yy::location, const std::string &)> pushWarning,
                              const std::function<void (yy::location, const std::string &)> pushError)
    {
        AST::OperNode::OperType opType = node->getOpType ();

        switch (opType) {  //! TODO is it all?

            case AST::OperNode::OperType::CALL: {
                CheckCallOperatorInExpr (curScope, node, pushWarning, pushError);
                break;
            }
            default: break;
        }
    }

    std::pair<AST::NodeT, AST::Node*> GetRValueType (AST::OperNode *node) //TODO not only var or function ---> Plus and others may be too
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

}  // namespace

void SemanticAnalyzer::BuildingBinaryOperation (Scope *curScope,
                                                AST::Node *node,
                                                const std::function<void (yy::location, const std::string &)> pushWarning,
                                                const std::function<void (yy::location, const std::string &)> pushError)
{
    std::vector<AST::Node *> execVector;
    BuildExecStackFromExpression (execVector, node, pushWarning, pushError);

    for (auto curNodeFromExpr : execVector) {

        switch (curNodeFromExpr->getType ()) {
            case AST::NodeT::OPERATOR: {
                CheckOperatorInExpr (curScope, static_cast<AST::OperNode *> (curNodeFromExpr), pushWarning, pushError);
                break;
            }
            case AST::NodeT::VARIABLE: {
                CheckVarInExpr (curScope, static_cast<AST::VarNode *> (curNodeFromExpr), pushWarning, pushError);
                break;
            }
            case AST::NodeT::NUMBER: {
                break;
            }
            default: pushError (curNodeFromExpr->getLocation (), "unexpected operator in expression");
        }
    }

}

void SemanticAnalyzer::CheckUnaryOperScope (Scope *curScope,
                                            AST::Node *node,
                                            const std::function<void (yy::location, const std::string &)> pushWarning,
                                            const std::function<void (yy::location, const std::string &)> pushError)
{
    AST::Node *leftChild = node->getLeftChild ();

    if (leftChild->getType () == AST::NodeT::OPERATOR) {
        AST::OperNode *someOperator = static_cast<AST::OperNode *> (leftChild);
        if (someOperator->getOpType () == AST::OperNode::OperType::ASSIGN) {
            CheckAssignStatementScope (curScope, someOperator, pushWarning, pushError);
        }
    }

    BuildingBinaryOperation (curScope, node->getRightChild(), pushWarning, pushError);

}

void SemanticAnalyzer::CheckExprScope (Scope *curScope,
                                       AST::OperNode *node,
                                       const std::function<void (yy::location, const std::string &)> pushWarning,
                                       const std::function<void (yy::location, const std::string &)> pushError)
{
    switch (node->getOpType ()) {
        case AST::OperNode::OperType::ASSIGN: {
            CheckAssignStatementScope (curScope, node, pushWarning, pushError);
            break;
        }
        case AST::OperNode::OperType::RETURN:
        case AST::OperNode::OperType::PRINT: {
            CheckUnaryOperScope (curScope, node, pushWarning, pushError);
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
        case AST::OperNode::OperType::MOD: 
            BuildingBinaryOperation (curScope, node, pushWarning, pushError);
            break;
        /*//TODO Good luck
        case AST::OperNode::OperType::UNARY_M:
        case AST::OperNode::OperType::UNARY_P: {
            break;
        }
        }*/

        default: pushError (node->getLocation (), "unexpected operator type");
    }
}

void SemanticAnalyzer::CreateNewObjectInScope (Scope *curScope, AST::OperNode* node, AST::VarNode *clearID,
                                               const std::function<void (yy::location, const std::string &)> pushWarning,
                                               const std::function<void (yy::location, const std::string &)> pushError) 
{

    std::pair<AST::NodeT, AST::Node *> rVal = GetRValueType (node);
    const std::string name = clearID->getName ();

    if (rVal.first != AST::NodeT::FUNCTION) {

        Variable *newVar = new Variable (clearID);
        curScope->add (name, newVar);

        AST::Node* rightChild = node->getRightChild();

        if (rightChild->getType() == AST::NodeT::OPERATOR)
            CheckExprScope (curScope, static_cast<AST::OperNode *>(rightChild), pushWarning, pushError);
        
    }
    else {

        FuncObject *newFunc = new FuncObject (static_cast<AST::FuncNode*> (rVal.second));
        curScope->add (name, newFunc);

        AST::Node* funcNode = node->getRightChild ();

        Scope *newScope = new Scope;
        AST::FuncNode* funcArgs = static_cast<AST::FuncNode*>((*funcNode) [0]);
        switch (funcNode->getChildrenNum ()) {

            case 3: {

                AST::FuncNode* preFuncName = static_cast<AST::FuncNode *>((*funcNode) [0]);
                AST::VarNode* funcName = static_cast<AST::VarNode*> ((*preFuncName) [0]);
                FuncObject* insideFunc = new FuncObject (static_cast<AST::FuncNode*> (rVal.second));
                newScope->add (funcName->getName (), insideFunc);
                funcArgs = static_cast<AST::FuncNode*>((*funcNode) [1]);

            }
            case 2: {

                for (auto beginIt = funcArgs->childBegin (); beginIt != funcArgs->childEnd (); ++beginIt) {

                    AST::VarNode* argASTRef = static_cast<AST::VarNode*> ((*beginIt));
                    Variable* newVar = new Variable (argASTRef);
                    newScope->add (argASTRef->getName (), newVar);

                }

                break;
            }
            default: 
                throw std::runtime_error ("wrong node for a function");

        }

        curScope->add (newScope);
        AnalyzeScopes (newScope, static_cast<AST::ScopeNode *> (funcNode->getRightChild ()), pushWarning, pushError);

    }

}

void SemanticAnalyzer::CheckAssignStatementScope (Scope *curScope, //TODO Try to example like c = x + y + g with undeclared g :)
                                                  AST::OperNode *node,
                                                  const std::function<void (yy::location, const std::string &)> pushWarning,
                                                  const std::function<void (yy::location, const std::string &)> pushError)
{
    AST::Node *idNode = node->getLeftChild ();
    
    if (idNode->getType () != AST::NodeT::VARIABLE) {
        pushError (idNode->getLocation (), "variable name expected");
        return;
    }

    AST::VarNode *clearID = static_cast<AST::VarNode *> (idNode);
    const std::string name = clearID->getName ();
    
    std::pair<Scope *, Scope::tblIt> findRes = curScope->smartLookup (name);
 
    if (findRes.first == nullptr)   // new var
        CreateNewObjectInScope (curScope, node, clearID, pushWarning, pushError);
    else {
        Scope::tblIt foundedIt = findRes.second;
        TypeWrapper* foundedWrap = (*foundedIt).second;

        if (foundedWrap->type_ == TypeWrapper::DataType::FUNC)
            pushError (clearID->getLocation (), "function redefinition");
        else {

            std::pair<AST::NodeT, AST::Node*> rVal = GetRValueType (node);
            AST::NodeT rType = rVal.first;

            AST::Node* rightNode = node->getRightChild ();
            if (rightNode->getType () == AST::NodeT::OPERATOR)
                CheckExprScope (curScope, static_cast<AST::OperNode*> (rightNode), pushWarning, pushError);
            else {

                switch (rType) {

                    case AST::NodeT::FUNCTION:
                        pushError (clearID->getLocation (), "variable can't be overriden by a function");
                        break;
                    case AST::NodeT::VARIABLE:
                        CheckVarInExpr (curScope, static_cast<AST::VarNode*> (rVal.second), pushWarning, pushError);
                        break;
                    case AST::NodeT::SCOPE:
                        //!TODO
                        break;

                }

            }

        }

    }
        
}

void SemanticAnalyzer::CheckCondScope (Scope *curScope,
                                       AST::CondNode *node,
                                       const std::function<void (yy::location, const std::string &)> pushWarning,
                                       const std::function<void (yy::location, const std::string &)> pushError)
{
    switch (node->getConditionType ()) {
        case AST::CondNode::ConditionType::IF:
        case AST::CondNode::ConditionType::WHILE: {
            CheckConditionExpression (curScope, node, pushWarning, pushError);
            break;
        }
        default: pushError (node->getLocation (), "unexpected condition type");
    }
}

void SemanticAnalyzer::CheckConditionExpression (Scope *curScope,
                                                 AST::CondNode *node,
                                                 const std::function<void (yy::location, const std::string &)> pushWarning,
                                                 const std::function<void (yy::location, const std::string &)> pushError)
{
    CheckUnaryOperScope (curScope, (*node)[0], pushWarning, pushError);
    Scope *newScope = new Scope;
    curScope->add (newScope);
    AnalyzeScopes (newScope, static_cast<AST::ScopeNode *> ((*node)[1]), pushWarning, pushError);
}

void SemanticAnalyzer::AnalyzeScopes (Scope *curScope,
                                      AST::ScopeNode *node,
                                      const std::function<void (yy::location, const std::string &)> pushWarning,
                                      const std::function<void (yy::location, const std::string &)> pushError)
{
    for (auto stBegin = node->childBegin (); stBegin != node->childEnd (); ++stBegin) {
        AST::Node *nodeToCheck = *stBegin;
        switch (nodeToCheck->getType ()) {
            case AST::NodeT::OPERATOR: {
                CheckExprScope (curScope, static_cast<AST::OperNode *> (nodeToCheck), pushWarning, pushError);
                break;
            }
            case AST::NodeT::CONDITION: {
                CheckCondScope (curScope, static_cast<AST::CondNode *> (nodeToCheck), pushWarning, pushError);
                break;
            }
            default: pushError (nodeToCheck->getLocation (), "unexpected statement type");
        }
    }
}

void SemanticAnalyzer::run (Tree::NAryTree<AST::Node *> *tree,
                            const std::function<void (yy::location, const std::string &)> pushWarning,
                            const std::function<void (yy::location, const std::string &)> pushError)
{
    AnalyzeHiddenReturn (this, tree, pushWarning);

    AST::ScopeNode *startScope = static_cast<AST::ScopeNode *> (tree->getRoot ());
    if (startScope)
        AnalyzeScopes (globalScope_->getRoot (), startScope, pushWarning, pushError);
}
