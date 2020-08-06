#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.hpp"
#include "shaders.hpp"
#include "geometry.hpp"

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

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
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
    IcosphereGeometry sphere_geometry(10);

    // main loop
    while (!window.should_close()) {
        process_input(glfw_window);

        // render
        {
            graphics.begin_render();

            // view transform
            // TODO: dynamic camera
            graphics.set_view(glm::lookAt(
                glm::vec3(0.0f, 0.0f, 3.0f), // eye position
                glm::vec3(0.0f, 0.0f, 0.0f), // where the camera is looking at
                glm::vec3(0.0f, 1.0f, 0.0f) // up direction
            ));

            // model transform
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, (float) glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            graphics.set_model(model);

            sphere_geometry.draw();
        }

        window.frame_done();
    }

    return 0;
}
