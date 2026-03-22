cmake_minimum_required(VERSION 3.10)
project(Engine LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
)

# GLAD
add_library(glad STATIC
    external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

# GLFW
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

# ImGui (Docking)
set(IMGUI_DIR external/imgui-docking/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    glad
    ${GLFW_LIB}
)

# System libraries
find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)

# Engine executable
add_executable(Engine
    ${ENGINE_SOURCES}
)

target_include_directories(Engine PRIVATE
    include
    external
    external/glm
    external/CSVWriter/include
)

target_link_libraries(Engine PRIVATE
    glad
    imgui
    ${GLFW_LIB}
    OpenGL::GL
    assimp::assimp
)

# Debug symbols (single-config generators)
target_compile_options(Engine PRIVATE
    $<$<CONFIG:Debug>:-g>
)
