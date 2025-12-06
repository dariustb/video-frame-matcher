#include <vfm_args.h>
#include <CLI/CLI.hpp>
#include <string>

namespace {
    const std::string APP_DESCRIPTION = "C++ command-line tool for detecting occurrences of a reference image inside video frames, with timestamped match output, configurable thresholds, and optional JSON/visual exports";
}

Args parseArgs(int argc, char** argv) {
    Args args;
    CLI::App app{APP_DESCRIPTION};

    app.add_option("--video", args.video_path, "Video path")->required();
    app.add_option("--image", args.image_path, "Reference image path")->required();
    app.add_option("--threshold", args.threshold, "Confidence threshold")->check(CLI::Range(0.0,1.0));
    app.add_option("--frame-step", args.frame_step, "Frame steps")->check(CLI::PositiveNumber);
    app.add_option("--start-time", args.start_time, "Video search start time")->check(CLI::NonNegativeNumber);
    app.add_option("--end-time", args.end_time, "Video search end time");
    app.add_option("--output-json", args.output_json, "Output JSON file export location");

    app.parse(argc, argv);

    if (args.end_time && *args.end_time < args.start_time)
        throw CLI::ValidationError("Video end time cannot be earlier than the start time");

    return args;
}
