#include "error.h"

#include <GL/freeglut.h>

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace error;

void error::printGfxError(graphics::GfxError e, const wchar_t * current_action, std::wostream & output) {
    switch (e.type) {
        case graphics::GfxErrorEnum::GLEW_ERROR:
            output
                << "A GLEW error has occured during ["
                << current_action
                << "] (code: "
                << e.content.glew_error.error_code
                << "):\n    \""
                << e.content.glew_error.error_string
                << "\"" << std::endl;
        break;
        case graphics::GfxErrorEnum::GLFW_ERROR:
            output
                << "A GLFW error has occured during ["
                << current_action
                << "] (code: "
                << e.content.glfw_error.error_code
                << "):\n    \""
                << e.content.glfw_error.error_description
                << "\"" << std::endl;
        break;
    }
}

graphics::GfxError error::fetchGLFWError() {
    const char * error_str;
    int error_code = glfwGetError(&error_str);
    return graphics::GfxError(
        graphics::GfxErrorEnum::GLFW_ERROR,
        graphics::GfxErrorUnion { .glfw_error = {
            .error_code = error_code,
            .error_description = error_str
        }}
    );
}

const wchar_t * stringifyShaderType(GLenum shader_type) {
    switch (shader_type) {
        case GL_VERTEX_SHADER:          return L"GL_VERTEX_SHADER";          break;
        case GL_FRAGMENT_SHADER:        return L"GL_FRAGMENT_SHADER";        break;
        case GL_GEOMETRY_SHADER:        return L"GL_GEOMETRY_SHADER";        break;
        case GL_TESS_CONTROL_SHADER:    return L"GL_TESS_CONTROL_SHADER";    break;
        case GL_TESS_EVALUATION_SHADER: return L"GL_TESS_EVALUATION_SHADER"; break;
        case GL_COMPUTE_SHADER:         return L"GL_COMPUTE_SHADER";         break;
        default:                        return L"[UNKNOWN]";                 break;
    }
}

void error::printGLSLProgramError(const graphics::GLSLProgramError & e, const wchar_t * current_action, std::wostream & output) {
    std::wstring missing_sources;
    std::wstring w_log;
    switch (e.type) {
        case graphics::GLSLProgramErrorType::FAILED_TO_LOAD_SOURCE:
            output
                << L"Failed to load GLSL program source (during: ["
                << current_action
                << L"])\nMissing source path: \""
                << e.failed_to_load_source.source_path
                << L"\"" << std::endl;
        break;
        case graphics::GLSLProgramErrorType::INCOMPLETE_SOURCE_SET:
            if (e.incomplete_source_set.vertex_shader_missing)
                missing_sources += L"vertex shader";
            if (e.incomplete_source_set.fragment_shader_missing) {
                if (missing_sources != L"")
                    missing_sources += L", ";
                missing_sources += L"fragment shader";
            }
            output
                << L"Failed to find necessary GLSL shader source files for "
                << missing_sources
                << L" (during: ["
                << current_action
                << L"])" << std::endl;
        break;
        case graphics::GLSLProgramErrorType::COMPILATION_ERROR:
            w_log.assign(e.compilation_error.log.begin(), e.compilation_error.log.end());
            output
                << L"Failed to compile "
                << stringifyShaderType(e.compilation_error.shader_type)
                << L" GLSL shader (during: ["
                << current_action
                << L"])\nCompilation log:\n"
                << w_log << std::endl;
        break;
        case graphics::GLSLProgramErrorType::LINKING_ERROR:
            w_log.assign(e.linking_error.log.begin(), e.linking_error.log.end());
            output
                << L"Failed to link GLSL program (during ["
                << current_action
                << L"])\nLinking log:\n"
                << w_log << std::endl;
        break;
        case graphics::GLSLProgramErrorType::NO_ERR:
            output
                << L"A procedure didn't finish successfully, but the error wasn't specified (during: ["
                << current_action
                << L"]).\nIf you see this message, it's very likely that something went wrong, but the error throwing procedure is flawed" << std::endl;
            break;
        default:
            output
                << L"[This error has no pretty-printing procedure] (during: ["
                << current_action
                << L"])" << std::endl;
        break;
    }
}