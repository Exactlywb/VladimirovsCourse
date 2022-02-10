#ifndef COMPILER_H__
#define COMPILER_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "nAryTree.hpp"

namespace ParaCompiler {

    class ParaCLLVMCompiler {
        Tree::NAryTree<AST::Node *> *tree_;

        std::unique_ptr<llvm::LLVMContext> context_;
        std::unique_ptr<llvm::Module> module_;
        std::unique_ptr<llvm::IRBuilder<>> builder_;

        std::unordered_map<std::string, llvm::Value *> symbTbl_;

    public:
        ParaCLLVMCompiler (Tree::NAryTree<AST::Node *> *tree)
            : tree_ (tree),
              context_ (std::make_unique<llvm::LLVMContext> ()),
              module_ (std::make_unique<llvm::Module> ("paraCL", *context_)),
              builder_ (std::make_unique<llvm::IRBuilder<>> (*context_))
        {
        }

        void run ()
        {
            if (!tree_)
                throw std::runtime_error ("nullptr on AST::Tree* in compiler");

            std::cout << "LLVM here" << std::endl;

            // genScope ();
        }
    };

}  // namespace ParaCompiler

#endif
