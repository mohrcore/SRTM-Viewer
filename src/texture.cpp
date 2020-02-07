#include "texture.h"

using namespace graphics;

Texture2D::Texture2D() : id(0) {}

Texture2D::Texture2D(
    const void * data,
    size_t width,
    size_t height,
    GLenum source_format,
    GLenum internal_format,
    GLenum type,
    GLenum mag_filter,
    GLenum min_filter,
    bool generate_mipmaps
) {
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, source_format, type, data);
    if (generate_mipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mag_filter);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //float border_color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);  
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(Texture2D && other) :
    id(other.id)
{
    other.id = 0;
}

Texture2D & Texture2D::operator=(Texture2D && other) {
    if (this->id)
        glDeleteTextures(1, &this->id);
    this->id = other.id;
    other.id = 0;
}

Texture2D::operator GLuint() const {
    return this->id;
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &this->id);
}