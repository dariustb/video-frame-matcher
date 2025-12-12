#include <iostream>

#include <vfm_imagesearch.h>
#include <vfm_args.h>
#include <vfm_io.h>

int main(int argc, char** argv) {
    const Args args = parseArgs(argc, argv);

    const MatchResults results = ImageSearch::searchVideoForImage(args.image_path, args.video_path, args.threshold);

    switch (results.status) {
        case MatchStatus::e_SUCCESS:
        case MatchStatus::e_NO_MATCH_FOUND:
            break;

        case MatchStatus::e_BAD_FILE:
            std::cerr << "Error: could not open file" << std::endl;
            break;

        default:
            std::cerr << "Error: unexpected return value" << std::endl;
            break;
    
    }

    printResults(results);

	return 0;
}
