#include <vfm_json.h>
#include <fstream>

namespace vfm {

static const char* matchStatusToString(MatchStatus status) {
    switch (status) {
        case MatchStatus::e_SUCCESS:
            return "SUCCESS";
        case MatchStatus::e_NO_MATCH_FOUND:
            return "NO_MATCH_FOUND";
        case MatchStatus::e_BAD_FILE:
            return "BAD_FILE";
        default:
            return "UNKNOWN";
    }
}

nlohmann::json matchResultsToJson(const MatchResults& results) {
    nlohmann::json j;

    // Add status
    j["status"] = matchStatusToString(results.status);

    // Add video metadata
    j["video"] = {
        {"path", results.video.path},
        {"fps", results.video.fps},
        {"frame_count", results.video.frame_count},
        {"duration_sec", results.video.duration_sec},
        {"width", results.video.width},
        {"height", results.video.height}
    };

    // Add image metadata
    j["image"] = {
        {"path", results.image.path},
        {"width", results.image.width},
        {"height", results.image.height},
        {"channels", results.image.channels}
    };

    // Add matches
    nlohmann::json matches = nlohmann::json::array();
    for (const auto& match : results.matches) {
        nlohmann::json match_obj = {
            {"time_seconds", match.time_seconds},
            {"frame_index", match.frame_index},
            {"score", match.score},
            {"has_bbox", match.has_bbox}
        };

        if (match.has_bbox) {
            match_obj["bbox"] = {
                {"x", match.x},
                {"y", match.y},
                {"w", match.w},
                {"h", match.h}
            };
        }

        matches.push_back(match_obj);
    }
    j["matches"] = matches;

    return j;
}

std::string matchResultsToJsonString(const MatchResults& results, int indent) {
    return matchResultsToJson(results).dump(indent);
}

bool writeMatchResultsToFile(const MatchResults& results,
                              const std::string& filename,
                              int indent) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        nlohmann::json j = matchResultsToJson(results);
        file << j.dump(indent);
        file.close();

        return true;
    } catch (...) {
        return false;
    }
}

}
