#include "ast.hpp"

AST::Tree::~Tree () {

    AST::Node* curNode = root_;

    while (curNode) {

again:
        std::vector<AST::Node*> children = curNode->getChildren ();
        size_t childrenNum = children.size ();

        for (size_t i = 0; i < childrenNum; ++i) {

            AST::Node* curChild = children [i];
            if (curChild) { //!TODO useless if?

                curNode->eraseChild (i);
                curNode = curChild;
                goto again;                 //Try to do better

            }

        }

        Node* parent = curNode->getParent ();
        delete curNode;
        curNode = parent;
        
    }
    
}

namespace {

    void PrintNodeIntoGraphviz (AST::Node* curNode, std::ostream& out) {

        out << "\"" << curNode << "\" [label = \"";
        curNode->nodeDump (out);
        out << "\"]\n";

        std::vector<AST::Node*> children = curNode->getChildren ();
        size_t childrenNum = children.size ();
        for (int i = 0; i < childrenNum; ++i) {
            
            AST::Node* curChild = children [i];
            if (curChild)
                PrintNodeIntoGraphviz (curChild, out);

        }

    }

    void BuildConnectionsInGraphviz (AST::Node* curNode, std::ostream& out) {

        std::vector<AST::Node*> children = curNode->getChildren ();
        size_t childrenNum = children.size ();
        for (int i = 0; i < childrenNum; ++i) {
            
            AST::Node* curChild = children [i];
            if (curChild)
                out << "\"" << curNode << "\" -> \"" << curChild << "\"\n";

        }

        for (int i = 0; i < childrenNum; ++i) {
            
            AST::Node* curChild = children [i];
            if (curChild)
                BuildConnectionsInGraphviz (curChild, out);

        }

    }

}

void AST::Tree::dump (std::ostream& out) const {    //!TODO more informative dump

    if (!root_)
        return;
    
    out <<  "digraph tree {\n"
            "rankdir = \"LR\"\n"
            "node [fontsize=10, shape=box, height=0.5]\n"
            "edge [fontsize=10]\n";
    
    PrintNodeIntoGraphviz       (root_, out);
    BuildConnectionsInGraphviz  (root_, out);

    out << "}\n";

}
