#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Graphics;

class Camera {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    float movementSpeed;

    float yaw;
    float pitch;
    float sensitivity;
    bool mouse_captured;
    double last_x;
    double last_y;

    Graphics &graphics;

    Camera(Graphics &graphics);
    // update view matrix in graphics
    void update();
    // set new position
    void set_position(glm::vec3 position);
    // set new direction
    void set_direction(float yaw, float pitch);
    // process keyboard input
    void process_input();
    // keyboard input callback
    void handle_keyboard_input(int key, int scancode, int action, int mods);
    // mouse movement callback
    void handle_mouse_position(double xpos, double ypos);
    // mouse button callback
    void handle_mouse_button(int button, int action, int mods);
    // mouse wheel callback
    void handle_mouse_wheel(double x_offset, double y_offset);
};
