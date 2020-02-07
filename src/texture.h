#pragma once

#include <GL/glew.h>

namespace graphics {
    struct Texture2D {
        GLuint id;
        Texture2D();
        Texture2D(
            const void * data, size_t width,
            size_t height, GLenum source_format,
            GLenum internal_format, GLenum type,
            GLenum mag_filter,
            GLenum min_filter,
            bool generate_mipmaps = true
        );
        Texture2D(Texture2D && other);
        Texture2D(Texture2D &) = delete;
        Texture2D & operator=(Texture2D &) = delete;
        Texture2D & operator=(Texture2D && other);
        operator GLuint() const;
        ~Texture2D();
    };
}