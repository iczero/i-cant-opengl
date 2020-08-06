#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shaders.hpp"
#include "camera.hpp"

class Window;

// main shader
class MainShaderProgram : public ShaderProgram {
public:
    int uniform_color;
    int uniform_projection;
    int uniform_view;
    int uniform_model;

    MainShaderProgram();
    void init();
    void set_color(glm::vec4 &color);
    void set_projection(glm::mat4 &projection_matrix);
    void set_view(glm::mat4 &view_matrix);
    void set_model(glm::mat4 &model_matrix);
};

// extraordinarily simple graphics
class Graphics {
public:
    Window &window;
    float fov;
    int viewport_width;
    int viewport_height;
    MainShaderProgram main_shader;
    Camera camera;
    float lastFrameTime;
    float deltaTime;

    Graphics(Window &window);
    // initialize the instance
    void init();
    // update projection matrix with current values
    void update_projection();
    // update viewport size
    void update_viewport(int width, int height);
    // set draw color
    void set_color(glm::vec4 color);
    // set view matrix
    void set_view(glm::mat4 view_transform);
    // set model matrix
    void set_model(glm::mat4 model_transform);
    // set fov, in degrees
    void set_fov(float fov);
    // call to begin rendering frame
    void begin_render();
};
