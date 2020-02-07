/*
#include "tiffimport.h"

#include <vector>

#include <tiffio.h>

using namespace tiffimg;

GeoTiffImage tiffimg::importGeoTiffFromFile(const std::string && path) {
    TIFF * tiffimg = TIFFOpen(&path[0], "r");
    uint32 width, height;
    TIFFGetField(tiffimg, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tiffimg, TIFFTAG_IMAGELENGTH, &height);
    std::vector<short> data(width * height);
    tmsize_t sl_sz = TIFFScanlineSize(tiffimg);
    for (size_t i = 0; i < height; i++) {
        size_t idx = i * width;
        TIFFReadScanline(tiffimg, &data[idx], i);
    }
    TIFFClose(tiffimg);
    GeoTiffImage img;
    img.data = std::move(data);
}
*/