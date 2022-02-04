#include "interpreter.hpp"

#include <stack>
#include <vector>

namespace interpret {

    Scope::~Scope ()
    {
        for (auto i : tbl_)
            delete i.second;
    }

    Wrapper *Scope::lookup (const std::string &name) const
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

    void Scope::add (const std::string &name, Wrapper *var)
    {
        tbl_.insert ({name, var});
    }

    void Scope::add (Scope *scope)
    {
        children_.push_back (scope);
        scope->parent_ = this;
    }

    int Interpreter::CalcVar (Scope *curScope, AST::VarNode *var)
    {
        Variable<int> *clearVar = static_cast<Variable<int> *> (curScope->lookup (var->getName ()));
        if (clearVar)
            return clearVar->getVal ();
        else
            throw std::runtime_error ("Undeclared variable in an expression\n");
    }

    int Interpreter::CalcOper (Scope *curScope, AST::OperNode *node)
    {
        auto childrenSt = node->childBegin ();
        switch (node->getOpType ()) {
            case AST::OperNode::OperType::ADD:
                return CalcExpr (curScope, childrenSt) + CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::SUB:
                return CalcExpr (curScope, childrenSt) - CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::MUL:
                return CalcExpr (curScope, childrenSt) * CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::DIV:
                return CalcExpr (curScope, childrenSt) / CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::MOD:
                return CalcExpr (curScope, childrenSt) % CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::EQ:
                return CalcExpr (curScope, childrenSt) == CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::NEQ:
                return CalcExpr (curScope, childrenSt) != CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::MORE:
                return CalcExpr (curScope, childrenSt) > CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::LESS:
                return CalcExpr (curScope, childrenSt) < CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::LTE:
                return CalcExpr (curScope, childrenSt) <= CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::GTE:
                return CalcExpr (curScope, childrenSt) >= CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::AND:
                return CalcExpr (curScope, childrenSt) && CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::OR:
                return CalcExpr (curScope, childrenSt) || CalcExpr (curScope, std::next (childrenSt, 1));
            case AST::OperNode::OperType::UNARY_M:
                return -CalcExpr (curScope, childrenSt);
            case AST::OperNode::OperType::UNARY_P:
                return +CalcExpr (curScope, childrenSt);
            case AST::OperNode::OperType::CALL:
                return execCall (curScope, node);
            case AST::OperNode::OperType::ASSIGN:
                return assignment (curScope, node);
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
    int Interpreter::execCallUsingStack (Scope *newScope, AST::ScopeNode *ASTScope)
    {
        execScope (newScope, ASTScope);
        int retRes = callStack_.top ();
        callStack_.pop ();
        scopeExecution_ = true;
        return retRes;
    }

    int Interpreter::execCallUsingStack (Scope *newScope, AST::FuncNode *funcDecl)
    {
        execScope (newScope, static_cast<AST::ScopeNode *> (funcDecl->getRightChild ()));
        int retRes = callStack_.top ();
        callStack_.pop ();
        scopeExecution_ = true;
        return retRes;
    }

    AST::FuncNode *getFuncNameASTNode (AST::FuncNode *funcDecl)
    {
        auto funcSt = funcDecl->childBegin ();
        auto funcFin = funcDecl->childEnd ();

        while (funcSt != funcFin) {
            if ((*funcSt)->getType () == AST::NodeT::FUNCTION) {
                AST::FuncNode *funcStNode = static_cast<AST::FuncNode *> (*funcSt);
                if (funcStNode->getFuncCompType () == AST::FuncNode::FuncComponents::FUNC_NAME)
                    return funcStNode;
            }

            ++funcSt;
        }

        return nullptr;
    }

    AST::FuncNode *getFuncArgsASTNode (AST::FuncNode *funcDecl)
    {
        auto funcSt = funcDecl->childBegin ();
        auto funcFin = funcDecl->childEnd ();

        while (funcSt != funcFin) {
            if ((*funcSt)->getType () == AST::NodeT::FUNCTION) {
                AST::FuncNode *funcStNode = static_cast<AST::FuncNode *> (*funcSt);
                if (funcStNode->getFuncCompType () == AST::FuncNode::FuncComponents::FUNC_ARGS)
                    return funcStNode;
            }

            ++funcSt;
        }

        return nullptr;
    }

    int Interpreter::createNewScope (Scope *newScope, AST::FuncNode *funcName, AST::FuncNode *funcArgs, AST::FuncNode *funcDecl, AST::OperNode *callNode, Scope *curScope)
    {
        if (funcName) {
            const std::string &insideName = static_cast<AST::VarNode *> (funcName->getRightChild ())->getName ();
            FuncObject *insideFunc = new FuncObject{funcDecl};

            newScope->add (insideName, insideFunc);
        }

        auto argsSt = funcArgs->childBegin ();  //TODO
        auto argsFin = funcArgs->childEnd ();

        AST::FuncNode *funcArgsVal = static_cast<AST::FuncNode *> (callNode->getRightChild ());

        auto childSt = funcArgsVal->childBegin ();
        auto childFin = funcArgsVal->childEnd ();

        while (argsSt != argsFin) {  //TODO compare sizes

            AST::VarNode *forGetName = static_cast<AST::VarNode *> (*argsSt);
            const std::string &name = forGetName->getName ();

            Variable<int> *newVar = new Variable<int>{CalcExpr (curScope, childSt)};

            newScope->add (name, newVar);

            ++argsSt;
            ++childSt;
        }
    }

    int Interpreter::execRealCall (Scope *curScope, Wrapper *obj, AST::OperNode *callNode)
    {
        FuncObject *funcObj = static_cast<FuncObject *> (obj);
        AST::FuncNode *funcDecl = funcObj->getNode ();

        Tree::NAryTree<Scope *> scopeTree;
        Scope *newScope = new Scope;
        scopeTree.setRoot (newScope);

        AST::FuncNode *funcArgs = getFuncArgsASTNode (funcDecl);
        AST::FuncNode *funcName = getFuncNameASTNode (funcDecl);

        createNewScope (newScope, funcName, funcArgs, funcDecl, callNode, curScope);

        return execCallUsingStack (newScope, funcDecl);
    }

    int Interpreter::execCall (Scope *curScope, AST::OperNode *callNode)
    {
        AST::VarNode *varNode = static_cast<AST::VarNode *> (*(callNode->childBegin ()));

        Wrapper *obj = curScope->lookup (varNode->getName ());
        if (obj->type_ == DataType::FUNC)
            return execRealCall (curScope, obj, callNode);
        else {
            //TODO ERROR
        }
    }

    int Interpreter::CalcScope (AST::ScopeNode *node)
    {
        Tree::NAryTree<Scope *> toDestruct;
        Scope *newScope = new Scope;
        toDestruct.setRoot (newScope);

        return execCallUsingStack (newScope, node);
    }

    int Interpreter::CalcExpr (Scope *curScope, std::vector<AST::Node *>::const_iterator it)
    {
        AST::Node *node = *it;
        switch (node->getType ()) {
            case AST::NodeT::VARIABLE:
                return CalcVar (curScope, static_cast<AST::VarNode *> (node));
            case AST::NodeT::NUMBER:
                return static_cast<AST::NumNode *> (node)->getValue ();
            case AST::NodeT::OPERATOR:
                return CalcOper (curScope, static_cast<AST::OperNode *> (node));
            case AST::NodeT::SCOPE:
                return CalcScope (static_cast<AST::ScopeNode *> (node));
            default:
                throw std::runtime_error ("Unexpected node type");
        }
    }

    AST::FuncNode *FuncNodeLookUp (AST::OperNode *node)
    {
        AST::Node *rightChild = node;
        while (node->getOpType () == AST::OperNode::OperType::ASSIGN) {
            rightChild = node->getRightChild ();
            if (rightChild->getType () == AST::NodeT::OPERATOR)
                node = static_cast<AST::OperNode *> (rightChild);
            else
                break;
        }

        if (rightChild->getType () == AST::NodeT::FUNCTION)
            return static_cast<AST::FuncNode *> (rightChild);
        else
            return nullptr;
    }

    int Interpreter::assignment (Scope *curScope, AST::OperNode *node)
    {
        auto childrenSt = node->childBegin ();
        AST::VarNode *leftN = static_cast<AST::VarNode *> (*childrenSt);
        const std::string &name = leftN->getName ();

        Wrapper *findObj = curScope->lookup (name);

        AST::FuncNode *funcNodeDecl = FuncNodeLookUp (node);
        if (funcNodeDecl) {
            if (!findObj) {
                FuncObject *newFuncObj = new FuncObject (funcNodeDecl);
                curScope->add (name, newFuncObj);
            }
            else {
            }
        }
        else {
            int val = CalcExpr (curScope, std::next (childrenSt, 1));  //Now we have only int type... In the near future it shall be template

            if (!findObj) {
                Variable<int> *newVar = new Variable<int> (val);
                curScope->add (name, newVar);
            }
            else {                                                                 //TODO: Checking for a type
                Variable<int> *clearVar = static_cast<Variable<int> *> (findObj);  //Only int type...
                clearVar->setVal (val);
            }
            return val;
        }
    }

    void Interpreter::print (Scope *curScope, AST::OperNode *node)
    {
        auto childrenSt = node->childBegin ();
        std::cout << CalcExpr (curScope, childrenSt) << std::endl;
    }

    void Interpreter::ret (Scope *curScope, AST::OperNode *node)
    {
        auto expr = node->childBegin ();
        callStack_.push (CalcExpr (curScope, expr));
        scopeExecution_ = false;
    }

    void Interpreter::execOper (Scope *curScope, AST::OperNode *node)
    {
        switch (node->getOpType ()) {
            case AST::OperNode::OperType::ASSIGN: {
                assignment (curScope, node);
                break;
            }
            case AST::OperNode::OperType::RETURN: {
                ret (curScope, node);
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
        auto childrenSt = node->childBegin ();
        if (CalcExpr (curScope, childrenSt)) {
            Scope *newScope = new Scope;
            curScope->add (newScope);
            execScope (newScope, static_cast<AST::ScopeNode *> (*(std::next (childrenSt, 1))));
        }
    }

    void Interpreter::execWhile (Scope *curScope, AST::CondNode *node)
    {
        auto childrenSt = node->childBegin ();
        while (CalcExpr (curScope, childrenSt)) {
            Scope *newScope = new Scope;
            curScope->add (newScope);
            execScope (newScope, static_cast<AST::ScopeNode *> (*(std::next (childrenSt, 1))));
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
        auto childrenSt = node->childBegin ();
        auto childrenFin = node->childEnd ();
        while (scopeExecution_ && childrenSt != childrenFin) {
            AST::Node *nodeToExec = *childrenSt;
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
            childrenSt = std::next (childrenSt, 1);
        }
    }

    void Interpreter::run ()
    {
        Tree::NAryTree<Scope *> *mainRoot = globalScope_;
        AST::ScopeNode *startNode = static_cast<AST::ScopeNode *> (tree_->getRoot ());
        if (startNode)
            execScope (mainRoot->getRoot (), startNode);
    }

}  // namespace interpret
