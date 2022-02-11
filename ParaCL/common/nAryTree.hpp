#ifndef __NARYTREE__
#define __NARYTREE__

#include <iostream>
#include <stack>
#include <vector>

namespace Tree {

    template <typename T>
    class NAryTree final {
        T root_ = nullptr;

        void PrintNodeIntoGraphviz (T curNode, std::ostream &out) const
        {
            out << "\"" << curNode << "\" [label = \"";
            curNode->nodeDump (out);
            out << "\"]\n";

            for (auto it = curNode->childBegin (); it != curNode->childEnd (); ++it) {
                T curChild = *it;
                if (curChild)
                    PrintNodeIntoGraphviz (curChild, out);
            }
        }

        void BuildConnectionsInGraphviz (T curNode, std::ostream &out) const
        {
            for (auto it = curNode->childBegin (); it != curNode->childEnd (); ++it) {
                T curChild = *it;
                if (curChild)
                    out << "\"" << curNode << "\" -> \"" << curChild << "\"\n";
            }

            for (auto it = curNode->childBegin (); it != curNode->childEnd (); ++it) {
                T curChild = *it;
                if (curChild)
                    BuildConnectionsInGraphviz (curChild, out);
            }
        }

    public:
        NAryTree (T root = nullptr) : root_ (root) {}

        ~NAryTree ()
        {
            T curNode = root_;
            if (!root_)
                return;

            std::stack<T> stack;
            std::vector<T> queueOnDelete;
            stack.push (curNode);

            while (stack.size ()) {
                curNode = stack.top ();
                stack.pop ();
                queueOnDelete.push_back (curNode);

                auto childrenSt = curNode->childBegin ();
                auto childrenFin = curNode->childEnd ();

                while (childrenSt != childrenFin) {
                    stack.push (*childrenSt);
                    childrenSt = std::next (childrenSt, 1);
                }
            }

            for (int i = queueOnDelete.size () - 1; i >= 0; --i)
                delete queueOnDelete[i];
        }

        // Rule of 0
        NAryTree (const NAryTree &) = delete;
        NAryTree (NAryTree &&) = delete;
        NAryTree &operator= (const NAryTree &) = delete;
        NAryTree &operator= (NAryTree &&) = delete;

        void dump (std::ostream &out) const
        {
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

        // Setters and getters
        void setRoot (T root) { root_ = root; }

        T getRoot () const { return root_; }
    };
}  // namespace Tree

#endif