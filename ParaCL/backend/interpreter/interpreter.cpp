#include "interpreter.hpp"

#include <stack>
#include <vector>

namespace interpret {
#if 1
    Scope::~Scope ()  //TODO: class wrap
    {
        for (auto i : tbl_)
            delete i.second;
    }
#endif
    VarWrapper *Scope::lookup (const std::string &name) const
    {
        const Scope *curScope = this;
        while (curScope) {
            auto elem = curScope->tbl_.find (name);
            if (elem != curScope->tbl_.end ())
                return elem->second;

            curScope = curScope->parent_;
        }

        return nullptr;
    }

    void Scope::add (const std::string &name, VarWrapper *var)
    {
        tbl_.insert ({name, var});
    }

    void Scope::add (Scope *scope)
    {
        children_.push_back (scope);
        scope->parent_ = this;
    }

    namespace {

        int CalcExpr (Scope *curScope, AST::Node *node);

        int CalcVar (Scope *curScope, AST::VarNode *var)
        {
            Variable<int> *clearVar = static_cast<Variable<int> *> (curScope->lookup (var->getName ()));
            return clearVar->getVal ();
        }

        int CalcOper (Scope *curScope, AST::OperNode *node)
        {
            std::vector<AST::Node *> children = node->getChildren ();
            switch (node->getOpType ()) {
                case AST::OperNode::OperType::ADD:
                    return CalcExpr (curScope, children[0]) + CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::SUB:
                    return CalcExpr (curScope, children[0]) - CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::MUL:
                    return CalcExpr (curScope, children[0]) * CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::DIV:
                    return CalcExpr (curScope, children[0]) / CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::EQ:
                    return CalcExpr (curScope, children[0]) == CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::NEQ:
                    return CalcExpr (curScope, children[0]) != CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::MORE:
                    return CalcExpr (curScope, children[0]) > CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::LESS:
                    return CalcExpr (curScope, children[0]) < CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::LTE:
                    return CalcExpr (curScope, children[0]) <= CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::GTE:
                    return CalcExpr (curScope, children[0]) >= CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::AND:
                    return CalcExpr (curScope, children[0]) && CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::OR:
                    return CalcExpr (curScope, children[0]) || CalcExpr (curScope, children[1]);
                case AST::OperNode::OperType::SCAN: {
                    int tmp;  //TODO: other types
                    std::cin >> tmp;
                    return tmp;
                }
                default: {
                    throw std::runtime_error ("Unexpected operator type in calculation");
                }
            }
        }

        int CalcExpr (Scope *curScope, AST::Node *node)
        {
            switch (node->getType ()) {
                case AST::NodeT::VARIABLE:
                    return CalcVar (curScope, static_cast<AST::VarNode *> (node));
                case AST::NodeT::NUMBER:
                    return static_cast<AST::NumNode *> (node)->getValue ();
                case AST::NodeT::OPERATOR:
                    return CalcOper (curScope, static_cast<AST::OperNode *> (node));
                default:
                    throw std::runtime_error ("Unexpected node type");
            }
        }

    }  // namespace

    void Interpreter::assignment (Scope *curScope, AST::OperNode *node)
    {
        std::vector<AST::Node *> children = node->getChildren ();

        AST::VarNode *leftN = static_cast<AST::VarNode *> (children[0]);
        int val = CalcExpr (curScope, children[1]);  //Now we have only int type... In the near future it shall be template

        const std::string &name = leftN->getName ();
        VarWrapper *findVar = curScope->lookup (name);
        if (!findVar) {
            Variable<int> *newVar = new Variable<int> (val);
            curScope->add (name, newVar);
        }
        else {
            Variable<int> *clearVar = static_cast<Variable<int> *> (findVar);  //Only int type...
            clearVar->setVal (val);
        }
    }

    void Interpreter::print (Scope *curScope, AST::OperNode *node)
    {
        std::vector<AST::Node *> children = node->getChildren ();

        for (auto curNode : children)
            std::cout << CalcExpr (curScope, curNode) << std::endl;
    }

    void Interpreter::execOper (Scope *curScope, AST::OperNode *node)
    {
        switch (node->getOpType ()) {
            case AST::OperNode::OperType::ASSIGN: {
                assignment (curScope, node);
                break;
            }
            case AST::OperNode::OperType::PRINT: {
                print (curScope, node);
                break;
            }
        }
    }

    void Interpreter::execIf (Scope *curScope, AST::CondNode *node)
    {
        std::vector<AST::Node *> children = node->getChildren ();
        if (CalcExpr (curScope, children[0])) {
            Scope *newScope = new Scope;
            curScope->add (newScope);
            execScope (newScope, static_cast<AST::ScopeNode *> (children[1]));
        }
    }

    void Interpreter::execWhile (Scope *curScope, AST::CondNode *node)
    {
        std::vector<AST::Node *> children = node->getChildren ();
        while (CalcExpr (curScope, children[0])) {
            Scope *newScope = new Scope;
            curScope->add (newScope);
            execScope (newScope, static_cast<AST::ScopeNode *> (children[1]));
        }
    }

    void Interpreter::execCond (Scope *curScope, AST::CondNode *node)
    {
        switch (node->getConditionType ()) {
            case AST::CondNode::ConditionType::IF:
                execIf (curScope, node);
                break;
            case AST::CondNode::ConditionType::WHILE:
                execWhile (curScope, node);
                break;
            default:
                throw std::runtime_error ("Unexpected condition statement type");
        }
    }

    void Interpreter::execScope (Scope *curScope, AST::ScopeNode *node)
    {
        std::vector<AST::Node *> curNodes = node->getChildren ();
        while (!curNodes.empty ()) {
            AST::Node *nodeToExec = curNodes[0];
            switch (nodeToExec->getType ()) {
                case AST::NodeT::OPERATOR: {
                    execOper (curScope, static_cast<AST::OperNode *> (nodeToExec));
                    break;
                }
                case AST::NodeT::CONDITION: {
                    execCond (curScope, static_cast<AST::CondNode *> (nodeToExec));
                    break;
                }
                default:
                    throw std::runtime_error ("Unexpected node to execute");
            }
            curNodes.erase (curNodes.begin ());
        }
    }

    void Interpreter::run ()
    {
        Tree::NAryTree<Scope *> *mainRoot = globalScope_;
        AST::ScopeNode *startNode = static_cast<AST::ScopeNode *> (tree_->getRoot ());
        execScope (mainRoot->getRoot (), startNode);
    }

}  // namespace interpret
