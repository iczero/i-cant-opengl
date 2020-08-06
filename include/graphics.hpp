#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaders.hpp"

// main shader
class MainShaderProgram : public ShaderProgram {
public:
    int uniform_color;
    int uniform_projection;
    int uniform_view;
    int uniform_model;

    MainShaderProgram();
    void init();
    void set_color(glm::vec4 color);
    void set_projection(glm::mat4 projection_matrix);
    void set_view(glm::mat4 view_matrix);
    void set_model(glm::mat4 model_matrix);
};

// extraordinarily simple graphics
class Graphics {
public:
    MainShaderProgram main_shader;

    Graphics();
    void init();
    void update_viewport(int width, int height);
    void set_color(glm::vec4 color);
    void set_view(glm::mat4 view_transform);
    void set_model(glm::mat4 model_transform);
    void begin_render();
    void render_geometry(unsigned int vao);
};
