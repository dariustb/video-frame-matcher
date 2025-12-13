#include <CLI/CLI.hpp>

#include <exception>
#include <iostream>

#include <vfm_args.h>
#include <vfm_imagesearch.h>
#include <vfm_io.h>
#include <vfm_json.h>
#include <vfm_types.h>

int main(int argc, char** argv) {
    Args args;
    try {
        args = parseArgs(argc, argv);
    }
    catch (const CLI::ValidationError& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return 2;
    }
    catch (const std::exception& err) {
        std::cerr << "Error: parseArgs failed: " << err.what() << std::endl;
        return 2;
    }

    const MatchResults results =
        ImageSearch::searchVideoForImage(args.image_path, args.video_path, args.threshold);

    switch (results.status) {
        case MatchStatus::e_SUCCESS:
        case MatchStatus::e_NO_MATCH_FOUND:
            printResults(results);
            break;

        case MatchStatus::e_BAD_FILE:
            std::cerr << "Error: could not open file" << std::endl;
            return 2;

        default:
            std::cerr << "Error: internal error" << std::endl;
            return 4;
    }

    if (!args.output_json.empty()) {
        if (!vfm::writeMatchResultsToFile(results, args.output_json)) {
            std::cerr << "Error: failed to write JSON output to " << args.output_json << std::endl;
            return 3;
        }
        std::cout << "JSON output written to: " << args.output_json << std::endl;
    }

	return 0;
}
