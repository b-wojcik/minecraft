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
    
    src/game/world/world.h
    src/game/world/world.cpp

    src/game/world/chunk_generator.h
    src/game/world/chunk_generator.cpp

    src/game/world/face_generator.h
    src/game/world/world_constants.h

    src/game/game_manager.h
    src/game/game_manager.cpp

    # Extra
    src/third_party/stb_image.c

    # Core
    src/engine/framework.h
    
    src/engine/core/keycode.h
    src/engine/core/input.h
    src/engine/core/input.cpp

    src/engine/core/debug.h
    src/engine/core/window.h
    src/engine/core/window.cpp

    # Gfx
    src/engine/gfx/shader_cache.h
    src/engine/gfx/shader_cache.cpp

    src/engine/gfx/texture_array.h
    src/engine/gfx/texture_array.cpp

    src/engine/gfx/camera/camera.h
    src/engine/gfx/camera/camera.cpp
    
    src/engine/gfx/camera/fp_camera.h
    src/engine/gfx/camera/fp_camera.cpp

    # Physics
    src/engine/physics/aabb.h
    src/engine/physics/aabb.cpp
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