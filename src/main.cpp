#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() {
    // init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // register callback for resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    float vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top
                             // second triangle
        0.0f, -0.5f, 0.0f,   // left
        0.9f, -0.5f, 0.0f,   // right
        0.45f, 0.5f, 0.0f    // top
    };

    pop::gfx::Shader vertexShader{pop::gfx::ShaderType::Vertex};
    pop::gfx::Shader fragmentShader{pop::gfx::ShaderType::Fragment};
    if (!pop::gfx::Shader::LoadAndCompile(vertexShader, "shaders/vertex.glsl"))
        return 1;
    if (!pop::gfx::Shader::LoadAndCompile(fragmentShader,
                                          "shaders/fragment.glsl"))
        return 1;
    pop::gfx::ShaderProgram shaderProgram{};
    shaderProgram.Attach(vertexShader);
    shaderProgram.Attach(fragmentShader);
    if (!shaderProgram.Link()) {
        std::cerr << shaderProgram.GetError() << "\n";
        return 1;
    }

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    // rendering loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);
        // rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // cleanup resources and terminate
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
