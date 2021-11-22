cwd=$(pwd)
/usr/local/bin/glslc ${cwd}/../vulkan/shaders/shader.vert -o ${cwd}/../vulkan/shaders/vert.spv
/usr/local/bin/glslc ${cwd}/../vulkan/shaders/shader.frag -o ${cwd}/../vulkan/shaders/frag.spv
