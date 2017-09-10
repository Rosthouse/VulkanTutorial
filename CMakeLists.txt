cmake_minimum_required(VERSION 3.8)
project(Vulkan)


# Configuring application
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp HelloTriangleApplication.cpp HelloTriangleApplication.h)
set(Vulkan_Tutorial_VERSION_MAJOR 1)
set(Vulkan_Tutorial_VERSION_MINOR 0)

set(ENV{VULKAN_SDK} "F:/SDKs/LunarGVulkan/1.0.57.0" CACHE FILEPATH)
set(ENV{GLFW_SDK} "F:/SDKs/GLFW/glfw-3.2.1.bin.WIN32" CACHE FILEPATH)
set(ENV{GLM_SDK} "F:/SDKs/GLM/glm-0.9.9-a1" CACHE FILEPATH)


# Finding libraries
find_package(VULKAN_PKG REQUIRED)


find_library(VULKAN_LIB vulkan-1)
find_library(
        GLFW_LIB
        glfw3
        HINTS ${GLFW_SDK})

# Configuring file

# Adding subdirectory
add_subdirectory(shaders)

# Adding headers to application
target_include_directories(
        ${VULKAN_SDK}/Include/
        ${GLFW_SDK}/include/
        ${GLM_SDK}/glm/
)

add_executable(Vulkan ${SOURCE_FILES} ${SHADER_FILES})

target_link_libraries(Vulkan ${GLFW_LIB} ${VULKAN_LIB})
