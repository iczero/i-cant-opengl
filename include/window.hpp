#pragma once
#include "graphics.hpp"
#include <GLFW/glfw3.h>

// this would do glfw initialization but you pretty much must do that in main()
class Window {
public:
    // viewport dimensions
    int viewport_width;
    int viewport_height;

    // thing that does rendering and other graphics things
    Graphics graphics;

    // associated glfw window
    GLFWwindow *window;

    Window(GLFWwindow *window);
    ~Window();

    // glfw framebuffer size callback
    static void update_viewport_handler(GLFWwindow *window, int width, int height);
    // updates viewport width/height and recreates projection matrix
    void update_viewport(int width, int height);
    bool should_close();
    void close();
    void frame_done();
};
