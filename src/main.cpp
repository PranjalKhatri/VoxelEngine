#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine.hpp"
#include "gl_types.hpp"
#include "glm/fwd.hpp"
#include "popglfw.hpp"
#include "shader.hpp"
#include "stb_image.h"
#include "camera.hpp"
#include "chunk_system.hpp"

#include <iostream>
#include <memory>
#include <thread>

using namespace pop;
using namespace pop::gfx;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main() {
    // init
    GLFW::init(3, 3, GLFW::GLFW_PROFILE::kCoreProfile);
    GLFWwindow* window = glfwCreateWindow(800, 600, "VoxelEngine", NULL, NULL);
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    glfwSetErrorCallback([](int code, const char* desc) {
        std::cerr << "GLFW Error [" << code << "]: " << desc << std::endl;
    });
    float fov    = glm::radians(45.0f);
    float aspect = 1920.0f / 1080.0f;
    float near   = 0.1f;

    // Tweaked infinite perspective handles the math for a "far plane" at
    // infinity
    glm::mat4 projection = glm::tweakedInfinitePerspective(fov, aspect, near);
    Engine    engine{projection, window};

    auto cam = std::make_shared<FlyCam>(glm::vec3{0, 0, 3}, glm::vec3{0, 0, 0},
                                        glm::vec3{0, 1, 0});
    engine.SetMainCamera(cam);
    std::cout << "Engine initialization done!\n";
    // // Shader
    std::unique_ptr<ShaderProgram> cubeShader =
        std::make_unique<ShaderProgram>();
    {
        Shader vertex{ShaderStage::Vertex};
        Shader fragment{ShaderStage::Fragment};
        Shader::LoadAndCompile(vertex, "assets/shaders/cube.vert");
        Shader::LoadAndCompile(fragment, "assets/shaders/cube.frag");

        cubeShader->Attach(vertex);
        cubeShader->Attach(fragment);
        cubeShader->Link();

        cubeShader->use();
        cubeShader->SetUniformInt("tex1", 0);
        // cubeShader->SetUniformInt("tex2", 1);
    }
    std::cout << "Cube Shader constructed\n";

    auto textureAtlas =
        std::make_shared<rtypes::TextureBinding>(rtypes::TextureBinding{
            std::make_shared<Texture>(TextureType::kTexture2D), 0});

    textureAtlas->texture->LoadFromFile("assets/textures/VoxelTextures.png",
                                        true);
    /*
    auto chunk = std::make_shared<voxel::Chunk>(glm::ivec3(0, 0, 0));
    chunk->SetShader(cubeShader->id());
    chunk->GenerateMesh();
    chunk->GetSolidRenderable()->AddTexture(wood);
    engine.AddRenderable(chunk->GetSolidRenderable());
    */

    /*
    auto cube = std::make_shared<gfx::CubeRenderable>(
        cubeShader->id(), "assets/textures/wood.jpg",
        "assets/textures/face.png");
    std::cout << "chunk shader set and constructed\n";
    // engine.AddRenderable(cube);
    */
    voxel::ChunkManager manager{cam.get()};
    manager.SetShader(cubeShader->id());
    manager.SetTexture(textureAtlas);
    engine.AddShaderProgram(std::move(cubeShader));
    std::thread chunkSystemThread{&voxel::ChunkManager::Run, &manager,
                                  std::ref(engine)};
    engine.Run();
    chunkSystemThread.join();

    // cleanup resources and terminate
    glfwTerminate();
    return 0;
}
