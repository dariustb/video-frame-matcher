#include <iostream>
#include <algorithm>

#include <vfm_imagesearch.h>
#include <vfm_args.h>

int main(int argc, char** argv) {
    const Args args = parseArgs(argc, argv);

    ImageSearch ImSearch;
    const MatchStatus matchStatus = ImageSearch::searchVideoForImage(args.image_path, args.video_path, args.threshold, ImSearch);

    switch (matchStatus) {
        case MatchStatus::e_SUCCESS: {
            const MatchResults& results = ImSearch.results();

            // Find the best match
            auto best_match = std::max_element(results.matches.begin(), results.matches.end(),
                [](const Match& a, const Match& b) { return a.score < b.score; });

            if (best_match != results.matches.end()) {
                std::cout << "Found " << results.matches.size() << " match(es)" << std::endl;
                std::cout << "Best match at frame # " << best_match->frame_index
                        << " (" << best_match->time_seconds << "s)"
                        << " with confidence of " << best_match->score * 100
                        << "%." << std::endl;
            }
            break;
        }

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
