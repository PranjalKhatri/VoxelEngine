#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_types.hpp"
#include "shader.hpp"
#include "vertex_buffers.hpp"
#include "stb_image.h"

#include <iostream>

using namespace pop::gfx;

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
        // positions          // colors           // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };

    Shader vertexShader{ShaderType::Vertex};
    Shader fragmentShader{ShaderType::Fragment};
    if (!pop::gfx::Shader::LoadAndCompile(vertexShader, "shaders/vertex.glsl"))
        return 1;
    if (!pop::gfx::Shader::LoadAndCompile(fragmentShader,
                                          "shaders/fragment.glsl"))
        return 1;
    ShaderProgram shaderProgram{};
    shaderProgram.Attach(vertexShader);
    shaderProgram.Attach(fragmentShader);
    if (!shaderProgram.Link()) {
        std::cerr << shaderProgram.GetError() << "\n";
        return 1;
    }

    VertexArray VAO{};
    VAO.Bind();
    // 2. copy our vertices array in a buffer for OpenGL to use
    GLBuffer VBO{BufferType::kArrayBuffer},
        EBO{BufferType::kElementArrayBuffer};
    VBO.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    VAO.AddAttribute({0, 3, GLType::kFloat, false, 8 * sizeof(float), 0});
    VAO.AddAttribute(
        {1, 3, GLType::kFloat, false, 8 * sizeof(float), 3 * sizeof(float)});

    EBO.Bind();
    EBO.BufferData(sizeof(indices), indices, GL_STATIC_DRAW);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nchannels;
    // load rgb data
    unsigned char* data =
        stbi_load("textures/wood.jpg", &width, &height, &nchannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // rendering loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);
        // rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        shaderProgram.use();
        VAO.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // cleanup resources and terminate
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
