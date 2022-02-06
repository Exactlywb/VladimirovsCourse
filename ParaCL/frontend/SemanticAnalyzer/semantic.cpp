#include "semantic.hpp"

namespace {

    void pushChildrenIntoStack (AST::Node *curNode, std::stack<AST::Node *> &stack)
    {
        auto childrenFin = curNode->childEnd ();

        for (auto childrenSt = curNode->childBegin (); childrenSt != childrenFin; ++childrenSt)
            stack.push (*childrenSt);
    }

    void uselessStatementRecognizer (SemanticAnalyzer *analyzer_, AST::Node *curNode, const std::function<void (yy::location, const std::string &)> pushWarning)
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

    void HiddenReturnNodesAnalyze (SemanticAnalyzer *analyzer_, AST::Node *curNode, const std::function<void (yy::location, const std::string &)> pushWarning)
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

    void AnalyzeHiddenReturn (SemanticAnalyzer *analyzer_, Tree::NAryTree<AST::Node *> *tree, const std::function<void (yy::location, const std::string &)> pushWarning)
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

}  // namespace

void SemanticAnalyzer::run (Tree::NAryTree<AST::Node *> *tree, const std::function<void (yy::location, const std::string &)> pushWarning)
{
    AnalyzeHiddenReturn (this, tree, pushWarning);
}
