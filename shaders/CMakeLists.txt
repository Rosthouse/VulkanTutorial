configure_file(frag.spv frag.spv COPYONLY)
configure_file(vert.spv vert.spv COPYONLY)

set(VULKAN_SDK "F:/SDKs/LunarGVulkan/1.0.57.0/Bin32" CACHE FILEPATH "Path to the lunar sdk")

list(APPEND CMAKE_PROGRAM_PATH "F:/SDKs/LunarGVulkan/1.0.57.0/Bin32")

add_custom_target(
        SHADER
        DEPENDS frag.spv vert.spv
)
add_custom_command(
        OUTPUT frag.spv
        COMMAND glslangValidator ARGS -V draw_triangle.frag
)
add_custom_command(
        OUTPUT vert.spv
        COMMAND glslangValidator ARGS -V draw_triangle.vert
)