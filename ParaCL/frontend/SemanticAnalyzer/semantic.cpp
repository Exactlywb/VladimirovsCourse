#include "semantic.hpp"

using tblIt = std::unordered_map<std::string, TypeWrapper *>::iterator;

namespace {

    void pushChildrenIntoStack (AST::Node *curNode, std::stack<AST::Node *> &stack)
    {
        auto childrenFin = curNode->childEnd ();

        for (auto childrenSt = curNode->childBegin (); childrenSt != childrenFin; ++childrenSt)
            stack.push (*childrenSt);
    }

    void uselessStatementRecognizer (SemanticAnalyzer *analyzer_, AST::Node *curNode, 
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
                    case AST::OperNode::OperType::RETURN:
                        break;
                    default:
                        pushWarning (operNode->getLocation (), "useless statement");
                        break;
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

    void HiddenReturnNodesAnalyze (SemanticAnalyzer *analyzer_, AST::Node *curNode, 
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

    void AnalyzeHiddenReturn (SemanticAnalyzer *analyzer_, Tree::NAryTree<AST::Node *> *tree, 
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

    void BuildExecStackFromExpression (std::stack<AST::Node*> &execStack, AST::Node *node,
                                       const std::function<void (yy::location, const std::string &)> pushWarning, 
                                       const std::function<void (yy::location, const std::string &)> pushError)
    {


        AST::Node* curNode = node;
        std::stack<AST::Node*> descent;

        descent.push (curNode);

        while (!descent.empty ()) {

            curNode = descent.top ();
            descent.pop ();

            switch (curNode->getType ()) {

                case AST::NodeT::NUMBER:
                case AST::NodeT::VARIABLE: {
                    execStack.push (curNode);
                    break;
                }
                case AST::NodeT::OPERATOR: {
                    
                    AST::OperNode* opNode = static_cast<AST::OperNode*> (curNode);
                    switch (opNode->getOpType ()) {

                        case AST::OperNode::OperType::UNARY_M:
                        case AST::OperNode::OperType::UNARY_P: {
                            descent.push (opNode->getLeftChild ());
                            break;
                        }
                        case AST::OperNode::OperType::CALL:
                        case AST::OperNode::OperType::SCAN: {
                            execStack.push (curNode);
                            break;
                        }
                        default:
                            descent.push (opNode->getLeftChild ());
                            descent.push (opNode->getRightChild ());

                    }

                    break;
                }
                default:
                    pushError (curNode->getLocation (), "unexpected operator in expression");

            }

        }

    }

    void CheckAssignOperScope (Scope *curScope, AST::OperNode *node, 
                            const std::function<void (yy::location, const std::string &)> pushWarning, 
                            const std::function<void (yy::location, const std::string &)> pushError)
    {

        //!TODO
        
    }

    void CheckOperatorInExpr (Scope *curScope, AST::OperNode *node,
                              const std::function<void (yy::location, const std::string &)> pushWarning,
                              const std::function<void (yy::location, const std::string &)> pushError) {

        //!TODO

    }

    void CheckVarInExpr (Scope *curScope, AST::VarNode *node,
                         const std::function<void (yy::location, const std::string &)> pushWarning,
                         const std::function<void (yy::location, const std::string &)> pushError) {

        std::pair<Scope*, Scope::tblIt> findRes = curScope->smartLookup (node->getName ());
        if (findRes.first == nullptr)
            pushError (node->getLocation (), "undeclared variable in expression");

        auto checkType = findRes.second;
        if ((*checkType).second->type_ == TypeWrapper::DataType::FUNC)
            pushError (node->getLocation (), "function can't be used as a variable");

    }

    void CheckUnaryOperScope (Scope *curScope, AST::OperNode *node, 
                              const std::function<void (yy::location, const std::string &)> pushWarning, 
                              const std::function<void (yy::location, const std::string &)> pushError)
    {

        std::stack<AST::Node*> execStack;
        BuildExecStackFromExpression (execStack, node->getRightChild (), pushWarning, pushError);

        while (!execStack.empty ()) {

            AST::Node* curNodeFromExpr = execStack.top ();
            execStack.pop ();

            switch (curNodeFromExpr->getType ()) {

                case AST::NodeT::OPERATOR: {
                    CheckOperatorInExpr (curScope, static_cast<AST::OperNode*> (curNodeFromExpr), pushWarning, pushError);
                    break;
                }
                case AST::NodeT::VARIABLE: {
                    CheckVarInExpr (curScope, static_cast<AST::VarNode*> (curNodeFromExpr), pushWarning, pushError);
                    break;
                }
                case AST::NodeT::NUMBER: {
                    break;
                }
                default:
                    pushError (curNodeFromExpr->getLocation (), "unexpected operator in expression");

            }

        }

    }

    void CheckExprScope (Scope *curScope, AST::OperNode *node, 
                         const std::function<void (yy::location, const std::string &)> pushWarning, 
                         const std::function<void (yy::location, const std::string &)> pushError)
    {

        switch (node->getOpType ()) {

            case AST::OperNode::OperType::ASSIGN: {
                CheckAssignOperScope (curScope, node, pushWarning, pushError);
                break;
            }
            case AST::OperNode::OperType::RETURN:
            case AST::OperNode::OperType::PRINT: {
                CheckUnaryOperScope (curScope, node, pushWarning, pushError);
                break;
            }
            default:
                pushError (node->getLocation (), "unexpected operator type");

        }

    }

}  // namespace

void SemanticAnalyzer::AnalyzeScopes   (Scope *curScope, AST::ScopeNode *node,
                                        const std::function<void (yy::location, const std::string &)> pushWarning, 
                                        const std::function<void (yy::location, const std::string &)> pushError) {

    for (auto stBegin = node->childBegin (); stBegin != node->childEnd (); ++stBegin) {

        AST::Node *nodeToCheck = *stBegin;
        std::cout << "still alive" << std::endl;
        switch (nodeToCheck->getType ()) {

            case AST::NodeT::OPERATOR: {
                CheckExprScope (curScope, static_cast<AST::OperNode*> (nodeToCheck), 
                                pushWarning, pushError);
                break;
            }
            // case AST::NodeT::CONDITION: {
            //     CheckCondScope (curScope, static_cast<AST::CondNode*> (nodeToCheck));
            //     break;
            // }
            default:
                pushError (node->getLocation (), "unexpected statement type");

        }

    }

}

void SemanticAnalyzer::run (Tree::NAryTree<AST::Node *> *tree, 
                            const std::function<void (yy::location, const std::string &)> pushWarning, 
                            const std::function<void (yy::location, const std::string &)> pushError)
{
    AST::ScopeNode* startScope = static_cast<AST::ScopeNode*> (tree->getRoot ());
    if (startScope)
        AnalyzeScopes (globalScope_->getRoot (), startScope, pushWarning, pushError);
    AnalyzeHiddenReturn (this, tree, pushWarning);
}

