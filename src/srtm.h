#pragma once

#include <string>
#include <vector>

#include "util.h"
#include "tiffimport.h"

namespace srtm {
    enum class SRTMType : size_t {
        SRTM1 = 3601,
        SRTM3 = 1201
    };

    struct SRTMData {
        SRTMData(std::vector<short> && data, SRTMType type);
        const SRTMType type;
        std::vector<short> data;
        short idx(size_t x, size_t y) const;
        float idx_window3(size_t x, size_t y);
    };
    SRTMData importSRTM(const std::string & path, SRTMType srtm_type);
    std::vector<SRTMData> importSRTMGeoTiff5x5(const std::string & path);
    
    enum class SRTMImportError {
        CANT_OPEN_FILE,
        UNSUPPORTED_FORMAT
    };
}