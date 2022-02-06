#include "interpreter.hpp"

namespace interpret {
    using tblIt = std::unordered_map<std::string, Wrapper *>::iterator;
    using vecASTIt = std::vector<AST::Node *>::const_iterator;

    namespace {

        void funcDeclAssignment (Scope *curScope, yy::location errloc, const std::string &name, Scope *findScope, Scope::tblIt obj, FuncObject *newFuncObj)
        {
            if (obj == curScope->tbl_.end ())
                curScope->add (name, newFuncObj);
            else {
                if ((*obj).second->type_ == DataType::VAR)
                    throw ErrorDetector ("type conflicts", errloc);

                findScope->tbl_.erase (obj);
                curScope->add (name, newFuncObj);
            }
        }

        int varDeclAssignment (Scope *curScope, yy::location errloc, const std::string &name, int val, Scope::tblIt obj)
        {
            if (obj == curScope->tbl_.end ()) {
                Variable<int> *newVar = new Variable<int> (val);
                curScope->add (name, newVar);
            }
            else {
                if ((*obj).second->type_ == DataType::FUNC) {
                    throw ErrorDetector ("type conflicts", errloc);
                }
                Variable<int> *clearVar = static_cast<Variable<int> *> ((*obj).second);  //Only int type...
                clearVar->setVal (val);
            }
            return val;
        }

        AST::FuncNode *getFuncComponentASTNode (AST::FuncNode *funcDecl, AST::FuncNode::FuncComponents type)
        {
            auto funcFin = funcDecl->childEnd ();

            for (auto funcSt = funcDecl->childBegin (); funcSt != funcFin; ++funcSt) {
                if ((*funcSt)->getType () == AST::NodeT::FUNCTION) {
                    AST::FuncNode *funcStNode = static_cast<AST::FuncNode *> (*funcSt);
                    if (funcStNode->getFuncCompType () == type)
                        return funcStNode;
                }
            }

            return nullptr;
        }

        void addFuncNameIntoNewScope (Scope *newScope, AST::FuncNode *funcName, AST::FuncNode *funcDecl)
        {
            if (funcName) {
                const std::string &insideName = static_cast<AST::VarNode *> (funcName->getRightChild ())->getName ();
                FuncObject *insideFunc = new FuncObject{funcDecl};

                newScope->add (insideName, insideFunc);
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

        void catchFunctionNamesake (Scope *curScope, AST::FuncNode *funcNodeDecl)
        {
            AST::FuncNode *funcNameASTNode = getFuncComponentASTNode (funcNodeDecl, AST::FuncNode::FuncComponents::FUNC_NAME);
            if (funcNameASTNode) {
                const std::string &funcName = static_cast<AST::VarNode *> (funcNameASTNode->getLeftChild ())->getName ();
                std::pair<Scope *, Scope::tblIt> findNamesakeFunc = curScope->smartLookup (funcName);
                if (findNamesakeFunc.second != curScope->tbl_.end ())
                    throw ErrorDetector ("function with the same name already exists", funcNameASTNode->getLocation ());
            }
        }
    }  // namespace

    Scope::~Scope ()
    {
        for (auto i : tbl_)
            delete i.second;
    }

    std::pair<Scope *, tblIt> Scope::smartLookup (const std::string &name)
    {
        Scope *curScope = this;
        while (curScope) {
            auto elem = curScope->tbl_.find (name);
            if (elem != curScope->tbl_.end ())
                return {curScope, elem};

            curScope = curScope->parent_;
        }

        return {nullptr, tbl_.end ()};
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
        auto findElem = curScope->smartLookup (var->getName ());
        if (findElem.first == nullptr)
            throw ErrorDetector ("undeclared variable in an expression", var->getLocation ());

        Wrapper *varWrapper = (*findElem.second).second;

        if (varWrapper->type_ == DataType::FUNC)
            throw ErrorDetector ("wrong syntax for function calling", var->getLocation ());

        Variable<int> *clearVar = static_cast<Variable<int> *> (varWrapper);

        return clearVar->getVal ();
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
                throw ErrorDetector ("unexpected operator type in calculation", node->getLocation ());
            }
        }
    }

    int Interpreter::getCallValue ()
    {
        int retRes = callStack_.top ();
        callStack_.pop ();
        scopeExecution_ = true;
        return retRes;
    }

    int Interpreter::execCallUsingStack (Scope *newScope, AST::ScopeNode *ASTScope)
    {
        execScope (newScope, ASTScope);
        return getCallValue ();
    }

