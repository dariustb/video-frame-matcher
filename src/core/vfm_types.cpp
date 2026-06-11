#include <vfm_types.h>

Match::Match(const double time, const int index, const double score,
    const bool has_bbox, const int x, const int y, const int w,
    const int h)
: time_seconds(time)
, frame_index(index)
, score(score)
, has_bbox(has_bbox)
, x(x)
, y(y)
, w(w)
, h(h)
{}

VideoMetadata::VideoMetadata(const std::string& path) : VideoMetadata(path, 0.0, 0, 0.0, 0, 0) {}

VideoMetadata::VideoMetadata(const std::string& path, const double fps, const int count,
    const double dur, const int w, const int h)
: path(path)
, fps(fps)
, frame_count(count)
, duration_sec(dur)
, width(w)
, height(h)
{}

ImageMetadata::ImageMetadata(const std::string& path) : ImageMetadata(path, 0, 0, 0) {}

ImageMetadata::ImageMetadata(const std::string& path, const int width, const int height,
    const int channels)
: path (path)
, width(width)
, height(height)
, channels(channels)
{}
