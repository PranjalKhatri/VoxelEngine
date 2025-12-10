cmake_minimum_required(VERSION 3.27)
project(LearnOpengl LANGUAGES C CXX)

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

# Link directory for .lib files
target_link_directories(LearnOpengl PRIVATE ${CMAKE_SOURCE_DIR}/lib)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/src/glad.c)

target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(LearnOpengl PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Windows system libs + GLFW + OpenGL + glad
target_link_libraries(LearnOpengl PRIVATE
    glad
    glfw3
    opengl32
    gdi32
    user32
)
