cmake_minimum_required(VERSION 3.10)
project(Engine LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ version injected by ProjectMaker
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Sources
# --------------------------------------------------
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
)

# --------------------------------------------------
# GLAD
# --------------------------------------------------
add_library(glad STATIC
    external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

# --------------------------------------------------
# ImGui (Docking)
# --------------------------------------------------
file(GLOB IMGUI_SOURCES
    external/imgui-docking/imgui/*.cpp
    external/imgui-docking/imgui/backends/imgui_impl_glfw.cpp
    external/imgui-docking/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_link_libraries(imgui PUBLIC glad)

target_include_directories(imgui PUBLIC
    external/imgui-docking/imgui
    external/imgui-docking/imgui/backends
)

# --------------------------------------------------
# TinyFileDialogs
# --------------------------------------------------
#add_library(tinyfiledialogs STATIC
#    external/libtinyfiledialogs/tinyfiledialogs.c
#)

#target_include_directories(tinyfiledialogs PUBLIC
#    external/libtinyfiledialogs
#)

# --------------------------------------------------
# System libraries
# --------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

# GLFW (system or bundled)
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw)
    set(GLFW_LIB glfw)
else()
    set(GLFW_LIB glfw)
endif()

# --------------------------------------------------
# Executable
# --------------------------------------------------
add_executable(Engine
    ${ENGINE_SOURCES}
)

target_include_directories(Engine PRIVATE
    include
    external        # for stb_image.h
    ${ASSIMP_INCLUDE_DIRS}
)

target_link_libraries(Engine PRIVATE
    glad
    imgui
    #tinyfiledialogs
    ${GLFW_LIB}
    OpenGL::GL
    assimp
)

# --------------------------------------------------
# Debug symbols (single-config generators)
# --------------------------------------------------
target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:-g>
)

