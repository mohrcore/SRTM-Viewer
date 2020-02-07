#pragma once

#include "gfx_error.h"
#include "glsl_program.h"

namespace error {
    void printGfxError(graphics::GfxError e, const wchar_t * current_action, std::wostream & output);
    graphics::GfxError fetchGLFWError();
    void printGLSLProgramError(const graphics::GLSLProgramError & e, const wchar_t * current_action, std::wostream & output);
}