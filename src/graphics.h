#pragma once


#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "window.h"
#include "texture.h"
#include "model3d.h"
#include "glsl_program.h"
#include "clock.h"
#include "camera3d.h"
#include "geotilematrix.h"

enum class ControlMode {
    CURSOR,
    ZOOM,
    LOOK,
    DRAG
};

enum class ViewMode {
    ORTHO,
    PERSPECTIVE,
};

enum class SkyMode {
    COLOR,
    PERLIN
};

class GraphicsController {
private:
    graphics::Window _window;
    graphics::Mesh _plane, _geosphere, _simpleplane;
    graphics::GLSLProgram _mapmesh_program, _geosphere_program, _geomapmesh_program, _orthoview_program, _space_program;
    glm::mat4x4 _ortho, _perspective;
    double _zoom_level;
    Clock _clock1;
    Clock _deltaclock;
    ControlMode _control_mode;
    ViewMode _viewmode;
    SkyMode _skymode;
    graphics::Camera3D _pcamera;
    std::pair<double, double> _current_lonlat;
    glm::vec3 _sunlight_dir_ortho, _sunlight_dir_perspective;
    float _sunlight_amt_ortho, _sunlight_amt_perspective;
    float _lod_factor;
    bool _keyspressed[5];
    double _last_cpos_x, _last_cpos_y;
    int _win_w, _win_h;
    double _dt;
    std::vector<geo::GeoTile> _tiles;
    void updateOrtho(float zoom_level);
    void drawOrtho() noexcept;
    void drawPerspective() noexcept;
    void updateCamera();
    void update2DView();
    void moveCamera(glm::vec3 direction);
    void gotoCoordinates(std::pair<double, double> coords);
    void nudgeLOD(double dt);
    static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow * window, double xpos, double ypos);
public:
    GraphicsController(graphics::Window && window);
    void init();
    void mainLoop();
    bool command(const std::string & cmd);
};