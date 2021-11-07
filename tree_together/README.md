# TREE
==================================================================
# How to build 
```
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make
```
## Compiler settings:)
```
  $ g++ -Wall -Wextra -Wpedantic -Wsign-promo 
    -Wsign-conversion -Wdouble-promotion
    -Wfloat-equal -Wbool-compire -Wdiv-by-zero
    -O2 
```
---
---
### In a folder 'build' an executiable file 'tree' and  test-file 'test_tree' will appear
---
## Steps of implementation:
* Build a Red-Black tree with an addition filed named **'subtreeSize'** --- the size of subtree of the node
* Input queries from the ***stdin***
* Execute every query, remember it in a vector and print the   result to ***stdout***  
---
## Some words about Red-Black-Tree
### A Red-Black Tree is a tree with the next invariant:
* Every node is either <H style="background:#FFFFFF"> <span style="color:#FF0000">Red</span></H> or <H style="background:#FFFFFF"> <span style="color:#000000">Black</span></H>
* Root is <H style="background:#FFFFFF"> <span style="color:#000000">Black</span></H>
* If node is <H style="background:#FFFFFF"> <span style="color:#FF0000">Red</span></H> then its childs are <H style="background:#FFFFFF"> <span style="color:#000000">Black</span></H>
* For each node all simple paths to leaves-descendants include the same amount of <H style="background:#FFFFFF"> <span style="color:#000000">Black</span></H> nodes
### The example of Red-Black Tree: 
