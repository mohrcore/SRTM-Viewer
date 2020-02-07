#pragma once

#include <functional>
#include <string>
#include <GL/glew.h>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "util.h"
#include "vbo_vao.h"

namespace graphics {
    struct Mesh {
        VertexBuffer vertex_vbo, index_vbo;
        VertexBuffer uv_vbo;
        VertexArray vao;
        GLuint primitive;
        unsigned int index_cnt;
        unsigned int vertex_cnt;
        Mesh(Mesh &) = delete;
        Mesh & operator=(Mesh &) = delete;
        Mesh();
        Mesh(VertexBuffer && vertex_vbo, VertexBuffer && uv_vbo, VertexBuffer && index_vbo, GLenum primitive);
        Mesh(Mesh &&) = default;
        Mesh & operator=(Mesh &&) = default;
    };
    struct Model3DNode {
        glm::mat4x4 transformation;
        bool does_transform;
        size_t node_meshes_beg;
        size_t node_meshes_cnt;
        size_t node_children_beg;
        size_t node_children_cnt;
    };
    struct Model3D {
        std::vector<Mesh> meshes;
        std::vector<Model3DNode> nodes;
        std::vector<size_t> node_meshes;
        std::vector<size_t> node_children;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        Model3D() = default;
        Model3D(Model3D && other) = default; //Ensure that the object is movable
        Model3D & operator=(Model3D && other) = default;
        void nodeDFS(std::function<bool(const Model3DNode &, const glm::mat4x4 &)> on_node, bool dont_use_model_transformations = false) const;
    };
}