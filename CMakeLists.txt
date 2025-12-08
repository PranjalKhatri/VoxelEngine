cmake_minimum_required(VERSION 3.27)
project(LearnOpengl LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for vim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add source files from src/
file(GLOB SRC_FILES src/*.cpp)

add_executable(LearnOpengl)
target_sources(
    LearnOpengl
    PRIVATE
    ${SRC_FILES}
)

# Include directory for headers
target_include_directories(LearnOpengl PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link directory for .lib files
target_link_directories(LearnOpengl PRIVATE ${CMAKE_SOURCE_DIR}/lib)

# Windows system libs + GLFW + OpenGL
target_link_libraries(LearnOpengl PRIVATE
    glfw3
    opengl32
    gdi32
    user32
)
