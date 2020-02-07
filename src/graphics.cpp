#include "graphics.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <utility>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "srtm.h"
#include "meshgenerator.h"
#include "tiffimport.h"
#include "directory_scanner.h"
#include "coordresolver.h"
#include "normalmap.h"

#define TEST_CUTS 17
#define CAMERA_LOOK_CONTROL_FACTOR 0.002f
#define ORTHO_DRAG_CONTROL_FACTOR (1.0f/32.0f)

glm::mat4x4 calculateViewportTransformationMatrixFromCamera(const graphics::Camera3D & camera) noexcept {
    glm::quat rotation = camera.getRotation();
    glm::vec3 position = camera.getPosition();
    glm::mat4x4 projection = camera.getProjectionMatrix();
    /* glm::mat4x4 transform = camera.getProjectionMatrix();
    transform = glm::rotate(transform, glm::angle(rotation), glm::axis(rotation)); //Looking doesn't work
    transform = glm::translate(transform, -position); */
    glm::mat4x4 transform = glm::translate(glm::identity<glm::mat4x4>(), -position);
    //Temprary solution. There's something going wrong with quateernion-based rotations (glm::quatLookAt)
    transform = glm::lookAt(glm::vec3 {0.0f, 0.0f, 0.0f}, camera.getForwardVec(), camera.getUpVector()) * transform;
    transform = projection * transform;
    return transform;
}

//The implementation of OpenGL I'm using seems to have some kind of a problem with NPOT textures
//of GL_SHORT type, so I made a function that reallocates the SRTM3 data to create a 2048x2048 texture
std::pair<graphics::Texture2D, graphics::Texture2D> createPOTfromSRTM3Data(const srtm::SRTMData & data) {
    std::vector<short> gpudata(2048 * 2048);
    for (size_t i = 0; i < (size_t)srtm::SRTMType::SRTM3; i++)
        for (size_t j = 0; j < (size_t)srtm::SRTMType::SRTM3; j++)
            gpudata[i * 2048 + j] = short(data.idx(j, i));
    std::vector<short> normalgpudata = normal::generateNormalMapFromHeightMap(gpudata, 2048, 0, 0, (size_t)srtm::SRTMType::SRTM3, (size_t)srtm::SRTMType::SRTM3);
    return std::pair<graphics::Texture2D, graphics::Texture2D>(
        graphics::Texture2D(
            &gpudata[0],
            2048,
            2048,
            GL_RED, GL_R16_SNORM, GL_SHORT,
            GL_LINEAR, GL_LINEAR
        ),
        graphics::Texture2D(
            &normalgpudata[0],
            2048,
            2048,
            GL_RG, GL_RG16_SNORM, GL_SHORT,
            GL_LINEAR, GL_LINEAR
        )
    );
}

std::vector<geo::GeoTile> loadTilesFromDirectory(const std::string & dir, double minlon = 0, double maxlon = 365, double minlat = 0, double maxlat = 180) {
    Clock tclock;
    std::vector<geo::GeoTile> gts;
    dir::lsd(dir, [&] (std::string path) {
        Clock clock;
        std::pair<double, double> coords;
        try {
            coords = coordresolver::resolveCoordinatesFromFilename(path);
        } catch (coordresolver::CoordResolveError e) {
            std::cout << "Error resolving coordinates for \"" << path << "\"" << std::endl
                << "    File skipped!" << std::endl;
            return true;
        }
        if (coords.first < minlon || coords.first > maxlon || coords.second < minlat || coords.second > maxlat) {
            std::cout << path << ": coordinates out of range."
                << "    File skipped!" << std::endl;
            return true;
        }
        std::cout << "Processing \"" << path << "\"...";
        std::flush(std::cout);
        auto srtm_data = srtm::importSRTM(path, srtm::SRTMType::SRTM3);
        auto textures = createPOTfromSRTM3Data(srtm_data);
        gts.push_back(geo::GeoTile {
            double(coords.first),
            double(coords.second),
            std::move(textures.first),
            std::move(textures.second)
        });
        std::cout << "    Ok (" << clock.getElapsedTime() << "s)" << std::endl;
        return true;
    });
    std::cout << "Done!    (" << tclock.getElapsedTime() << "s)" << std::endl;
    return gts;
}

std::string commandInput(const std::string & ps1 = "> ") {
    std::string cmd;
    std::cout << ps1;
    std::flush(std::cout);
    std::getline(std::cin, cmd);
    return cmd;
}

