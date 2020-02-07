#include <iostream>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <sstream>

#include "window.h"
#include "init.h"
#include "glsl_program.h"
#include "error.h"
#include "graphics.h"
#include "tiffimport.h"
#include "coordresolver.h"
#include "directory_scanner.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 680

std::string strReslovedCoords(const std::string & path) {
    auto cds = coordresolver::resolveCoordinatesFromFilename(path);
    std::stringstream ss;
    ss << cds.first << ", " << cds.second;
    return ss.str();
}

void testCoordResolver() {
    auto c1 = strReslovedCoords("N45E006.hgt");
    std::cout << "N45E006.hgt : " << c1 << std::endl;
    c1 = strReslovedCoords("folder/anotherfolder/N45E006.hgt");
    std::cout << "folder/anotherfolder/N45E006.hgt : " << c1 << std::endl;
    c1 = strReslovedCoords("S31E2");
    std::cout << "S31E2 : " << c1 << std::endl;
    c1 = strReslovedCoords("S31e2");
    std::cout << "S31e2 : " << c1 << std::endl;
    c1 = strReslovedCoords("w2n80.baba");
    std::cout << "w2n80.baba : " << c1 << std::endl;
    c1 = strReslovedCoords("w2n80.baba/e301N41");
    std::cout << "w2n80.baba/e301N41 : " << c1 << std::endl;
}

void testDirScan() {
    dir::lsd("data/", [&](std::string path) {
        std::cout << "Path: " << path << std::endl;
        return true;
    });
}

bool buildLoadCommand(int argc, const char * argv[], std::string & str) {
    std::string minlon, maxlon, minlat, maxlat;
    minlon = "0";
    maxlon = "180";
    minlat = "0";
    maxlat = "360";
    if (argc > 2) {
        for (int i = 2; i < argc; i ++) {
            if (std::string(argv[i]) == "-sz") {
                if (i + 2 >= argc) {
                    std::cout <<"Insufficient argument count for flag -sz";
                    return false;
                }
                minlon = argv[i+1];
                maxlon = argv[i+2];
            }
            if (std::string(argv[i]) == "-dl") {
                if (i + 2 >= argc) {
                    std::cout <<"Insufficient argument count for flag -dl";
                    return false;
                }
                minlat = argv[i+1];
                maxlat = argv[i+2];
            }
        }
    }
    std::stringstream load_cmd_ss;
    load_cmd_ss << "load-data " << argv[1] << " " << minlon << " " << maxlon << " " << minlat << " " << maxlat;
    str = std::move(load_cmd_ss.str());
    return true;
}

int main(int argc, const char * argv[]) {
    glewExperimental = true;
    init::initGLFW();
    graphics::Window main_window;
    main_window.init(u8"SRTM View", WINDOW_WIDTH, WINDOW_HEIGHT);
    init::initGLEW();
    GraphicsController gfx_ctrl(std::move(main_window));
    try {
        gfx_ctrl.init();
    } catch (graphics::GLSLProgramError e) {
        error::printGLSLProgramError(e, L"loading shaders", std::wcout);
        return -1;
    }
    //testCoordResolver();
    //testDirScan();
    //auto img = tiffimg::importGeoTiffFromFile(u8"data/srtm_40_02.tif");
    if (argc >= 2) {
        std::string command;
        if (buildLoadCommand(argc, argv, command)) {
            gfx_ctrl.command(command);
        }
    }
    gfx_ctrl.mainLoop();
    return 0;
}