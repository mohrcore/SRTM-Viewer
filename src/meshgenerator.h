#pragma once

#include "model3d.h"

namespace gen {
    std::vector<float> generatePlaneMeshVertices(unsigned int cuts);
    std::vector<unsigned int> generatePlaneMeshIndices(unsigned int cuts);
    std::vector<float> generatePlaneMeshUVs(unsigned int cuts);
    std::vector<unsigned int> generatePlaneMeshQuadIndices(unsigned int cuts);
    graphics::Mesh generatePlaneMeshMesh(unsigned int cuts);
    graphics::Model3D generatePlaneMeshModel3D(unsigned int cuts);
    graphics::Mesh generatePlaneMeshQuadMesh(unsigned int cuts);

    std::vector<float> generateGeoSphereVertices(unsigned int latcnt, unsigned int loncnt);
    std::vector<float> generateGeoSphereCylindricalUVs(unsigned int latcnt, unsigned int loncnt);
    std::vector<unsigned int> generateGeoSphereLineIndices(unsigned int latcnt, unsigned int loncnt);
    graphics::Mesh generateGeoSphereLineMesh(unsigned int latcnt, unsigned int loncnt);
}