#ifndef INCLUDED_VFM_TYPES
#define INCLUDED_VFM_TYPES

#include <string>
#include <vector>

enum class MatchStatus {
  e_SUCCESS,
  e_NO_MATCH_FOUND,
  e_BAD_FILE,
  e_DECODE_ERROR,
  e_INTERNAL_ERROR
};

struct Match {
    double time_seconds;
    int    frame_index;
    double score;
    bool   has_bbox;
    int    x, y, w, h;
};

struct VideoMetadata {
    std::string path;
    double      fps;
    int         frame_count;
    double      duration_sec;
    int         width;
    int         height;
};

struct ImageMetadata {
    std::string path;
    int         width;
    int         height;
    int         channels;
};

struct MatchResults {
    MatchStatus              status;
    VideoMetadata            video;
    ImageMetadata            image;
    std::vector<Match>       matches;
};

#endif
