#ifndef INCLUDED_VFM_TYPES
#define INCLUDED_VFM_TYPES

#include <string>
#include <vector>

enum class MatchStatus {
  e_SUCCESS,
  e_NO_MATCH_FOUND,
  e_BAD_FILE,
};

struct Match {
    double time_seconds;
    int    frame_index;
    double score;
    bool   has_bbox;
    int    x, y, w, h;
    
    Match() = delete;
    Match(const double time, const int index, const double score,
          const bool has_bbox, const int x, const int y, const int w,
          const int h);
};

struct VideoMetadata {
    std::string path;
    double      fps;
    int         frame_count;
    double      duration_sec;
    int         width;
    int         height;

    VideoMetadata() = delete;
    VideoMetadata(const std::string& path);
    VideoMetadata(const std::string& path, const double fps, const int count,
                  const double dur, const int w, const int h);
};

struct ImageMetadata {
    std::string path;
    int         width;
    int         height;
    int         channels;
    
    ImageMetadata() = delete;
    ImageMetadata(const std::string& path);
    ImageMetadata(const std::string& path, const int width, const int height,
                  const int channels);
};

struct MatchResults {
    MatchStatus              status;
    ImageMetadata            image;
    VideoMetadata            video;
    std::vector<Match>       matches;

    MatchResults() = delete;
    MatchResults(const MatchStatus status,const ImageMetadata& image,
                 const VideoMetadata& video, const std::vector<Match>& matches);
};

#endif
