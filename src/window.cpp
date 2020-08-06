#include <stdexcept>
#include "window.hpp"

Window::Window(GLFWwindow *window): window(window) {
    graphics.init();
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, &Window::update_viewport_handler);
}

Window::~Window() {
    // this will most likely end badly
    // glfwDestroyWindow(window);
}

void Window::update_viewport_handler(GLFWwindow *window, int width, int height) {
    Window *instance = static_cast<Window *>(glfwGetWindowUserPointer(window));
    instance->update_viewport(width, height);
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
