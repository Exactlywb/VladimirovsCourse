file(GLOB_RECURSE GLSL_SOURCE_FILES "*.frag" "*.vert")

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}.spv")

  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARIES})

