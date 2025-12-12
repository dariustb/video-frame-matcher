#include <iostream>

#include <vfm_imagesearch.h>
#include <vfm_args.h>

int main(int argc, char** argv) {
    const Args args = parseArgs(argc, argv);

    ImageSearch ImSearch;
    const MatchStatus matchStatus = ImageSearch::searchVideoForImage(args.image_path, args.video_path, ImSearch);

    switch (matchStatus) {
        case MatchStatus::e_SUCCESS:
            ImSearch.exportResultFrame();
            std::cout << "Selected frame # "  << ImSearch.result_frame_count()
                    << " with confidence of " << ImSearch.result_confidence() * 100
                    << "%." << std::endl;
            break;

        case MatchStatus::e_NO_MATCH_FOUND:
            std::cout << "No match found" << std::endl;
            break;

        case MatchStatus::e_BAD_FILE:
            std::cerr << "Error: could not open file" << std::endl;
            break;

        default:
            std::cerr << "Error: unexpected return value" << std::endl;
            break;

    }

	return 0;
}
