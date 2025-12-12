#ifndef INCLUDED_VFM_IMAGESEARCH
#define INCLUDED_VFM_IMAGESEARCH

#include <opencv2/opencv.hpp>
#include <vfm_types.h>

class ImageSearch {
  public:
    // Search Functions
    static void isImageWithinFrame(const cv::Mat& image, const cv::Mat& frame, int frame_index, double fps, double threshold, std::vector<Match>& matches);
    static MatchStatus isImageWithinVideo(const cv::Mat& target_image, cv::VideoCapture& source_video, double threshold, std::vector<Match>& matches);
    static MatchResults searchVideoForImage(const std::string& image_path, const std::string& video_path, double threshold);

    // Metadata Functions
    static ImageMetadata getImageMetadata(const std::string& image_path);
    static VideoMetadata getVideoMetadata(const std::string& video_path);
};

#endif
