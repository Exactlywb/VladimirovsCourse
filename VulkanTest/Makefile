CXX = g++
CXXFLAGS = -std=c++17 -O2 -o $@
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr

triangle.out: main.cpp lib/triangle.h lib/triangle.hpp vulkan/vulkan.cpp
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -DNDEBUG
	make shaders

triv_triangle.out: main.cpp lib/triangle.h lib/triangle.hpp vulkan/vulkan.cpp
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -DNDEBUG -DTRIV
	make shaders	

check.out: tmain.cpp lib/triangle.h lib/triangle.hpp
	$(CXX) $(CXXFLAGS) $< 

debug.out: main.cpp lib/triangle.h lib/triangle.hpp vulkan/vulkan.cpp
	$(CXX) $(CXXFLAGS) -g $< $(LDFLAGS) -DPRINT
	make shaders

shaders: vulkan/shaders/shader.vert vulkan/shaders/shader.frag
	./vulkan/shaders/compile.sh
