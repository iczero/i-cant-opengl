#include "camera.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "graphics.hpp"
#include "window.hpp"

Camera::Camera(Graphics &graphics):
    position(glm::vec3(0.0f, 0.0f, 0.0f)),
    direction(glm::vec3(0.0f, 0.0f, 1.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f)),
    movementSpeed(5.0f),
    yaw(90.0f),
    pitch(0.0f),
    sensitivity(0.1f),
    mouse_captured(false),
    last_x(0.0f),
    last_y(0.0f),
    graphics(graphics) {}

void Camera::update() {
    float pitch_r = glm::radians(pitch);
    float yaw_r = glm::radians(yaw);
    direction.x = glm::cos(yaw_r) * glm::cos(pitch_r);
    direction.y = glm::sin(pitch_r);
    direction.z = glm::sin(yaw_r) * glm::cos(pitch_r);
    graphics.set_view(glm::lookAt(position, position + direction, up));
}

void Camera::set_position(glm::vec3 position) {
    this->position = position;
    update();
}

void Camera::set_direction(float yaw, float pitch) {
    this->yaw = yaw;
    this->pitch = pitch;
    update();
}

void Camera::process_input() {
    Window &window = graphics.window;
    GLFWwindow *glfw_window = window.window;
    bool should_update = false;
    if (glfwGetKey(glfw_window, GLFW_KEY_W) == GLFW_PRESS) {
        position += movementSpeed * graphics.deltaTime * direction;
        should_update = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_S) == GLFW_PRESS) {
        position += -movementSpeed * graphics.deltaTime * direction;
        should_update = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_A) == GLFW_PRESS) {
        position += -movementSpeed * graphics.deltaTime * glm::normalize(glm::cross(direction, up));
        should_update = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS) {
        position += movementSpeed * graphics.deltaTime * glm::normalize(glm::cross(direction, up));
        should_update = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += movementSpeed * graphics.deltaTime * up;
        should_update = true;
    }
    if (glfwGetKey(glfw_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position += -movementSpeed * graphics.deltaTime * up;
        should_update = true;
    }
    if (should_update) update();
}

// used for oneshot key controls as opposed to motion
void Camera::handle_keyboard_input(int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    Window &window = graphics.window;
    GLFWwindow *glfw_window = window.window;
    bool should_update = false;
    switch (key) {
        case GLFW_KEY_ESCAPE: {
            if (!mouse_captured) glfwSetWindowShouldClose(glfw_window, true);
            else {
                // TODO: maybe move this to window?
                mouse_captured = false;
                glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            break;
        }
    }
    if (should_update) update();
}

void Camera::handle_mouse_position(double xpos, double ypos) {
    if (!mouse_captured) return;
    float x_offset = (xpos - last_x) * sensitivity;
    float y_offset = (last_y - ypos) * sensitivity;
    yaw += x_offset;
    pitch = glm::clamp(pitch + y_offset, -89.9f, 89.9f);
    last_x = xpos;
    last_y = ypos;
    update();
}

void Camera::handle_mouse_button(int button, int action, int) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return; // we don't handle anything else
    if (action != GLFW_PRESS) return;
    GLFWwindow *glfw_window = graphics.window.window;
    if (!mouse_captured) {
        mouse_captured = true;
        glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // update initial cursor position
        glfwGetCursorPos(glfw_window, &last_x, &last_y);
    }
}