glm::mat4x4 genWorldTransformMatrix(float rot) noexcept {
    glm::mat4x4 world_transform = glm::identity<glm::mat4x4>();
    world_transform = glm::translate(world_transform, glm::vec3 {0.0f, 0.0f, 0.0f});
    world_transform = glm::rotate(world_transform, float(M_PI / 3.0), glm::vec3 {1.0f, 0.0f, 0.0f});
    world_transform = glm::rotate(world_transform, rot, glm::vec3 {0.0f, 0.0f, 1.0f});
    world_transform = glm::scale(world_transform, glm::vec3(12.0f, 12.0f, 12.0f));
    return world_transform;
}

glm::mat4x4 genWorldTransformMatrixPerspective(float rot) noexcept {
    glm::mat4x4 world_transform = glm::identity<glm::mat4x4>();
    //world_transform = glm::rotate(world_transform, rot, glm::vec3 {0.0f, 1.0f, 0.0f});
    return world_transform;
}

constexpr double deg1Length() {
    //double v1x, v1y, v2x, v2y;
    double v1x = 0.0;
    double v1y = 1.0;
    double v2x = sin(glm::radians(1.0f));
    double v2y = cos(glm::radians(1.0f));
    return std::sqrt((v2x - v1x) * (v2x - v1x) + (v2y - v1y) * (v2y - v1y));
}

double calculateTileScaleFactorX(double lon) noexcept {
    const double deg1len = deg1Length();
    double r = sin(glm::radians(lon));
    double v1x = 0.0;
    double v1y = r;
    double v2x = r * sin(glm::radians(1.0f));
    double v2y = r * cos(glm::radians(1.0f));
    double cdeg1len =  std::sqrt((v2x - v1x) * (v2x - v1x) + (v2y - v1y) * (v2y - v1y));
    return cdeg1len / deg1len;
}

GraphicsController::GraphicsController(graphics::Window && window) :
    _window(std::move(window)),
    _zoom_level(1.0),
    _control_mode(ControlMode::CURSOR),
    _viewmode(ViewMode::ORTHO)
{
    glfwSetWindowUserPointer(this->_window, this);
    glfwGetCursorPos(this->_window, &this->_last_cpos_x, &this->_last_cpos_y);
    glfwSetKeyCallback(this->_window, GraphicsController::keyCallback);
    glfwSetCursorPosCallback(this->_window, GraphicsController::cursorPositionCallback);
    glfwGetWindowSize(this->_window, &this->_win_w, &this->_win_h);
    std::memset(this->_keyspressed, false, 5);
}

void GraphicsController::init() {
    this->_plane = gen::generatePlaneMeshQuadMesh(TEST_CUTS);
    this->_simpleplane = gen::generatePlaneMeshMesh(0);
    this->_geosphere = gen::generateGeoSphereLineMesh(58, 60);
    this->_mapmesh_program = graphics::compileProgramFromSourcePaths({
        {GL_VERTEX_SHADER,          {L"shaders/mapmesh.vert"}},
        {GL_FRAGMENT_SHADER,        {L"shaders/mapmesh.frag"}},
        {GL_TESS_CONTROL_SHADER,    {L"shaders/mapmesh.tessctl"}},
        {GL_TESS_EVALUATION_SHADER, {L"shaders/mapmesh.tessevl"}}
    });
    this->_geomapmesh_program = graphics::compileProgramFromSourcePaths({
        {GL_VERTEX_SHADER,          {L"shaders/geomapmesh.vert"}},
        {GL_FRAGMENT_SHADER,        {L"shaders/mapmesh.frag"}},
        {GL_TESS_CONTROL_SHADER,    {L"shaders/geomapmesh.tessctl"}},
        {GL_TESS_EVALUATION_SHADER, {L"shaders/geomapmesh.tessevl"}}
    });
    this->_geosphere_program = graphics::compileProgramFromSourcePaths({
        {GL_VERTEX_SHADER,   {L"shaders/geosphere.vert"}},
        {GL_FRAGMENT_SHADER, {L"shaders/geosphere.frag"}}
    });
    this->_orthoview_program = graphics::compileProgramFromSourcePaths({
        {GL_VERTEX_SHADER,   {L"shaders/orthoview.vert"}},
        {GL_FRAGMENT_SHADER, {L"shaders/mapmesh.frag"}}
    });
    this->_space_program = graphics::compileProgramFromSourcePaths({
        {GL_VERTEX_SHADER,   {L"shaders/spacemap.vert"}},
        {GL_FRAGMENT_SHADER, {L"shaders/perlin3d.frag", L"shaders/spacemap.frag"}}
    });
    //this->_tiles = std::move(loadTilesFromDirectory("data/"));
    this->updateOrtho(this->_zoom_level);
    this->_pcamera.setPosition({0.0f, 0.0f, 2.0f});
    this->_pcamera.setProjection(glm::radians(75.0f), float(this->_win_w) / float(this->_win_h),  0.0001f, 100.0f);
    this->_pcamera.lookAt({0.0f, 0.0f, 0.0f});
    this->_sunlight_amt_ortho = 1.0f;
    this->_sunlight_amt_perspective = 1.0f;
    this->_sunlight_dir_ortho = glm::vec3(0.0f, 0.0f, -1.0f);
    this->_sunlight_dir_perspective = glm::normalize(glm::vec3(-1.0f, -1.0f, 0.0f));
    this->_lod_factor = 1.0f;
    this->_skymode = SkyMode::PERLIN;
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	//glEnable(GL_CULL_FACE);
}

