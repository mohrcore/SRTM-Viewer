#include "model3d.h"

#include <queue>

#include <iostream>

using namespace graphics;

const float test_vbo_vertex[] = {
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
};

const float test_vbo_uv[] = {
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
};

const unsigned int test_vbo_idx[] = {0, 1, 2, 1, 2, 3};


Mesh::Mesh() :
    vertex_cnt(0),
    index_cnt(0),
    primitive(GL_TRIANGLES)
{}

Mesh::Mesh(VertexBuffer && vertex_vbo, VertexBuffer && uv_vertex_vbovbo, VertexBuffer && index_vbo, GLenum primitive) :
    vertex_vbo(std::move(vertex_vbo)),
    index_vbo(std::move(index_vbo)),
    uv_vbo(std::move(uv_vertex_vbovbo)),
    primitive(primitive)
{
    this->index_cnt = this->index_vbo.length();
    this->vertex_cnt = this->vertex_vbo.length();
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_vbo);
    //Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
    //Normals
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    //Tangents
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_vbo);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    //UVs
    glBindBuffer(GL_ARRAY_BUFFER, this->uv_vbo);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glBindVertexArray(0);
}

void nodeDFS_dfs(
    const Model3DNode & node,
    const glm::mat4x4 & current_transform,
    const std::vector<Model3DNode> & nodes,
    const std::vector<size_t> & node_children,
    std::function<bool(const Model3DNode &, const glm::mat4x4 &)> & on_node
) {
    if (node.does_transform) {
        glm::mat4x4 node_transform = current_transform * node.transformation;
        bool stop = on_node(node, node_transform);
        if (!stop)
            for (size_t i = node.node_children_beg; i < node.node_children_beg + node.node_children_cnt; i++)
                nodeDFS_dfs(nodes[node_children[i]], node_transform, nodes, node_children, on_node);
    } else {
        //Skip pushing a whole matrix to stack if the node doesn't transform
        bool stop = on_node(node, current_transform);
        if (!stop)
            for (size_t i = node.node_children_beg; i < node.node_children_beg + node.node_children_cnt; i++)
                nodeDFS_dfs(nodes[node_children[i]], current_transform, nodes, node_children, on_node);
    }
}

void Model3D::nodeDFS(std::function<bool(const Model3DNode &, const glm::mat4x4 &)> on_node, bool dont_use_model_transformations) const {
    Model3DNode rootnode = this->nodes[0];
    glm::mat4x4 initial_transform;
    if (dont_use_model_transformations) {
        initial_transform = glm::identity<glm::mat4x4>();
    } else {
        initial_transform = glm::translate(glm::identity<glm::mat4x4>(), this->position);
        initial_transform = glm::scale(initial_transform, this->scale);
        initial_transform = initial_transform * (glm::mat4x4)this->rotation;
    }
    nodeDFS_dfs(rootnode, initial_transform, this->nodes, this->node_children, on_node);
}