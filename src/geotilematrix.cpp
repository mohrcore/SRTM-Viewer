#include "geotilematrix.h"

#include <unordered_map>
#include <utility>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "coordresolver.h"

using namespace geo;

int geoGridHash5deg(double lon, double lat) {
    return std::floor(lat) + 360 * std::floor(lon);
}

//n log n
GeoMatrix::GeoMatrix(std::vector<GeoTile> && gts) :
    _loaded_tiles(std::move(gts))
{
    std::unordered_map<int, size_t> geopos_to_ar_pos;
    for (size_t i = 0; i < this->_loaded_tiles.size(); i++) {
        auto & gt = this->_loaded_tiles[i];
        geopos_to_ar_pos[geoGridHash5deg(gt.lon, gt.lat)] = i;
    }
    for (size_t i = 0; i < this->_loaded_tiles.size(); i++) {
        int geog = geoGridHash5deg(this->_loaded_tiles[i].lon, this->_loaded_tiles[i].lat);
        this->_neighbours[i][0] = i;
        this->_neighbours[i][1] = i;
        this->_neighbours[i][2] = i;
        this->_neighbours[i][3] = i;
        //West neighbour
        if (geopos_to_ar_pos.count(geog-1))
            this->_neighbours[i][0] = geopos_to_ar_pos[geog-1];
        //East neighbour
        if (geopos_to_ar_pos.count(geog+1))
            this->_neighbours[i][1] = geopos_to_ar_pos[geog+1];
        //North neighbur
        if (geopos_to_ar_pos.count(geog-360))
            this->_neighbours[i][2] = geopos_to_ar_pos[geog-360];
        //South neighbour
        if (geopos_to_ar_pos.count(geog+360))
            this->_neighbours[i][3] = geopos_to_ar_pos[geog+360];
    }
}



graphics::Texture2D createElevationPOTfromSRTM3Data(const srtm::SRTMData & data) {
    std::vector<short> gpudata(2048 * 2048);
    for (size_t i = 0; i < (size_t)srtm::SRTMType::SRTM3; i++)
        for (size_t j = 0; j < (size_t)srtm::SRTMType::SRTM3; j++)
            gpudata[i * 2048 + j] = short(data.idx(j, i));
    return graphics::Texture2D(
        &gpudata[0],
        2048,
        2048,
        GL_RED, GL_R16_SNORM, GL_SHORT,
        GL_LINEAR, GL_LINEAR
    );
}

GeoTile geo::loadGeoTileFromSRTM3File(const std::string & path) {
    srtm::SRTMData srtm_data = srtm::importSRTM(path, srtm::SRTMType::SRTM3);
    auto tilecoords = coordresolver::resolveCoordinatesFromFilename(path);
    auto height_texture = createElevationPOTfromSRTM3Data(srtm_data);
    GeoTile tile {
        tilecoords.first,
        tilecoords.second,
        std::move(height_texture)
    };
}

void LODMatrix::calculateLODsForView(glm::vec3 camera_position) {
    for (size_t i = 0; i < this->_loaded_tiles.size(); i++) {
        auto & tile = this->_loaded_tiles[i];
        const float performance_bias = 0; //To be replaced in future by a value adapting to current performacne
        double lon = tile.lon + 0.5; //Center of tile
        double lat = tile.lat + 0.5; //Center of tile
        glm::vec3 tile_pos { 0.0f, 1.0f, 0.0f };
        tile_pos = glm::rotate(glm::identity<glm::quat>(), float(glm::radians(lon)), glm::vec3 {1.0f, 0.0f, 0.0f}) * tile_pos;
        tile_pos = glm::rotate(glm::identity<glm::quat>(), float(glm::radians(lat)), glm::vec3 {0.0f, 1.0f, 0.0f}) * tile_pos;
        float tile_distance = glm::distance(tile_pos, camera_position);
        LOD lod;
        if (tile_distance - performance_bias < 0.01)
            lod = LOD::ANOTHER;
        else if (tile_distance - performance_bias < 0.02)
            lod = LOD::HYPER;
        else if (tile_distance - performance_bias < 0.04)
            lod = LOD::VERY_GOOD;
        else if (tile_distance - performance_bias < 0.08)
            lod = LOD::GOOD;
        else if (tile_distance - performance_bias < 0.16)
            lod = LOD::NORMAL;
        else if (tile_distance - performance_bias < 0.32)
            lod = LOD::LOW;
        else if (tile_distance - performance_bias < 0.64)
            lod = LOD::VERY_LOW;
        else if (tile_distance - performance_bias < 1.28)
            lod = LOD::POOR;
        else
            lod = LOD::NO_DETAIL;
        this->_tile_lod[i] = lod;
    }
}

QuadTesselationSettings LODMatrix::findTesselationSettingForTile(size_t idx) const {
    return QuadTesselationSettings {
        .tile_lod   = this->_tile_lod[idx],
        .left_lod   = this->_tile_lod[this->_neighbours[idx][0]],
        .right_lod  = this->_tile_lod[this->_neighbours[idx][1]],
        .top_lod    = this->_tile_lod[this->_neighbours[idx][2]],
        .bottom_lod = this->_tile_lod[this->_neighbours[idx][3]]
    };
}