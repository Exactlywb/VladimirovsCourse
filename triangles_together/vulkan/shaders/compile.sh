cwd=$(pwd)
/usr/local/bin/glslc ${cwd}/../vulkan/shaders/shader.vert -o vert.spv
/usr/local/bin/glslc ${cwd}/../vulkan/shaders/shader.frag -o frag.spv