
set(ENV{VULKAN_SDK} CACHE FILEPATH)

list(APPEND CMAKE_PROGRAM_PATH "C:/Frameworks/VulkanSDK/1.0.57.0/Bin32")

add_custom_target(
        Build_Shaders
        DEPENDS frag.spv vert.spv
)

add_custom_command(
        OUTPUT frag.spv
        COMMAND glslangValidator ARGS -V -h draw_triangle.frag
)

add_custom_command(
        OUTPUT vert.spv
        COMMAND glslangValidator ARGS -V -h draw_triangle.vert
)