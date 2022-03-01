#ifndef AST_H__
#define AST_H__

#include <iostream>
#include <string>
#include <vector>

#include "locator.hpp"

//*****************************************************************************
//************************* AST NODE DESCRIPTION HERE *************************
//*****************************************************************************
namespace AST {

    enum class NodeT {

        VARIABLE,
        NUMBER,
        OPERATOR,

        CONDITION,
        FUNCTION,

        SCOPE,
        INLINESCOPE,
        FILLER

    };

    class Node : public NodeLocator {
        Node *parent_;
        std::vector<Node *> children_;
        const NodeT type_;

    public:
        Node (const NodeT type, Node *parent = nullptr) : type_ (type), parent_ (parent) {}

        Node (const NodeT type, yy::location loc, Node *parent = nullptr)
            : NodeLocator (loc), type_ (type), parent_ (parent)
        {
        }

        virtual ~Node () = default;

        Node *operator[] (int num) { return children_[num]; }
        const Node *operator[] (int num) const { return children_[num]; }
        // Let's implement 0-rule
        Node (const Node &other) = delete;
        Node (Node &&other) = delete;
        Node &operator= (const Node &other) = delete;
        Node &operator= (Node &&other) = delete;

        // If you'd like to inherit from class Node you have to write NodeDump () func;
        virtual void nodeDump (std::ostream &out) const = 0;

        // Setters and getters

        NodeT getType () const { return type_; }

        template <typename It>
        void setChildren (It start, It fin)
        {
            children_.assign (start, fin);
        }

        std::vector<Node *>::const_iterator childBegin () const { return children_.cbegin (); }

        std::vector<Node *>::const_iterator childEnd () const { return children_.cend (); }

        Node *getLeftChild () const
        {
            size_t vecSize = children_.size ();
            if (vecSize == 0)
                return nullptr;

            return children_[0];
        }

        Node *getRightChild () const
        {
            size_t vecSize = children_.size ();
            if (vecSize == 0)
                return nullptr;

            return children_[children_.size () - 1];
        }

        size_t getChildrenNum () const { return children_.size (); }

        void setParent (Node *parent) { parent_ = parent; }

        Node *getParent () const { return parent_; }

        void addChild (Node *child)
        {
            children_.push_back (child);
            child->setParent (this);
        }

        std::vector<Node *>::iterator eraseChild (const int ind)
        {  //|!| THIS FUNCTION DOESN'T CHANGE parent_ FOR CHILD NODE
            return children_.erase (children_.begin () + ind);
        }
    };

}  // namespace AST

//*****************************************************************************
//**************************** AST Node inheritors ****************************
//*****************************************************************************
namespace AST {
    
    class Filler final : public Node {

    public:
        Filler (Node *parent = nullptr)
            : Node (NodeT::FILLER, parent)
        {
        }

        void nodeDump (std::ostream &out) const override { out << "FILLER"; }
    };

    class VarNode final : public Node {
        std::string name_;

    public:
        VarNode (const std::string &name, yy::location loc, Node *parent = nullptr)
            : Node (NodeT::VARIABLE, loc, parent), name_ (name)
        {
        }

        void nodeDump (std::ostream &out) const override { out << name_; }

        std::string getName () const { return name_; }
    };

    class FuncNode final : public Node {
    public:
        enum class FuncComponents {

            FUNC_DECL,
            FUNC_ARGS,
            FUNC_NAME

        };

    private:
        FuncComponents compType_;

    public:
        FuncNode (const FuncComponents compType, yy::location loc, Node *parent = nullptr)
            : Node (NodeT::FUNCTION, loc, parent), compType_ (compType)
        {
        }

        FuncComponents getFuncCompType () const { return compType_; }

        void nodeDump (std::ostream &out) const override
        {
            switch (compType_) {
                case FuncComponents::FUNC_DECL: out << "FUNC_DECL"; break;
                case FuncComponents::FUNC_ARGS: out << "FUNC_ARGS"; break;
                case FuncComponents::FUNC_NAME: out << "FUNC_NAME"; break;
            }
        }
    };

