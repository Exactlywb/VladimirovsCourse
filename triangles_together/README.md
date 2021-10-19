# How to build a project
```
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make
```
## Compiler settings:
```
  $ g++ -Wall -Wextra -O2
```
---
---
### In a folder 'build' a file 'triangles' will appear executing a programm
---
---
---
# Some words about the algorithm
### The task requires to output the numbers of intersected triangles recieved from stdin. To do it faster then with naive implementation with O(n^2) we use octree to divide space into 8 parts. Then we find intersections between triangles in the same part or between triangles lied in the several parts at the same time. 
---
## Useful materials: 
* https://habr.com/ru/post/334990/ ---> the article about the Octree
*  **Geometric Tools for Computer Graphics** by Philip Schneider and David Eberly [2003]