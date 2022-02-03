#include "semantic.hpp"

namespace {

    void SetPoisonReturn (AST::Node* curNode) {

        const int poison = -666;

        AST::OperNode*  returnNode  = new AST::OperNode (AST::OperNode::OperType::RETURN);
        AST::NumNode*   numNode     = new AST::NumNode (poison);
        returnNode->addChild (numNode);
        curNode->addChild (returnNode);

    }

    void HiddenReturnNodesAnalyze (AST::Node* curNode) {

        AST::Node* rightNode = curNode->getRightChild ();

        if (rightNode == nullptr) {
            //WARNINGS
            SetPoisonReturn (curNode);
        } else {

            switch (rightNode->getType ()) {

                case AST::NodeT::CONDITION: {
                    //WARNINGS
                    SetPoisonReturn (curNode);
                    break;
                }
                case AST::NodeT::OPERATOR: {
                    AST::OperNode* oper = static_cast<AST::OperNode*> (rightNode);
                    if (oper->getOpType () == AST::OperNode::OperType::RETURN)
                        break;
                    else if (oper->getOpType () == AST::OperNode::OperType::ASSIGN) {
                        AST::OperNode*  returnNode  = new AST::OperNode (AST::OperNode::OperType::RETURN);
                        returnNode->addChild (oper);
                        curNode->eraseChild (curNode->getChildrenNum () - 1);
                        curNode->addChild (returnNode);
                    } else {
                        //WARNINGS
                        SetPoisonReturn (curNode);
                    }

                    break;
                }
                
            }

        }

    }

    void AnalyzeHiddenReturn (Tree::NAryTree<AST::Node*>* tree) {

        AST::Node* curNode = tree->getRoot ();
        if (!curNode)
            return;

        std::stack<AST::Node*> stack;
        stack.push (curNode);

        while (stack.size ()) {
            curNode = stack.top ();
            stack.pop ();

            AST::NodeT curType = curNode->getType ();
            if (curType == AST::NodeT::SCOPE)
                HiddenReturnNodesAnalyze (curNode);

//next
            auto childrenSt = curNode->childBegin ();
            auto childrenFin = curNode->childEnd ();

            while (childrenSt != childrenFin) {
                stack.push (*childrenSt);
                childrenSt = std::next (childrenSt, 1);
            }
        }
        
    }

}

void SemanticAnalyzer::run (Tree::NAryTree<AST::Node*>* tree) {

    AnalyzeHiddenReturn (tree);

}
