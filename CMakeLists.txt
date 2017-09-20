cmake_minimum_required(VERSION 3.8)
project(Vulkan)


# Configuring application
set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES main.cpp HelloTriangleApplication.cpp HelloTriangleApplication.h Vertex.h VulkanTutorialConfig.h.in)
#set(SOURCE_FILES main.cpp HelloTriangleApplication.h VulkanTutorialConfig.h.in)
set(Vulkan_Tutorial_VERSION_MAJOR 1)
set(Vulkan_Tutorial_VERSION_MINOR 0)

set(ENV{VULKAN_SDK} "F:/SDKs/LunarGVulkan/1.0.57.0" CACHE PATH)
set(ENV{GLFW_SDK} "F:/SDKs/GLFW/glfw-3.2.1.bin.WIN32" CACHE PATH)
set(ENV{GLM_SDK} "F:/SDKs/GLM/glm-0.9.8.5" CACHE PATH)
set(ENV{STB_SDK} "F:/SDKs/stb" CACHE PATH)
set(ENV{TOL_SDK} "F:/SDKs/tinyobjloader" CACHE PATH)

# Finding libraries

if (MSVC)
    find_library(VULKAN_LIB vulkan-1 PATHS $ENV{VULKAN_SDK}/Lib32)
    find_library(GLFW_LIB glfw3 PATHS $ENV{GLFW_SDK}/lib-vc2015)
else ()
    find_library(VULKAN_LIB vulkan-1 PATHS $ENV{VULKAN_SDK})
    find_library(GLFW_LIB glfw3 PATHS $ENV{GLFW_SDK}/lib-mingw)
endif ()

# Configuring file
option(RH_ENABLE_VALIDATION_LAYERS "Use Validation Layers" ON)
option(RH_COLOR_BLENDING "Use Color Blending" OFF)

configure_file(
        ${PROJECT_SOURCE_DIR}/VulkanTutorialConfig.h.in
        ${PROJECT_BINARY_DIR}/VulkanTutorialConfig.h
)


# Adding subdirectory
add_subdirectory(shaders)
add_subdirectory(textures)
add_subdirectory(models)

# Adding headers to application
include_directories(
        ${PROJECT_BINARY_DIR}
        $ENV{VULKAN_SDK}/Include/
        $ENV{GLFW_SDK}/include/
        $ENV{GLM_SDK}/glm/
        $ENV{STB_SDK}
        $ENV{TOL_SDK}
)

add_executable(Vulkan ${SOURCE_FILES} ${SHADER_FILES})
add_dependencies(Vulkan SHADER)
target_link_libraries(Vulkan ${VULKAN_LIB} ${GLFW_LIB})
