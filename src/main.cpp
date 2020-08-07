#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.hpp"
#include "shaders.hpp"
#include "geometry.hpp"
#include "NBody.h"
#include "Octree.h"

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;

// why terminate stuff manually when c++ can do it for you!
class GLFWGuard {
public:
    GLFWGuard() {
        glfwInit();
    }
    ~GLFWGuard() {
        glfwTerminate();
    }
};

void render_geometry_at(Graphics &graphics, Geometry &geometry, glm::vec3 position, float scale = 1.0f, bool spinny = false) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    if (spinny) model = glm::rotate(model, (float) glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    graphics.set_model(model);
    geometry.draw();
}

int main() {
    // initialize glfw
    GLFWGuard glfw_guard;

    // opengl 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // create window
    GLFWwindow *glfw_window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "yay opengl", NULL, NULL);
    if (!glfw_window) {
        std::cerr << "Failed to create glfw window" << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(glfw_window);

    // initialize gl bindings with glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize glad" << std::endl;
        return 1;
    }
    // everything above MUST be run before any OpenGL routines

    // window "managing" thing? idk
    Window window(glfw_window);
    window.update_viewport(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    Graphics &graphics = window.graphics;

    // create a sphere
    IcosphereGeometry sphere_geometry(3);

    // initialize simulation
    NBody nbody;
    nbody.randomly_generate_bodies(100);
    nbody.simulate_frame();
    Octree &tree = *nbody.m_bodies;

    auto i = 0U;

    // main loop
    while (!window.should_close()) {
        // if (i++ > 5) window.close();
        graphics.camera.process_input();
        float time = glfwGetTime();
        if (glfwGetKey(glfw_window, GLFW_KEY_R) == GLFW_PRESS) nbody.simulate_frame();
        std::cout << "time for simulate_frame: " << glfwGetTime() - time << std::endl;
        time = glfwGetTime();

        // render
        {
            graphics.begin_render();
            for (auto *node : tree.allNodes) {
                glm::vec3 pos(node->position.x, node->position.y, node->position.z);
                pos /= 10.0f;
                float radius = node->mass / 1e13;
                render_geometry_at(graphics, sphere_geometry, pos, radius);
            }
            std::cout << "time for render: " << glfwGetTime() - time << std::endl;
            
            // render_geometry_at(graphics, sphere_geometry, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, true);
            // render_geometry_at(graphics, sphere_geometry, glm::vec3(5.0f, 6.0f, -7.0f), 1.5f, false);
            // render_geometry_at(graphics, sphere_geometry, glm::vec3(-4.3f, 1.6f, -9.1f), 0.4, true);
        }

        window.frame_done();
    }

    return 0;
}
