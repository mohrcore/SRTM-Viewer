#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace graphics {
    struct Window {
        GLFWwindow * window;
        Window();
        Window(Window && other);
        Window(Window &) = delete;
        Window & operator=(Window &) = delete;
        ~Window();
        operator GLFWwindow*();
        void init(const char * title, size_t width, size_t height);
    };
}