    class OperNode final : public Node {
    public:
        enum class OperType;

    private:
        OperType opType_;

    public:
        enum class OperType {

            ADD,  // a + b
            SUB,  // a - b
            MUL,  // a * b
            DIV,  // a / b
            MOD,  // a % b

            UNARY_M,  // -a
            UNARY_P,  // +a

            ASSIGN,  // a = b

            MORE,  // a > b
            LESS,  // a < b
            EQ,    // a == b
            NEQ,   // a != b
            GTE,   // a >= b
            LTE,   // a <= b

            AND,  // a && b
            OR,   // a || b

            SCAN,   // a = ?
            PRINT,  // print (a)

            RETURN,  // return ...
            CALL

        };

        OperNode (const OperType opType, yy::location loc, Node *parent = nullptr)
            : Node (NodeT::OPERATOR, loc, parent), opType_ (opType)
        {
        }

        OperNode (const OperType opType, Node *parent = nullptr) : Node (NodeT::OPERATOR, parent), opType_ (opType) {}

        OperType getOpType () const { return opType_; }

        void nodeDump (std::ostream &out) const override
        {
            switch (opType_) {
                case OperType::ADD: out << "ADD (+)"; break;
                case OperType::SUB: out << "SUB (-)"; break;
                case OperType::UNARY_M: out << "UNARY_M (-)"; break;
                case OperType::UNARY_P: out << "UNARY_P (+)"; break;
                case OperType::MUL: out << "MUL (*)"; break;
                case OperType::DIV: out << "DIV (/)"; break;
                case OperType::ASSIGN: out << "ASSIGN (=)"; break;
                case OperType::MORE: out << "MORE (>)"; break;
                case OperType::LESS: out << "LESS (<)"; break;
                case OperType::EQ: out << "EQ (==)"; break;
                case OperType::NEQ: out << "NEQ (!=)"; break;
                case OperType::GTE: out << "GTE (>=)"; break;
                case OperType::LTE: out << "LTE (<=)"; break;
                case OperType::SCAN: out << "SCAN (?)"; break;
                case OperType::OR: out << "OR (||)"; break;
                case OperType::AND: out << "AND (&&)"; break;
                case OperType::MOD: out << "MOD (%)"; break;
                case OperType::PRINT: out << "PRINT [print ()]"; break;
                case OperType::RETURN: out << "RETURN"; break;
                case OperType::CALL: out << "CALL"; break;
                default: out << "Unexpected operator type!";
            }
        }
    };

    class NumNode final : public Node {
        int value_;

    public:
        NumNode (const int value = 0, Node *parent = nullptr) : Node (NodeT::NUMBER, parent), value_ (value) {}

        void nodeDump (std::ostream &out) const override { out << value_; }

        int getValue () const { return value_; }
    };

    class ScopeNode final : public Node {
    public:
        ScopeNode (Node *parent = nullptr) : Node (NodeT::SCOPE, parent) {}
        ScopeNode (yy::location loc, Node *parent = nullptr) : Node (NodeT::SCOPE, loc, parent) {}

        void nodeDump (std::ostream &out) const override { out << "SCOPE"; }
    };

    class InlineScopeNode final : public Node {
    public:
        InlineScopeNode (Node *parent = nullptr) : Node (NodeT::INLINESCOPE, parent) {}

        void nodeDump (std::ostream &out) const override { out << "INLINESCOPE"; }
    };

    class CondNode final : public Node {
    public:
        enum class ConditionType {

            IF,
            WHILE

        };

    private:
        ConditionType condType_;

    public:
        CondNode (const ConditionType condType, yy::location loc, Node *parent = nullptr)
            : Node (NodeT::CONDITION, loc, parent), condType_ (condType)
        {
        }

        ConditionType getConditionType () const { return condType_; }

        void nodeDump (std::ostream &out) const override
        {
            switch (condType_) {
                case ConditionType::WHILE: out << "WHILE"; break;
                case ConditionType::IF: out << "IF"; break;
                default: out << "Unexpected condition type!";
            }
        }
    };

}  // namespace AST

#endif
