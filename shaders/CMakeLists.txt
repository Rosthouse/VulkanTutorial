list(APPEND CMAKE_PROGRAM_PATH "$ENV{VULKAN_SDK}/Bin32")

add_custom_target(
        SHADER
        ALL
        DEPENDS frag.spv vert.spv
)

file(REMOVE ${PROJECT_BINARY_DIR}/shaders/frag.spv ${PROJECT_BINARY_DIR}/shaders/vert.spv)

add_custom_command(
        OUTPUT ${PROJECT_BINARY_DIR}/shaders/frag.spv
        COMMAND glslangValidator ARGS -V ${PROJECT_SOURCE_DIR}/shaders/draw_triangle.frag
)
add_custom_command(
        OUTPUT ${PROJECT_BINARY_DIR}/Shaders/vert.spv
        COMMAND glslangValidator ARGS -V ${PROJECT_SOURCE_DIR}/shaders/draw_triangle.vert
)