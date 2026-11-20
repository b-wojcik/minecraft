cmake_minimum_required(VERSION 3.16)

project("minecraft" LANGUAGES C CXX)

include(FetchContent)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)

# GLAD 2 - OpenGL 4.4 Core
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR cmake
)

FetchContent_MakeAvailable(glad)

glad_add_library(
    glad_gl_core_44
    STATIC
    REPRODUCIBLE
    API gl:core=4.4
)

# GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)

FetchContent_MakeAvailable(glm)

# Minecraft
add_executable(
    minecraft

    # Game
    src/main.cpp
    src/world/world.hpp
    src/world/chunk_generator.hpp
    src/world/face_generator.hpp
    src/world/world_constants.hpp

    # Extra
    src/third_party/stb_image.c

    # Core
    src/core/debug.hpp
    src/core/input.hpp
    src/core/window.hpp

    # Gfx
    src/gfx/shader_cache.hpp
    src/gfx/camera/camera.hpp
    src/gfx/camera/fp_camera.hpp
    src/gfx/texture_array.hpp
)

target_compile_features(
    minecraft
    PRIVATE
        cxx_std_20
)

target_link_libraries(
    minecraft
    PRIVATE
        glfw
        glad_gl_core_44
        glm
)