cmake_minimum_required(VERSION 3.27)
project(VoxelEngine LANGUAGES C CXX)

# C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For clangd / vim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect sources
file(GLOB SRC_FILES src/*.cpp)

# Copy assets
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
)

# Executable
add_executable(VoxelEngine ${SRC_FILES})
add_dependencies(VoxelEngine copy_assets)

target_compile_options(VoxelEngine PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wpointer-arith
    -Wcast-qual
    -Wwrite-strings
)

# ---- External libraries ----

# glad
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# stb_image
add_library(stb_image STATIC external/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC external/stb_image)


target_include_directories(VoxelEngine PRIVATE
    include
    external/glm
    external/glfw/include
    external/fast_noise_lite
)

# Prebuilt libs (GLFW)
target_link_directories(VoxelEngine PRIVATE lib)

# Link everything
target_link_libraries(VoxelEngine PRIVATE
    glad
    stb_image
    glfw3
    opengl32
    gdi32
    user32
)
