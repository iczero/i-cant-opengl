#include "graphics.hpp"
#include <stdexcept>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Graphics::Graphics() {}

void Graphics::init() {
    // gl configuration
    glEnable(GL_DEPTH_TEST);

    // initialize shaders
    main_shader.init();
    draw_color(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

    // wireframe drawing mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Graphics::update_viewport(int width, int height) {
    glViewport(0, 0, width, height);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / height, 0.1f, 100.0f);
    main_shader.use();
    main_shader.set_projection(projection);
}

void Graphics::set_color(glm::vec4 color) {
    main_shader.use();
    main_shader.set_color(color);
}

void Graphics::set_view(glm::mat4 view_transform) {
    main_shader.use();
    main_shader.set_view(view_transform);
}

void Graphics::set_model(glm::mat4 model_transform) {
    main_shader.use();
    main_shader.set_model(model_transform);
}

void Graphics::begin_render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    main_shader.use();
}

MainShaderProgram::MainShaderProgram(): ShaderProgram("main") {}

void MainShaderProgram::init() {
    Shader vertex_shader = Shader::from_file("main", GL_VERTEX_SHADER, "shaders/main.vert");
    if (!vertex_shader.compile()) throw std::runtime_error("compile main vertex shader failed");
    Shader fragment_shader = Shader::from_file("main", GL_FRAGMENT_SHADER, "shaders/main.frag");
    if (!fragment_shader.compile()) throw std::runtime_error("compile main fragment shader failed");
    attach_shader(vertex_shader);
    attach_shader(fragment_shader);
    if (!link()) throw std::runtime_error("link main shader program failed");

    uniform_color = get_uniform_location("color");
    uniform_projection = get_uniform_location("projection");
    uniform_view = get_uniform_location("view");
    uniform_model = get_uniform_location("model");
}

void MainShaderProgram::set_color(glm::vec4 color) {
    set_uniform(uniform_color, color);
}

void MainShaderProgram::set_projection(glm::mat4 projection_matrix) {
    set_uniform(uniform_projection, projection_matrix);
}

void MainShaderProgram::set_view(glm::mat4 view_matrix) {
    set_uniform(uniform_view, view_matrix);
}

void MainShaderProgram::set_model(glm::mat4 model_matrix) {
    set_uniform(uniform_model, model_matrix);
}
