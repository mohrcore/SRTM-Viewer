#include "srtm.h"

#include <fstream>
#include <vector>

#include "util.h"

using namespace srtm;

SRTMData::SRTMData(std::vector<short> && data, SRTMType type) :
    data(std::move(data)),
    type(type)
{}

short SRTMData::idx(size_t x, size_t y) const {
    return this->data[x + y * (size_t)this->type];
}

float SRTMData::idx_window3(size_t x, size_t y) {
    float v = this->idx(x, y);
    float div = 1.0f;
    if (x > 0) {
        v += this->idx(x - 1, y);
        div += 1.0f;
    }
    if (x < (size_t)this->type) {
        v += this->idx(x + 1, y);
        div += 1.0f;
    }
    if (y > 0) {
        v += this->idx(x, y - 1);
        div += 1.0f;
    }
    if (y < (size_t)this->type) {
        v += this->idx(x, y + 1);
        div += 1.0f;
    }
    if (x > 0 && y > 0) {
        v += this->idx(x - 1, y - 1) * 0.29289322f;
        div += 0.29289322;
    }
    if (x < (size_t)this->type && y > 0) {
        v += this->idx(x + 1, y - 1) * 0.29289322f;
        div += 0.29289322;
    }
    if (x > 0 && y < (size_t)this->type) {
        v += this->idx(x - 1, y + 1) * 0.29289322f;
        div += 0.29289322;
    }
    if (x < (size_t)this->type && y < (size_t)this->type) {
        v += this->idx(x + 1, y + 1) * 0.29289322f;
        div += 0.29289322;
    }
    return v / div;
}

SRTMData srtm::importSRTM(const std::string & path, SRTMType type) {
    std::ifstream file(path);
    if (!file.good())
        throw (SRTMImportError::CANT_OPEN_FILE);
    size_t fsz = getStreamSize(file);

    std::vector<char> data(fsz);
    std::vector<short> pdata(fsz);
    file.read(&data[0], fsz);
#ifdef ARCH_BIG_ENDIAN
    for (size_t i = 0; i < fsz / 2; i++)
        pdata[i] = (short)(((short)data[i*2+1] << 8 & 0xff00 | (short(data[i*2 ]) & 0x00ff)));
#else
    for (size_t i = 0; i < fsz / 2; i++)
        pdata[i] = (short)(((short)data[i*2] << 8 & 0xff00 | (short(data[i*2 + 1]) & 0x00ff)));
#endif
    return SRTMData(std::move(pdata), type);
}

//std::vector<SRTMData> srtm::importSRTMGeoTiff5x5()