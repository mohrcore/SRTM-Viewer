#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <functional>

#include "texture.h"
#include "srtm.h"


//IGNORE MOST OF THIS!!!!
//Only geotile struct is used!

// --------------------------------- LOD MODES: ---------------------------------
// * MULtI16x16: 16x16 individual quads per tile with varying tesselation
//   for close views
// * GRID16x16: 16x16 quad mesh per tile for high-resolution tesselation
//   on entire tile
// * SINGLE_QUAD: Single quad per tile for low-resolution tesselation
//   on tile, for distanced view
// ------------------------------------------------------------------------------

namespace geo {
    struct GeoTile {
        double lon, lat;
        graphics::Texture2D heightmap_tex;
        graphics::Texture2D normal_tex;
    };

    class GeoMatrix {
    protected:
        std::vector<GeoTile> _loaded_tiles;
        std::vector<size_t[4]> _neighbours;
    public:
        GeoMatrix(std::vector<GeoTile> && gts);
    };

    GeoTile loadGeoTileFromSRTM3File(const std::string & path);

    enum class LODMode {
        SINGLE_QUAD,
        GRID16x16,
        MULTI16x16 //No supported yet
    };

    enum class LOD : unsigned int {
        NO_DETAIL = 0,
        POOR = 16,
        VERY_LOW = 32,
        LOW = 64,
        NORMAL = 128,
        GOOD = 256,
        VERY_GOOD = 512,
        HYPER = 1024,
        ANOTHER = 1300
        // LUNATIC = ??????????????
    };

    struct QuadTesselationSettings {
        LOD tile_lod;
        LOD left_lod;
        LOD right_lod;
        LOD top_lod;
        LOD bottom_lod;
    };

    class LODMatrix : public GeoMatrix {
    protected:
        std::vector<LOD> _tile_lod;
        QuadTesselationSettings findTesselationSettingForTile(size_t idx) const;
    public:
        void calculateLODsForView(glm::vec3 camera_pos);
        void forTile(std::function<void(const GeoTile &, QuadTesselationSettings)> fun) const;
    };
}