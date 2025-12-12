#include <algorithm>
#include <iostream>

#include <vfm_io.h>
#include <vfm_types.h>

void printResults(const MatchResults &results)
{
    if (results.status == MatchStatus::e_NO_MATCH_FOUND) {
        std::cout << "No match found" << std::endl;
        return;
    }

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
}
