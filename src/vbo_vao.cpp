#include "vbo_vao.h"

#include <iostream>

using namespace graphics;

AbstractVBO::~AbstractVBO() {}

VertexArray::VertexArray() {
    glGenVertexArrays(1, &this->vao);
}
VertexArray::VertexArray(GLuint vao) :
    vao(vao) {}

VertexArray::VertexArray(VertexArray && other) :
    vao(other.vao)
{
    other.vao = 0;
}

VertexArray & VertexArray::operator=(VertexArray && other) {
    this->vao = other.vao;
    other.vao = 0;
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &this->vao);
}

VertexArray::operator GLuint() const {
    return this->vao;
}

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &this->vbo);
}

VertexBuffer::VertexBuffer(GLuint vbo_, size_t length_, size_t elem_sz_) :
    vbo(vbo_),
    len(length_),
    elem_sz(elem_sz_) {}

VertexBuffer::VertexBuffer(VertexBuffer && other) :
    vbo(other.vbo),
    len(other.len)
{
    other.vbo = 0;
    other.len = 0;
}

VertexBuffer::VertexBuffer(void * data, size_t length,size_t elem_sz, GLenum usage) :
    len(length),
    elem_sz(elem_sz)
{
    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, length * elem_sz, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer & VertexBuffer::operator=(VertexBuffer && other) {
    this->vbo = other.vbo;
    this->len = other.len;
    other.vbo = 0;
    return *this;
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &this->vbo);
}

size_t VertexBuffer::length() const {
    return this->len;
}

size_t VertexBuffer::elementSize() const {
    return this->elem_sz;
}

VertexBuffer::operator GLuint() const {
    return this->vbo;
}

VectorVBO::VectorVBO(size_t element_sz, size_t min_buf_len)
    : _min_buf_len(min_buf_len), _elem_sz(element_sz) , _elem_cnt(0), _buf_len(min_buf_len) {
    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, min_buf_len * element_sz, nullptr, GL_DYNAMIC_DRAW);
}

VectorVBO::VectorVBO(VectorVBO && other)
    : _min_buf_len(other._min_buf_len),
      _elem_sz(other._elem_sz),
      _elem_cnt(other._elem_cnt),
      _buf_len(other._buf_len),
      _vbo(other._vbo) {
    other._vbo = 0;
}

VectorVBO::~VectorVBO() {
    glDeleteBuffers(1, &this->_vbo);
}

void VectorVBO::push(size_t len, const void * data) {
    //NOTE: should be optimized to avoid loop!
    while (this->_elem_cnt + len > this->_buf_len) {
        this->expand();
    }
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, this->_elem_cnt * _elem_sz, len * this->_elem_sz, data);
    this->_elem_cnt += len;
}

void VectorVBO::clear() {
    if (this->_buf_len > this->_min_buf_len) {
        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
        glBufferData(GL_ARRAY_BUFFER, this->_min_buf_len * this->_elem_sz, nullptr, GL_DYNAMIC_DRAW);
    }
    this->_buf_len = this->_min_buf_len;
    this->_elem_cnt = 0;
}

size_t VectorVBO::length() const {
    return this->_elem_cnt;
}

size_t VectorVBO::capacity() const {
    return this->_buf_len;
}

size_t VectorVBO::elementSize() const {
    return this->_elem_sz;
}

void VectorVBO::expand() {
    glBindBuffer(GL_COPY_READ_BUFFER, this->_vbo);
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, cvbo);
    glBufferData(GL_COPY_WRITE_BUFFER, this->_buf_len * this->_elem_sz * 2, nullptr, GL_DYNAMIC_COPY);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->_buf_len * this->_elem_sz);
    //glBufferData(GL_COPY_READ_BUFFER, this->_buf_len * this->_elem_sz * 2, nullptr, GL_DYNAMIC_DRAW);
    //glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, this->_buf_len * this->_elem_sz);
    glDeleteBuffers(1, &this->_vbo);
    this->_vbo = cvbo;
    this->_buf_len *= 2;
}

void VectorVBO::remove(size_t idx_beg, size_t idx_end) {       
    size_t rm_len = idx_end - idx_beg; //removed section length
    size_t nb_len = this->_buf_len;    //new buffer length (in elements)
    while ((this->_elem_cnt - rm_len) * 4 < nb_len && (this->_elem_cnt - rm_len) * 2 > this->_min_buf_len)
        nb_len /= 2;
    glBindBuffer(GL_COPY_READ_BUFFER, this->_vbo);
    GLuint cvbo;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_COPY_WRITE_BUFFER, cvbo);
    glBufferData(GL_COPY_WRITE_BUFFER, nb_len * this->_elem_sz, nullptr, GL_DYNAMIC_DRAW);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->_elem_sz * idx_beg);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, this->_elem_sz * idx_end, this->_elem_sz * idx_beg, this->_elem_sz * (this->_elem_cnt - idx_end));
    glDeleteBuffers(1, &this->_vbo);
    this->_vbo = cvbo;
    this->_buf_len = nb_len;
    this->_elem_cnt = this->_elem_cnt - rm_len;
}

void VectorVBO::pop(size_t cnt) {
    this->remove(this->length() - cnt, this->length());
}

bool VectorVBO::empty() const {
    return this->length() == 0;
}

VectorVBO::operator GLuint() const {
    return this->_vbo;
}

void VectorVBO::modify(std::function<void(void *)> & mod) {
    glBindBuffer(GL_COPY_WRITE_BUFFER, this->_vbo);
    void * memptr = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_READ_WRITE);
    mod(memptr);
    GLboolean res = glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    if (res == GL_FALSE) {
        std::cout << "DUPA" << std::endl;
    }
    glFlushMappedBufferRange(GL_COPY_WRITE_BUFFER, 0, this->_elem_cnt * this->_elem_sz);
}