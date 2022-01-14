#include "ast.hpp"

#include <vector>
#include <stack>

AST::Tree::~Tree ()
{
    AST::Node *curNode = root_;
    if (!root_)
        return;

    std::stack<AST::Node *> stack;
    std::vector<AST::Node *> queueOnDelete;
    stack.push (curNode);

    while (stack.size ()) {
        curNode = stack.top ();
        stack.pop ();
        queueOnDelete.push_back (curNode);

        for (auto &child : curNode->getChildren ())
            stack.push (child);
    }

    for (int i = queueOnDelete.size () - 1; i >= 0; --i) {
        delete queueOnDelete[i];
    }
}

namespace {

    void PrintNodeIntoGraphviz (AST::Node *curNode, std::ostream &out)
    {
        out << "\"" << curNode << "\" [label = \"";
        curNode->nodeDump (out);
        out << "\"]\n";

        std::vector<AST::Node *> children = curNode->getChildren ();
        size_t childrenNum = children.size ();
        for (int i = 0; i < childrenNum; ++i) {
            AST::Node *curChild = children[i];
            if (curChild)
                PrintNodeIntoGraphviz (curChild, out);
        }
    }

    void BuildConnectionsInGraphviz (AST::Node *curNode, std::ostream &out)
    {
        std::vector<AST::Node *> children = curNode->getChildren ();
        size_t childrenNum = children.size ();
        for (int i = 0; i < childrenNum; ++i) {
            AST::Node *curChild = children[i];
            if (curChild)
                out << "\"" << curNode << "\" -> \"" << curChild << "\"\n";
        }

        for (int i = 0; i < childrenNum; ++i) {
            AST::Node *curChild = children[i];
            if (curChild)
                BuildConnectionsInGraphviz (curChild, out);
        }
    }

}  // namespace

void AST::Tree::dump (std::ostream &out) const
{  //!TODO more informative dump

    if (!root_)
        return;

    out << "digraph tree {\n"
           "rankdir = \"LR\"\n"
           "node [fontsize=10, shape=box, height=0.5]\n"
           "edge [fontsize=10]\n";

    PrintNodeIntoGraphviz (root_, out);
    BuildConnectionsInGraphviz (root_, out);

    out << "}\n";
}
