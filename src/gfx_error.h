#pragma once

#include "util.h"

namespace graphics {
    enum class GfxErrorEnum {
        GLEW_ERROR,
        GLFW_ERROR,
    };

    union GfxErrorUnion {
        struct {
            unsigned int error_code;
            const char * error_string;
        } glew_error;
        struct {
            int error_code;
            const char * error_description;
        } glfw_error;
    };

    using GfxError = TaggedUnion<GfxErrorEnum, GfxErrorUnion>;
}