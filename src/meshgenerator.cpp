#include "meshgenerator.h"

//#include <iostream>

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "vbo_vao.h"

using namespace gen;

std::vector<float> gen::generatePlaneMeshVertices(unsigned int cuts) {
    std::vector<float> data((2 + cuts) * (2 + cuts) * 9);
    float div = cuts + 1;
    for (size_t i = 0; i < cuts + 2; i++) {
        for (size_t j = 0; j < cuts + 2; j++) {
            //Position
            size_t idx = (j + i * (cuts + 2)) * 9;
            data[idx+0] = float(j) * 2.0f / div - 1.0f;
            data[idx+1] = float(i) * 2.0f / div - 1.0f;
            data[idx+2] = 0.0f;
            //Normal
            data[idx+3] = 0.0f;
            data[idx+4] = 0.0f;
            data[idx+5] = 1.0f;
            //Tangent
            data[idx+6] = 0.0f;
            data[idx+7] = 1.0f;
            data[idx+8] - 0.0f;
        }
    }
    return data;
}

std::vector<unsigned int> gen::generatePlaneMeshIndices(unsigned int cuts) {
    std::vector<unsigned int> data((cuts + 1) * (cuts + 1) * 6);
    for (size_t i = 0; i < cuts + 1; i++) {
        for (size_t j = 0; j < cuts + 1; j++) {
            size_t idx = (j + i * (cuts + 1)) * 6;
            unsigned int vidx = j + i * (cuts + 2);
            data[idx+0] = vidx;
            data[idx+1] = vidx + cuts + 2 + 1;
            data[idx+2] = vidx + cuts + 2;
            data[idx+3] = vidx;
            data[idx+4] = vidx + 1;
            data[idx+5] = vidx + cuts + 2 + 1;
        }
    }
    return data;
}

std::vector<unsigned int> gen::generatePlaneMeshQuadIndices(unsigned int cuts) {
    std::vector<unsigned int> data((cuts + 1) * (cuts + 1) * 4);
    for (size_t i = 0; i < cuts + 1; i++) {
        for (size_t j = 0; j < cuts + 1; j++) {
            size_t idx = (j + i * (cuts + 1)) * 4;
            unsigned int vidx = j + i * (cuts + 2);
            data[idx+0] = vidx;
            data[idx+1] = vidx + 1 ;
            data[idx+2] = vidx + cuts + 2 + 1;
            data[idx+3] = vidx + cuts + 2;
        }
    }
    return data;
}

std::vector<float> gen::generatePlaneMeshUVs(unsigned int cuts) {
    std::vector<float> data((2 + cuts) * (2 + cuts) * 3);
    float div = cuts + 1;
    for (size_t i = 0; i < cuts + 2; i++) {
        for (size_t j = 0; j < cuts + 2; j++) {
            size_t idx = (j + i * (cuts + 2)) * 3;
            data[idx+0] = float(j) / div;
            data[idx+1] = 1.0f - float(i) / div;
            data[idx+2] = 1.0f;
        }
    }
    return data;
}

graphics::Mesh gen::generatePlaneMeshMesh(unsigned int cuts) {
    auto vertices = generatePlaneMeshVertices(cuts);
    auto indices =  generatePlaneMeshIndices(cuts);
    auto uvs =      generatePlaneMeshUVs(cuts);
    return graphics::Mesh (
        graphics::VertexBuffer(&vertices[0], vertices.size() / 9, sizeof(float) * 9, GL_STATIC_DRAW),
        graphics::VertexBuffer(&uvs[0], uvs.size() / 3, sizeof(float) * 3, GL_STATIC_DRAW),
        graphics::VertexBuffer(&indices[0], indices.size(), sizeof(unsigned int), GL_STATIC_DRAW),
        GL_TRIANGLES
    );
}

graphics::Model3D gen::generatePlaneMeshModel3D(unsigned int cuts) {
    graphics::Mesh mesh = generatePlaneMeshMesh(cuts);
    graphics::Model3D model;
    model.meshes.push_back(std::move(mesh));
    //Root node
    model.nodes.push_back(graphics::Model3DNode {
        glm::identity<glm::mat4x4>(), false,  // Node transform (disabled)
        0, 1,                                 // Meshes in node
        0, 0                                  // Children
    });
    model.position = glm::vec3 {0.0f,  0.0f, 0.0f};
    model.rotation = glm::identity<glm::quat>();
    model.scale = glm::vec3 {1.0f, 1.0f, 1.0f};
    return model;
}

graphics::Mesh gen::generatePlaneMeshQuadMesh(unsigned int cuts) {
    auto vertices = generatePlaneMeshVertices(cuts);
    auto indices =  generatePlaneMeshQuadIndices(cuts);
    auto uvs =      generatePlaneMeshUVs(cuts);
    return graphics::Mesh (
        graphics::VertexBuffer(&vertices[0], vertices.size() / 6, sizeof(float) * 6, GL_STATIC_DRAW),
        graphics::VertexBuffer(&uvs[0], uvs.size() / 3, sizeof(float) * 3, GL_STATIC_DRAW),
        graphics::VertexBuffer(&indices[0], indices.size(), sizeof(unsigned int), GL_STATIC_DRAW),
        GL_QUADS
    );
}

