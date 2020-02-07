#include "normalmap.h"

#include <algorithm>
#include <glm/glm.hpp>

using namespace normal;

glm::mat3x3 makeSobelGxMat() {
    glm::mat3x3 mat;
    mat[0][0] = -1.0f;
    mat[0][1] = 0.0f;
    mat[0][2] = 1.0f;
    mat[1][0] = -2.0f;
    mat[1][1] = 0.0f;
    mat[1][2] = 2.0f;
    mat[2][0] = -1.0f;
    mat[2][1] = 0.0f;
    mat[2][3] = 1.0f;
    return mat;
}

size_t idxDataClamped(size_t img_w, size_t dx, size_t dy, size_t dw, size_t dh, size_t x, size_t y) {
    return std::clamp(y, dy, dy + dh - 1) * img_w + std::clamp(x, dx, dx + dw - 1);
}

//Sobel filter
std::vector<short> normal::generateNormalMapFromHeightMap(const std::vector<short> & data, size_t img_w, size_t x, size_t y, size_t w, size_t h) {
    //Sobel filter matrices
    //static glm::mat3x3 gxmat = makeSobelGxMat();
    //static glm::mat3x3 gymat = glm::transpose(gxmat);
    std::vector<short> nmap(data.size() * 2);
    //Convolution
    for (size_t i = y; i < y + h; i++) {
        for (size_t j = x; j < x + w; j++) {
            int sumx = 0;
            int sumy = 0;
            sumx =   -1 * data[idxDataClamped(img_w, x, y, w, h, j - 1, i - 1)]
                   +  1 * data[idxDataClamped(img_w, x, y, w, h, j + 1, i - 1)]
                   + -2 * data[idxDataClamped(img_w, x, y, w, h, j - 1, i    )]
                   +  2 * data[idxDataClamped(img_w, x, y, w, h, j + 1, i    )]
                   + -1 * data[idxDataClamped(img_w, x, y, w, h, j - 1, i + 1)]
                   +  1 * data[idxDataClamped(img_w, x, y, w, h, j + 1, i + 1)];
            sumy =   -1 * data[idxDataClamped(img_w, x, y, w, h, j - 1, i - 1)]
                   + -2 * data[idxDataClamped(img_w, x, y, w, h, j,     i - 1)]
                   + -1 * data[idxDataClamped(img_w, x, y, w, h, j + 1, i    )]
                   +  1 * data[idxDataClamped(img_w, x, y, w, h, j - 1, i + 1)]
                   +  2 * data[idxDataClamped(img_w, x, y, w, h, j,     i + 1)]
                   +  1 * data[idxDataClamped(img_w, x, y, w, h, j + 1, i + 1)];
            size_t idx = (i * img_w + j) * 2;
            nmap[idx+0] = sumx;
            nmap[idx+1] = sumy;            
        }
    }
    return nmap;
}