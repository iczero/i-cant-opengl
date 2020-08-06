#include <stdexcept>
#include "window.hpp"

Window::Window(GLFWwindow *window): graphics(*this), window(window) {
    graphics.init();
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, &Window::update_viewport_handler);
    glfwSetKeyCallback(window, &Window::keyboard_input_handler);
    glfwSetCursorPosCallback(window, &Window::mouse_position_handler);
    glfwSetMouseButtonCallback(window, &Window::mouse_button_handler);
    glfwSetScrollCallback(window, &Window::mouse_wheel_handler);
}

Window::~Window() {
    // this will most likely end badly
    // glfwDestroyWindow(window);
}

void Window::update_viewport_handler(GLFWwindow *window, int width, int height) {
    Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
    instance->update_viewport(width, height);
}

void Window::keyboard_input_handler(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
    instance->graphics.camera.handle_keyboard_input(key, scancode, action, mods);
}

void Window::mouse_position_handler(GLFWwindow *window, double xpos, double ypos) {
    Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
    instance->graphics.camera.handle_mouse_position(xpos, ypos);
}

void Window::mouse_button_handler(GLFWwindow *window, int button, int action, int mods) {
    Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
    instance->graphics.camera.handle_mouse_button(button, action, mods);
}

void Window::mouse_wheel_handler(GLFWwindow *window, double x_offset, double y_offset) {
    Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
    instance->graphics.camera.handle_mouse_wheel(x_offset, y_offset);
}

void Window::update_viewport(int width, int height) {
    viewport_width = width;
    viewport_height = height;
    graphics.update_viewport(width, height);
}

bool Window::should_close() {
    return glfwWindowShouldClose(window);
}

void Window::close() {
    glfwSetWindowShouldClose(window, true);
}

void Window::frame_done() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}
