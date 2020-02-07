#include "init.h"

#include "gfx_error.h"
#include "error.h"

using namespace init;

void init::initGLEW() {
    glewExperimental = true;
    GLenum error = glewInit();
    if (error) {
        const char * err_string = (const char *)glewGetErrorString(error);
        throw (graphics::GfxError(
            graphics::GfxErrorEnum::GLEW_ERROR,
            graphics::GfxErrorUnion { .glew_error = {
                .error_code = error,
                .error_string = err_string
            }}
        ));
    }
}

void init::initGLFW() {
    int success = glfwInit();
    if (!success) {
        const char * error_str;
        int error_code = glfwGetError(&error_str);
        throw (error::fetchGLFWError());
    }    
}