file(GLOB_RECURSE GLSL_SOURCE_FILES "*.frag" "*.vert")

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.inc")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV} -mfmt=c
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARIES})

