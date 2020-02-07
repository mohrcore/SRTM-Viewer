#include "coordresolver.h"

using namespace coordresolver;

enum class __CoordResolvState {
    EXPECTING_WORLD_DIR,
    EXPECTING_ANGLE
};

//Why are there no regexes in STLF wtf?!?!?!
std::pair<double, double> coordresolver::resolveCoordinatesFromFilename(std::string filename) {
    size_t fname_beg = filename.find_last_of(R"(/\)");
    if (fname_beg != std::string::npos) {
        filename = filename.substr(fname_beg + 1, filename.length() - fname_beg - 1);
    }
    std::string::iterator it = filename.begin();
    bool resv = false;
    __CoordResolvState state = __CoordResolvState::EXPECTING_WORLD_DIR;
    char world_dir = '\0';
    std::pair<int, int> angles(-1.0, -1.0);
    int cangle = 0;
    do {
        if (it == filename.end())
            throw CoordResolveError::UNEXPECTED_END;
        switch (state) {
        case __CoordResolvState::EXPECTING_WORLD_DIR:
            if (*it != 'N' && *it != 'S' && *it != 'W' && *it != 'E' && *it != 'n' && *it != 's' && *it != 'w' && *it != 'e')
                throw CoordResolveError::UNEXPECTED_TOKEN;
            world_dir = *it;
            it++;
            state = __CoordResolvState::EXPECTING_ANGLE;
        break;
        //Parsing state machine, boring stuff
        case __CoordResolvState::EXPECTING_ANGLE:
            if (*it < '0' || *it > '9')
                throw CoordResolveError::ANGLE_UNSPECIFIED;
            cangle = 0.0;
            while (*it >= '0' && *it <= '9') {
                cangle = cangle * 10.0 + double(*it - '0');
                it++;
            }
            if (*it == '.' || *it == ',') {
                it++;
                double frac = 0.1;
                while (*it >= '0' && *it <= '9') {
                    cangle += frac * double(*it - '0');
                    frac /= 10.0;
                    it++;
                }
            }
            if (world_dir == 'N' || world_dir == 'n') {
                if (cangle > 90.0)
                    throw CoordResolveError::ANGLE_OUT_OF_RANGE;
                angles.first = 90.0- cangle;
            } else if (world_dir == 'S' || world_dir == 's') {
                if (cangle > 90.0)
                    throw CoordResolveError::ANGLE_OUT_OF_RANGE;
                angles.first = 90.0 + cangle;
            } else if (world_dir == 'W' || world_dir == 'w') {
                if (cangle >= 360.0)
                    throw CoordResolveError::ANGLE_OUT_OF_RANGE;
                angles.second = 360.0 - cangle;
            } else if (world_dir == 'E' || world_dir == 'e') {
                if (cangle >= 360.0)
                    throw CoordResolveError::ANGLE_OUT_OF_RANGE;
                angles.second = cangle;
            }
            state = __CoordResolvState::EXPECTING_WORLD_DIR;
        break;
        }
    } while (angles.first == -1.0 || angles.second == -1.0);
    return angles;
}