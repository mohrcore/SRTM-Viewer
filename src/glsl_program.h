#pragma once

#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>

namespace graphics {
    //This could be easily extended to support more shading stages, eg. geometry shaders
    /* enum class GLSLSourceType {
        FRAGMENT,
        VERTEX
    }; */
    using GLSLSourceType = GLenum;
    class GLSLProgram {
    public:
        GLuint program;
        GLuint loc_world_transform;
        GLuint loc_viewport_transform;
        GLSLProgram();
        GLSLProgram(GLuint program_id);
        operator GLuint() const;
        GLSLProgram(GLSLProgram &) = delete;
        GLSLProgram(GLSLProgram const &) = delete;
        GLSLProgram & operator=(GLSLProgram &) = delete;
        GLSLProgram & operator=(GLSLProgram const &) = delete;
        GLSLProgram(GLSLProgram && other);
        GLSLProgram & operator=(GLSLProgram && other);
        ~GLSLProgram();
    };
    using GLSLProgramSourcePaths = std::map<GLSLSourceType, std::vector<std::wstring>>;
    using GLSLProgramSources = std::map<GLSLSourceType, std::vector<std::string>>; 
    GLSLProgram compileProgramFromSourcePaths(const GLSLProgramSourcePaths & source_paths);
    GLSLProgram compileProgramFromSources(const GLSLProgramSources & sources);
    enum class GLSLProgramErrorType {
        FAILED_TO_LOAD_SOURCE,
        INCOMPLETE_SOURCE_SET,
        COMPILATION_ERROR,
        LINKING_ERROR,
        NO_ERR
    };
    class GLSLProgramError {
    public:
        GLSLProgramErrorType type;
        union {
            void * __nullfield;
            struct {
                std::wstring source_path;
            } failed_to_load_source;
            struct {
                bool vertex_shader_missing;
                bool fragment_shader_missing;
            } incomplete_source_set;
            struct {
                std::string log;
                GLSLSourceType shader_type;
            } compilation_error;
            struct {
                std::string log;
            } linking_error;
        };
        GLSLProgramError();
        GLSLProgramError(const GLSLProgramError & other);
        ~GLSLProgramError();
    };
}