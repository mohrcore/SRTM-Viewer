#include "window.h"

#include "error.h"
#include "init.h"

using namespace graphics;

Window::Window()
    : window(nullptr) {}

Window::Window(Window && other) {
    this->window = other.window;
    other.window = nullptr;
}

Window::~Window() {
    glfwDestroyWindow(this->window);
}

void Window::init(const char * title, size_t width, size_t height) {
    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow * window = glfwCreateWindow(int(width), int(height), title, NULL, NULL);
    if (window == NULL)
        throw (error::fetchGLFWError());
    glfwMakeContextCurrent(window);
    init::initGLEW();
    this->window = window;
}

Window::operator GLFWwindow*() {
    return this->window;
}