    int Interpreter::execCallUsingStack (Scope *newScope, AST::FuncNode *funcDecl)
    {
        ++recursionController_;
        if (recursionController_.isOverflow ())
            throw ErrorDetector ("stack overflow", funcDecl->getLocation ());

        execScope (newScope, static_cast<AST::ScopeNode *> (funcDecl->getRightChild ()));
        --recursionController_;

        return getCallValue ();
    }

    void Interpreter::createNewScope (Scope *newScope, AST::FuncNode *funcName, AST::FuncNode *funcArgs, AST::FuncNode *funcDecl, AST::OperNode *callNode, Scope *curScope)
    {
        addFuncNameIntoNewScope (newScope, funcName, funcDecl);

        auto callParamsSt = funcArgs->childBegin ();
        auto callParamsFin = funcArgs->childEnd ();

        AST::FuncNode *funcArgsVal = static_cast<AST::FuncNode *> (callNode->getRightChild ());

        auto funcArgsSt = funcArgsVal->childBegin ();
        auto funcArgsFin = funcArgsVal->childEnd ();

        if (std::distance (callParamsSt, callParamsFin) != std::distance (funcArgsSt, funcArgsFin))
            throw ErrorDetector ("wrong number of arguments for a function calling", callNode->getLocation ());

        while (callParamsSt != callParamsFin) {
            AST::VarNode *forGetName = static_cast<AST::VarNode *> (*callParamsSt);
            const std::string &name = forGetName->getName ();

            Variable<int> *newVar = new Variable<int>{CalcExpr (curScope, funcArgsSt)};

            newScope->add (name, newVar);

            ++callParamsSt;
            ++funcArgsSt;
        }
    }

    int Interpreter::execRealCall (Scope *curScope, Wrapper *obj, AST::OperNode *callNode)
    {
        FuncObject *funcObj = static_cast<FuncObject *> (obj);
        AST::FuncNode *funcDecl = funcObj->getNode ();

        Tree::NAryTree<Scope *> scopeTree;
        Scope *newScope = new Scope;
        scopeTree.setRoot (newScope);

        AST::FuncNode *funcArgs = getFuncComponentASTNode (funcDecl, AST::FuncNode::FuncComponents::FUNC_ARGS);
        AST::FuncNode *funcName = getFuncComponentASTNode (funcDecl, AST::FuncNode::FuncComponents::FUNC_NAME);

        createNewScope (newScope, funcName, funcArgs, funcDecl, callNode, curScope);

        return execCallUsingStack (newScope, funcDecl);
    }

    int Interpreter::execCall (Scope *curScope, AST::OperNode *callNode)
    {
        AST::VarNode *varNode = static_cast<AST::VarNode *> (*(callNode->childBegin ()));

        auto nameLookup = curScope->smartLookup (varNode->getName ());
        if (nameLookup.first == nullptr)
            throw ErrorDetector ("can't find a function for a call", callNode->getLocation ());

        Wrapper *obj = (*(nameLookup.second)).second;
        if (obj->type_ == DataType::FUNC)
            return execRealCall (curScope, obj, callNode);

        throw ErrorDetector ("object is not a function", callNode->getLocation ());
    }

    int Interpreter::CalcScope (AST::ScopeNode *node)
    {
        Tree::NAryTree<Scope *> toDestruct;
        Scope *newScope = new Scope;
        toDestruct.setRoot (newScope);

        return execCallUsingStack (newScope, node);
    }

    int Interpreter::CalcExpr (Scope *curScope, vecASTIt it)
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
                throw std::runtime_error ("unexpected statement to execute");
        }
    }

    int Interpreter::assignment (Scope *curScope, AST::OperNode *node)
    {
        const int poison = -666;
        auto childrenSt = node->childBegin ();
        AST::VarNode *leftN = static_cast<AST::VarNode *> (*childrenSt);
        const std::string &name = leftN->getName ();

        std::pair<Scope *, Scope::tblIt> findObj = curScope->smartLookup (name);
        Scope::tblIt obj = findObj.second;
        AST::FuncNode *funcNodeDecl = FuncNodeLookUp (node);

        if (funcNodeDecl) {
            catchFunctionNamesake (curScope, funcNodeDecl);

            FuncObject *newFuncObj = new FuncObject (funcNodeDecl);
            funcDeclAssignment (curScope, node->getLocation (), name, findObj.first, obj, newFuncObj);

            return poison;
        }
        else {
            int val = CalcExpr (curScope, std::next (childrenSt, 1));  //Now we have only int type... In the near future it shall be template
            return varDeclAssignment (curScope, node->getLocation (), name, val, obj);
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
            default:
                throw ErrorDetector ("unexpected operator to execute", node->getLocation ());
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
                throw ErrorDetector ("unexpected condition statement type", node->getLocation ());
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
