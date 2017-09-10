cmake_minimum_required(VERSION 3.8)
project(Vulkan)


# Configuring application
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp HelloTriangleApplication.cpp HelloTriangleApplication.h)
set(Vulkan_Tutorial_VERSION_MAJOR 1)
set(Vulkan_Tutorial_VERSION_MINOR 0)

# Configuring file



# Adding subdirectory
add_subdirectory(shaders)

# Adding headers to application
include_directories(F:/SDKs/LunarGVulkan/1.0.57.0/Include)
include_directories(${PROJECT_SOURCE_DIR}/GLFW/include)
include_directories(${PROJECT_SOURCE_DIR}/GLM)

# Linking
link_directories(${PROJECT_SOURCE_DIR}/GLFW/lib-mingw/)
link_directories(F:/SDKs/LunarGVulkan/1.0.57.0/Lib32/)

add_executable(Vulkan ${SOURCE_FILES} ${SHADER_FILES})

target_link_libraries(Vulkan glfw3 vulkan-1)
