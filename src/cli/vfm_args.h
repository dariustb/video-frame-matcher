#ifndef INCLUDED_VFM_ARGS
#define INCLUDED_VFM_ARGS

#include <string>
#include <optional>

namespace {
    const double DEFAULT_THRESHOLD  = 0.85;
    const int    DEFAULT_FRAME_STEP = 2;
    const double DEFAULT_START_TIME = 0.0;
}

struct Args {
    std::string video_path;
    std::string image_path;
    double threshold  = DEFAULT_THRESHOLD;
    int    frame_step = DEFAULT_FRAME_STEP;
    double start_time = DEFAULT_START_TIME;
    std::optional<double> end_time;
    std::string output_json;
};

Args parseArgs(int argc, char** argv);

#endif