std::vector<float> gen::generateGeoSphereVertices(unsigned int latcnt, unsigned int loncnt) {
    const glm::vec3 upvec {0.0f, 1.0f, 0.0f};
    const glm::vec3 rightvec {1.0f, 0.0f, 0.0f};
    std::vector<float> data((loncnt * 2 + 1) * ((latcnt + 2) * 9));
    for (size_t i = 0; i < latcnt + 2; i++) {
        for (size_t j = 0; j < loncnt * 2 + 1; j++) {
            //glm::quat rot = glm::rotate(glm::identity<glm::quat>(), float(M_PI / double((latcnt + 1))) * i, rightvec);
            //rot = glm::rotate(rot, float(M_PI / double(loncnt)) * j, upvec);
            //glm::vec3 vxpos = glm::rotate(glm::identity<glm::quat>(), float(M_PI / double(loncnt)) * j, upvec) * rot * upvec;
            float lon = float(M_PI / double((latcnt + 1))) * i;
            float lat = float(M_PI / double(loncnt)) * j;
            glm::vec3 vxpos = glm::vec3(glm::sin(lon) * glm::sin(lat), glm::cos(lon), glm::sin(lon) * glm::cos(lat));
            glm::vec3 tan = glm::cross(vxpos, glm::vec3(glm::cos(lat), -glm::sin(lat), 0.0f));
            size_t idx = (j + i * (loncnt * 2 + 1)) * 9;
            //Position
            data[idx+0] = vxpos.x;
            data[idx+1] = vxpos.y;
            data[idx+2] = vxpos.z;
            //Normal
            data[idx+3] = vxpos.x;
            data[idx+4] = vxpos.y;
            data[idx+5] = vxpos.z;
            //Tangent
            data[idx+6] = tan.x;
            data[idx+6] = tan.y;
            data[idx+6] = tan.z;
            //std::cout << "x: " << vxpos.x << " y: " << vxpos.y << " z: " << vxpos.z << std::endl;
        }
    }
    return data;
}

std::vector<float> gen::generateGeoSphereCylindricalUVs(unsigned int latcnt, unsigned int loncnt) {
    std::vector<float> data((loncnt * 2 + 1) * ((latcnt + 2) * 3));
    for (size_t i = 0; i < latcnt + 2; i++) {
        double loncos = glm::cos(M_PI * double(i) / (latcnt + 1));
        float ycoord = float(1.0 - (loncos + 1.0) / 2.0);
        for (size_t j = 0; j < loncnt * 2 + 1; j++) {
            size_t idx = (j + i * (latcnt + 1)) * 3;
            //UV
            data[idx+0] = float(j) / loncnt * 2;
            data[idx+1] = ycoord;
            data[idx+2] = 1.0f;
        }
    }
    return data;
}

std::vector<unsigned int> gen::generateGeoSphereLineIndices(unsigned int latcnt, unsigned int loncnt) {
    std::vector<unsigned int> data((loncnt * 4) * (latcnt + 1) + latcnt * (loncnt * 4));
    for (size_t i = 0; i < loncnt * 2; i++) {
        for (size_t j = 0; j < (latcnt + 1); j++) {
            size_t idx = (i * (latcnt + 1) + j) * 2;
            data[idx+0] = j * (loncnt * 2 + 1) + i;
            data[idx+1] = (j + 1) * (loncnt * 2 + 1) + i;
        }
    }
    size_t b2 = (loncnt * 2) * (latcnt + 1) * 2;
    for (size_t i = 0; i < latcnt; i++) {
        for (size_t j = 0; j < (loncnt * 2); j++) {
            size_t idx = (i * (loncnt * 2) + j) * 2 + b2;
            data[idx+0] = (i + 1) * (loncnt * 2 + 1) + j;
            data[idx+1] = (i + 1)* (loncnt * 2 + 1) + j + 1;
        }
    }
    return data;
}

graphics::Mesh gen::generateGeoSphereLineMesh(unsigned int latcnt, unsigned int loncnt) {
    auto vertices = generateGeoSphereVertices(latcnt, loncnt);
    auto uvs      = generateGeoSphereCylindricalUVs(latcnt, loncnt);
    auto indices  = generateGeoSphereLineIndices(latcnt, loncnt);
    return graphics::Mesh(
        graphics::VertexBuffer(&vertices[0], vertices.size() / 6, sizeof(float) * 6, GL_STATIC_DRAW),
        graphics::VertexBuffer(&uvs[0], uvs.size() / 3, sizeof(float) * 3, GL_STATIC_DRAW),
        graphics::VertexBuffer(&indices[0], indices.size(), sizeof(unsigned int), GL_STATIC_DRAW),
        GL_LINES
    );
}