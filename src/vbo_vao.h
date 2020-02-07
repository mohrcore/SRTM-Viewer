#pragma once

#include <functional>

#include <GL/glew.h>

namespace graphics {
    class AbstractVBO {
    public:
        virtual operator GLuint() const =0;
        virtual size_t length() const =0;
        virtual size_t elementSize() const =0;
        virtual ~AbstractVBO();
    };
    struct VertexBuffer : public AbstractVBO {
        GLuint vbo;
        size_t len;
        size_t elem_sz;
        size_t length() const override;
        size_t elementSize() const override;
        operator GLuint() const override;
        VertexBuffer();
        VertexBuffer(GLuint vbo, size_t length, size_t elem_sz);
        VertexBuffer(VertexBuffer &) = delete;
        VertexBuffer(void * data, size_t length, size_t elem_sz, GLenum usage);
        VertexBuffer & operator=(VertexBuffer & other) = delete;
        VertexBuffer(VertexBuffer &&);
        VertexBuffer & operator=(VertexBuffer && other);
        ~VertexBuffer() override;
    };
    struct VertexArray {
        GLuint vao;
        operator GLuint() const;
        VertexArray();
        VertexArray(GLuint vao);
        VertexArray(VertexArray &) = delete;
        VertexArray & operator=(VertexArray & other) = delete;
        VertexArray(VertexArray &&);
        VertexArray & operator=(VertexArray && other);
        ~VertexArray();
    };
    class VectorVBO : public AbstractVBO {
    private:
        size_t _buf_len;
        size_t _elem_cnt;
        size_t _elem_sz;
        size_t _min_buf_len;
        void expand();
        GLuint _vbo;
    public:
        VectorVBO(size_t element_sz, size_t min_buf_len);
        VectorVBO(VectorVBO &) = delete;
        VectorVBO & operator=(VectorVBO &) = delete;
        VectorVBO(VectorVBO && other);
        ~VectorVBO() override;
        void push(size_t len, const void * data);
        void remove(size_t idx_beg, size_t idx_end);
        void pop(size_t cnt);
        void clear();
        void modify(std::function<void(void *)> & mod);
        bool empty() const;
        size_t length() const override;
        size_t elementSize() const override;
        size_t capacity() const;
        operator GLuint() const override;
    };
}