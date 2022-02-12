#ifndef INTERPRET_H__
#define INTERPRET_H__

#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <list>

#include "ast.hpp"
#include "location.hh"
#include "nAryTree.hpp"

namespace interpret {

    class ASTListConveyor  {

        std::list<AST::Node*> conveyor_;
        virtual ~ASTListConveyor() = 0;
    public:
        void buildConveyor (AST::ScopeNode* startNode);
        void runConveyor   ();
    };

    class Intepreter final : public ASTListConveyor{

        using listIt = std::list<AST::Node*>::const_iterator; //Sorry
        std::stack<listIt> retStack_;

        Intepreter (Tree::NAryTree<AST::Node*> &tree) {

            buildConveyor (static_cast<AST::ScopeNode*>(tree.getRoot()));
        }

        void run () {

            runConveyor();
        }
    };
}  // namespace interpret

#endif