void GraphicsController::updateOrtho(float zoom_level) {
    int win_width, win_height;
    glfwGetWindowSize(this->_window, &win_width, &win_height);
    this->_ortho = glm::ortho(
        std::ceil(win_width / 2.0f) / (16.0f * zoom_level), std::ceil(-win_width / 2.0f) / (16.0f * zoom_level), 
        std::ceil(win_height / 2.0f) / (16.0f * zoom_level), std::ceil(-win_height / 2.0f) / (16.0f * zoom_level),
        -100.01f, 100.0f
    );
    //this->_ortho = glm::perspective(80.0, 1024.0 / 680.0, 0.0, 10000.0);
    //this->_ortho = glm::identity<glm::mat4x4>();
}

void GraphicsController::mainLoop() {
    this->_clock1.reset();
    this->_deltaclock.reset();
    do {
        this->_dt = this->_deltaclock.getElapsedTime();
        this->_deltaclock.reset();
        this->updateCamera();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        switch (this->_viewmode) {
            case ViewMode::ORTHO:
                this->drawOrtho();
            break;
            case ViewMode::PERSPECTIVE:
                this->drawPerspective();
            break;
        }
        glfwSwapBuffers(this->_window);
        glfwPollEvents();
    } while (glfwGetKey(this->_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(this->_window) == 0);
}

void GraphicsController::drawOrtho() noexcept {
    auto lonlat = this->_current_lonlat;
    double xfactor = calculateTileScaleFactorX(lonlat.first);
    //glm::mat4x4 world_transform = genWorldTransformMatrix(this->_clock1.getElapsedTime() / 2.0);
    //Draw map
    glUseProgram(this->_orthoview_program);
    //glPatchParameteri(GL_PATCH_VERTICES, 4);
    glUniformMatrix4fv(1, 1, GL_FALSE, &this->_ortho[0][0]);
    glUniform1f(7, this->_sunlight_amt_ortho);
    glUniform3f(6, this->_sunlight_dir_ortho.x, this->_sunlight_dir_ortho.y, this->_sunlight_dir_ortho.z);
    //glUniform1f(3, float(this->_zoom_level));
    for (auto && tile : this->_tiles) {
        glm::vec3 translation = 2.0f * (glm::vec3(float(tile.lat), -float(tile.lon), 0.0f) - glm::vec3(lonlat.second, -lonlat.first, 0.0f) + glm::vec3(0.5f, -0.5f, 0.0));
        glm::mat4x4 world_transform = glm::identity<glm::mat4x4>();
        world_transform = glm::rotate(world_transform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        world_transform = glm::scale(world_transform, glm::vec3(xfactor, 1.0f, 1.0f));
        world_transform = glm::translate(world_transform, translation);
        glUniformMatrix4fv(2, 1, GL_FALSE, &world_transform[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tile.heightmap_tex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tile.normal_tex);
        glBindVertexArray(this->_simpleplane.vao);
        glDrawElements(this->_simpleplane.primitive, this->_simpleplane.index_cnt, GL_UNSIGNED_INT, (void *)0);
        glBindVertexArray(0);
    }
}

glm::mat4x4 calculateInverseProjectionMatrixNT(const graphics::Camera3D & cam) {
    glm::mat4x4 projection = cam.getProjectionMatrix();
    //Temprary solution. There's something going wrong with quateernion-based rotations (glm::quatLookAt)
    glm::mat4x4 transform = glm::lookAt(glm::vec3 {0.0f, 0.0f, 0.0f}, cam.getForwardVec(), cam.getUpVector());
    transform = projection * transform;
    return glm::inverse(transform);
}

void GraphicsController::drawPerspective() noexcept {
    glm::mat4x4 world_transform = genWorldTransformMatrixPerspective(this->_clock1.getElapsedTime() / 2.0);
    glm::mat4x4 projview_mat = calculateViewportTransformationMatrixFromCamera(this->_pcamera);
    //Draw geosphere
    glUseProgram(this->_geosphere_program);
    glUniformMatrix4fv(1, 1, GL_FALSE, &projview_mat[0][0]);
    glUniformMatrix4fv(2, 1, GL_FALSE, &world_transform[0][0]);
    glBindVertexArray(this->_geosphere.vao);
    glDrawElements(this->_geosphere.primitive, this->_geosphere.index_cnt, GL_UNSIGNED_INT, (void *)0);
    glBindVertexArray(0);
    //Draw tiles
    glUseProgram(this->_geomapmesh_program);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glUniformMatrix4fv(1, 1, GL_FALSE, &projview_mat[0][0]);
    glUniformMatrix4fv(2, 1, GL_FALSE, &world_transform[0][0]);
    glUniform1f(7, this->_sunlight_amt_perspective);
    glUniform3f(6, this->_sunlight_dir_perspective.x, this->_sunlight_dir_perspective.y, this->_sunlight_dir_perspective.z);
    glUniform1f(8, this->_lod_factor);
    auto campos = this->_pcamera.getPosition();
    glUniform3f(5, campos.x, campos.y, campos.z);
    for (auto && geotile : this->_tiles) {
        glm::vec2 lonlat(float(geotile.lon + 0.5), float(geotile.lat + 0.5));
        glUniform2f(4, lonlat.x, lonlat.y);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, geotile.heightmap_tex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, geotile.normal_tex);
        glBindVertexArray(this->_plane.vao);
        glDrawElements(GL_PATCHES, this->_plane.index_cnt, GL_UNSIGNED_INT, (void *)0);
        glBindVertexArray(0);
    }
    //Draw background
    if (this->_skymode == SkyMode::PERLIN) {
        glm::mat4x4 bg_projview = glm::identity<glm::mat4x4>(); //Background is rendered on a quad spanning thhe entire viewport.
        glm::mat4x4 bg_wt = glm::translate(glm::identity<glm::mat4x4>(), {0.0f, 0.0f, 0.9999999f}); //Background is rendered on a quad spanning thhe entire viewport.
        glm::mat4x4 inv_projview_mat = calculateInverseProjectionMatrixNT(this->_pcamera);
        glUseProgram(this->_space_program);
        glUniformMatrix4fv(1, 1, GL_FALSE, &bg_projview[0][0]);
        glUniformMatrix4fv(2, 1, GL_FALSE, &bg_wt[0][0]);
        glUniformMatrix4fv(5, 1, GL_FALSE, &inv_projview_mat[0][0]);
        glUniform3f(6, glm::sin(0.03 * this->_clock1.getElapsedTime()), glm::cos(0.03 * this->_clock1.getElapsedTime()), glm::sin(0.01 * this->_clock1.getElapsedTime()));
        glBindVertexArray(this->_simpleplane.vao);
        glDrawElements(this->_simpleplane.primitive, this->_simpleplane.index_cnt, GL_UNSIGNED_INT, (void *)0);
        glBindVertexArray(0);
    }
}

void GraphicsController::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    auto gfx_ctrl = (GraphicsController *)glfwGetWindowUserPointer(window);
    switch (key) {
        case GLFW_KEY_Z:
            if (gfx_ctrl->_viewmode == ViewMode::ORTHO) {
                if (action == GLFW_PRESS) {
                    gfx_ctrl->_control_mode = ControlMode::ZOOM;
                    glfwSetInputMode(gfx_ctrl->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                if (action == GLFW_RELEASE) {
                    gfx_ctrl->_control_mode = ControlMode::CURSOR;
                    glfwSetInputMode(gfx_ctrl->_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
        break;
        case GLFW_KEY_TAB:
            if (action == GLFW_PRESS) {
                switch (gfx_ctrl->_viewmode) {
                    case ViewMode::ORTHO:
                        gfx_ctrl->_viewmode = ViewMode::PERSPECTIVE;
                        gfx_ctrl->_control_mode = ControlMode::LOOK;
                        glfwSetInputMode(gfx_ctrl->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    break;
                    case ViewMode::PERSPECTIVE:
                        gfx_ctrl->_viewmode = ViewMode::ORTHO;
                        gfx_ctrl->_control_mode = ControlMode::CURSOR;
                        glfwSetInputMode(gfx_ctrl->_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    break;
                }
            }
        break;
        case GLFW_KEY_W:
            if (action == GLFW_PRESS)
                gfx_ctrl->_keyspressed[0] = true;
            if (action == GLFW_RELEASE)
                gfx_ctrl->_keyspressed[0] = false;
        break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS)
                gfx_ctrl->_keyspressed[1] = true;
            if (action == GLFW_RELEASE)
                gfx_ctrl->_keyspressed[1] = false;
        break;
        case GLFW_KEY_S:
            if (action == GLFW_PRESS)
                gfx_ctrl->_keyspressed[2] = true;
            if (action == GLFW_RELEASE)
                gfx_ctrl->_keyspressed[2] = false;
        break;
        case GLFW_KEY_D:
            if (action == GLFW_PRESS)
                gfx_ctrl->_keyspressed[3] = true;
            if (action == GLFW_RELEASE)
                gfx_ctrl->_keyspressed[3] = false;
        break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS)
                gfx_ctrl->_keyspressed[4] = true;
            if (action == GLFW_RELEASE)
                gfx_ctrl->_keyspressed[4] = false;
            if (action == GLFW_PRESS && gfx_ctrl->_viewmode == ViewMode::ORTHO) {
                gfx_ctrl->_control_mode = ControlMode::DRAG;
                (gfx_ctrl->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            if (action == GLFW_RELEASE && gfx_ctrl->_viewmode == ViewMode::ORTHO) {
                gfx_ctrl->_control_mode = ControlMode::CURSOR;
                (gfx_ctrl->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        break;
        case GLFW_KEY_ENTER:
            if (action == GLFW_PRESS)
                gfx_ctrl->command(commandInput());
        break;
    }
}

void GraphicsController::moveCamera(glm::vec3 direction) {
    float distance = glm::length(this->_pcamera.getPosition()); // Geosphere is at (0, 0, 0)
    glm::vec3 fwd = glm::normalize(glm::cross(glm::cross(this->_pcamera.getUpVector(), this->_pcamera.getForwardVec()), this->_pcamera.getUpVector()));
    glm::vec3 dir = this->_pcamera.getForwardVec() * direction.z + glm::normalize(glm::cross(this->_pcamera.getUpVector(), fwd)) * direction.x + this->_pcamera.getUpVector() * direction.y;
    this->_pcamera.move({double(dir.x) * this->_dt, double(dir.y) * this->_dt, double(dir.z) * this->_dt});
    //this->_pcamera.setPosition(glm::normalize(this->_pcamera.getPosition()) * distance);
    this->_pcamera.setUpVector(glm::normalize(this->_pcamera.getPosition()));
}

void GraphicsController::updateCamera() {
    float mult = 2.0 * std::clamp((std::exp((glm::length(this->_pcamera.getPosition()) - 1.0)) - 1.0), 0.0, 5.0);
    if (this->_keyspressed[0]) {
        this->moveCamera({0.0, 0.0, mult});
    } else if (this->_keyspressed[1]) {
        this->moveCamera({mult, 0.0, 0.0});
    } else if (this->_keyspressed[2]) {
        this->moveCamera({0.0, 0.0, -mult});
    } else if (this->_keyspressed[3]) {
        this->moveCamera({-mult, 0.0, 0.0});
    }
}

void GraphicsController::update2DView() {
    //float mult = 2.0 * 
}

void GraphicsController::cursorPositionCallback(GLFWwindow * window, double xpos, double ypos) {
    auto gfx_ctrl = (GraphicsController *)glfwGetWindowUserPointer(window);
    double dx = xpos - gfx_ctrl->_last_cpos_x;
    double dy = ypos - gfx_ctrl->_last_cpos_y;
    if (gfx_ctrl->_control_mode == ControlMode::ZOOM) {
        gfx_ctrl->_zoom_level = std::clamp(gfx_ctrl->_zoom_level - dy * 0.005, 0.01, 60.0);
        gfx_ctrl->updateOrtho(gfx_ctrl->_zoom_level);
    }
    if (gfx_ctrl->_control_mode == ControlMode::LOOK) {
        float xmov = float(dx * CAMERA_LOOK_CONTROL_FACTOR);
        float ymov = float(dy * CAMERA_LOOK_CONTROL_FACTOR);
        float tilt = 0.0f;
        gfx_ctrl->_pcamera.look({-xmov, -ymov, tilt});
    }
    if (gfx_ctrl->_control_mode == ControlMode::DRAG) {
        float movx = ORTHO_DRAG_CONTROL_FACTOR / (calculateTileScaleFactorX(gfx_ctrl->_current_lonlat.first) * gfx_ctrl->_zoom_level);
        float movy = ORTHO_DRAG_CONTROL_FACTOR / gfx_ctrl->_zoom_level;
        gfx_ctrl->_current_lonlat.second -= movx * dx;
        gfx_ctrl->_current_lonlat.first -= movy * dy;
    }
    gfx_ctrl->_last_cpos_x = xpos;
    gfx_ctrl->_last_cpos_y = ypos;
}

bool GraphicsController::command(const std::string & cmd) {
    std::size_t spos = cmd.find_first_of(' ');
    std::string cmd_name = cmd.substr(0, spos);
    std::string args = cmd.substr(spos + 1, cmd.length() - spos);
    if (cmd_name == "goto") {
        std::pair<double, double> coords;
        try {
            coords = coordresolver::resolveCoordinatesFromFilename(args);
        } catch (coordresolver::CoordResolveError) {
            std::cout << "Invalid coordinate format!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        this->_current_lonlat = coords;
        return true;
    }
    if (cmd_name == "set-light-amount-ortho") {
        double amt;
        try {
            amt = std::stod(args);
        } catch (...) {
            std::cout << "Invalid number!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        this->_sunlight_amt_ortho = amt;
        return true;
    }
    if (cmd_name == "set-light-amount-perspective") {
        double amt;
        try {
            amt = std::stod(args);
        } catch (...) {
            std::cout << "Invalid number!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        this->_sunlight_amt_perspective = amt;
        return true;
    }
    if (cmd_name == "set-light-direction-ortho") {
        std::stringstream ss(args);
        try {
            ss >> this->_sunlight_dir_ortho.x;
            ss >> this->_sunlight_dir_ortho.y;
            ss >> this->_sunlight_dir_ortho.z;
        } catch (...) {
            std::cout << "Invalid number!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        return true;
    }
    if (cmd_name == "set-light-direction-perspective") {
        std::stringstream ss(args);
        try {
            ss >> this->_sunlight_dir_perspective.x;
            ss >> this->_sunlight_dir_perspective.y;
            ss >> this->_sunlight_dir_perspective.z;
        } catch (...) {
            std::cout << "Invalid number!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        return true;
    }
    if (cmd_name == "set-lod-factor") {
        try {
            this->_lod_factor = std::stof(args);
        } catch (...) {
            std::cout << "Invalid number!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        return true;
    }
    if (cmd_name == "load-data") {
        std::stringstream ss(args);
        std::string path;
        double minlon, maxlon, minlat, maxlat;
        try {
            ss >> path;
        } catch (...) {
            std::cout << "Incorrect path format!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        try {
            ss >> minlon;
            ss >> maxlon;
            ss >> minlat;
            ss >> maxlat;
        } catch (...) {
            std::cout << "Invalid number!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        if (ss.fail()) {
            minlat = 0;
            maxlat = 360;
            minlon = 0;
            maxlon = 180;
        }
        try {
            this->_tiles = loadTilesFromDirectory(path, minlon, maxlon, minlat, maxlat);
        } catch (...) {
            std::cout << "Error loading files!" << std::endl;
            std::flush(std::cout);
            return false;
        }
        return true;
    }
    if (cmd_name == "set-sky-mode") {
        if (args == "perlin") {
            this->_skymode = SkyMode::PERLIN;
        } else if (args == "color") {
            this->_skymode = SkyMode::COLOR;
        } else {
            std::cout << "Incorrect mode name! Use \"perlin\" or \"color\"" << std::endl;
            return false;
        }
        return true;
    }
    std::cout << "Invalid command!" << std::endl;
    return false;
}
