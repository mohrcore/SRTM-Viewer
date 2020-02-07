#include "glsl_program.h"

#include <utility>
#include <fstream>
#include <experimental/filesystem>
#include <cstring>

#include "util.h"

#ifdef FILESYSTEM_EXPERIMENTAL
#define FS_PATH std::experimental::filesystem::path
#else
#ifdef FILESYSTEM_USE_ASCII_STRING_AS_PATH
#define FS_PATH std::string
#else
#define FS_PATH std::filesystem::path
#endif
#endif


using namespace graphics;

GLuint compileGLSLShader(const std::vector<std::string> & sources, GLSLSourceType source_type) {
    const char * * sources_c = new const char *[sources.size()];
    size_t * sources_c_sz = new size_t[sources.size()];
    for (size_t i = 0; i < sources.size(); i++) {
        sources_c[i] = &sources[i][0];
        sources_c_sz[i] = sources[i].size();
    }
    GLuint shader = glCreateShader(source_type);
	glShaderSource(shader, sources.size(), (const GLchar * *)sources_c, (GLint *)sources_c_sz);
	glCompileShader(shader);
    delete[] sources_c;
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint loglength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglength);
		char * log = new char[loglength];
		glGetShaderInfoLog(shader, loglength, &loglength, log);
        GLSLProgramError program_error;
        program_error.type = GLSLProgramErrorType::COMPILATION_ERROR;
        program_error.compilation_error.shader_type = source_type;
        new (&program_error.compilation_error.log) std::string(log);
		delete[] log;
        glDeleteShader(shader);
		throw (program_error);
	}
	return shader;
}

GLuint linkGLSLProgram(const std::vector<GLuint> & shaders) {
    GLuint program = glCreateProgram();
    for (GLuint shader : shaders)
        glAttachShader(program, shader);
    glLinkProgram(program);
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE) {
        GLint loglength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglength);
		char * log = new char[loglength];
		glGetProgramInfoLog(program, loglength, &loglength, log);
        GLSLProgramError program_error;
        program_error.type = GLSLProgramErrorType::LINKING_ERROR;
        new (&program_error.linking_error.log) std::string(log);
        delete[] log;
        glDeleteProgram(program);
        throw (program_error);
    }
    for (GLuint shader : shaders)
        glDetachShader(program, shader);
    return program;
}

GLSLProgram graphics::compileProgramFromSourcePaths(const GLSLProgramSourcePaths & source_paths) {
    std::map<GLSLSourceType, std::vector<std::string>> source_map;
    for (std::pair<GLSLSourceType, std::vector<std::wstring>> && key_value : source_paths) {
        GLSLSourceType source_type = key_value.first;
        for (auto && path_str : key_value.second) {
            //std::fstream(const std::filesystem::path &) overload is used to allow unicode file paths, this hasn't been tested tho
            const FS_PATH source_path = path_str;
            std::ifstream source_stream(source_path);
            if (!source_stream.good()) {
                GLSLProgramError program_error;
                program_error.type = GLSLProgramErrorType::FAILED_TO_LOAD_SOURCE;
                new (&program_error.failed_to_load_source) std::wstring(path_str);
                throw (program_error);
            }
            size_t fsz = getStreamSize(source_stream);
            std::string source = std::string(fsz + 1, '\0');
            source_stream.read(&source[0], fsz);
            source_map[source_type].push_back(std::move(source));
        }
    }
    return compileProgramFromSources(source_map);
}

GLSLProgram graphics::compileProgramFromSources(const GLSLProgramSources & sources) {
    bool contains_vertex = sources.count(GL_VERTEX_SHADER);
    bool contains_fragment = sources.count(GL_FRAGMENT_SHADER);
    if (!contains_vertex || !contains_fragment) {
        GLSLProgramError program_error;
        program_error.type = GLSLProgramErrorType::INCOMPLETE_SOURCE_SET;
        program_error.incomplete_source_set = {!contains_vertex, !contains_fragment};
        throw (program_error);
    }
    std::vector<GLuint> shaders;
    for (std::pair<GLSLSourceType, std::vector<std::string>> && key_value : sources) {
        auto source_type = key_value.first;
        auto & shader_sources = key_value.second;
        for (auto && source : shader_sources) {
            shaders.push_back(compileGLSLShader({source}, source_type));
        }
    }
    GLuint program_id;
    try {
        program_id = linkGLSLProgram(shaders);
    } catch (GLSLProgramError e) {
        for (GLuint shader : shaders)
            glDeleteShader(shader);
        throw (e);
    }
    return GLSLProgram(program_id);
}

GLSLProgram::GLSLProgram()
    : program(0) {}

GLSLProgram::GLSLProgram(GLuint program_id)
    : program(program_id) {}

GLSLProgram::GLSLProgram(GLSLProgram && other) :
    program(other.program),
    loc_world_transform(other.loc_world_transform),
    loc_viewport_transform(other.loc_viewport_transform)
{
    other.program = 0;
}

GLSLProgram & GLSLProgram::operator=(GLSLProgram && other) {
    glDeleteProgram(this->program);
    this->program = other.program;
    this->loc_viewport_transform = other.loc_viewport_transform;
    this->loc_world_transform = other.loc_world_transform;
    other.program = 0;
    return *this;
}

GLSLProgram::~GLSLProgram() {
    glDeleteProgram(this->program);
}

GLSLProgram::operator GLuint() const {
    return this->program;
}

GLSLProgramError::GLSLProgramError() :
    type(GLSLProgramErrorType::NO_ERR),
    __nullfield(nullptr) {}

GLSLProgramError::GLSLProgramError(const GLSLProgramError & other) {
    this->type = other.type;
    switch (this->type) {
        case GLSLProgramErrorType::FAILED_TO_LOAD_SOURCE:
            new (&this->failed_to_load_source.source_path) std::wstring(other.failed_to_load_source.source_path);
        break;
        case GLSLProgramErrorType::INCOMPLETE_SOURCE_SET:
            this->incomplete_source_set.fragment_shader_missing = other.incomplete_source_set.fragment_shader_missing;
            this->incomplete_source_set.vertex_shader_missing = other.incomplete_source_set.vertex_shader_missing;
        break;
        case GLSLProgramErrorType::COMPILATION_ERROR:
            this->compilation_error.shader_type = other.compilation_error.shader_type;
            new (&this->compilation_error.log) std::string(other.compilation_error.log);
        break;
        case GLSLProgramErrorType::LINKING_ERROR:
            new (&this->linking_error.log) std::string(other.linking_error.log);
        break;
        default: break;
    }
}

GLSLProgramError::~GLSLProgramError() {
    switch (this->type) {
        case GLSLProgramErrorType::FAILED_TO_LOAD_SOURCE:
            this->failed_to_load_source.source_path.~basic_string();
        break;
        case GLSLProgramErrorType::COMPILATION_ERROR:
            this->compilation_error.log.~basic_string();
        break;
        case GLSLProgramErrorType::LINKING_ERROR:
            this->linking_error.log.~basic_string();
        default: break;
    }
}