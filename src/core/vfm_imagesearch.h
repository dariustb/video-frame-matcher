#ifndef INCLUDED_VFM_IMAGESEARCH
#define INCLUDED_VFM_IMAGESEARCH

#include <opencv2/opencv.hpp>
#include <vfm_types.h>

class ImageSearch {
  public:
    // Constructor
    ImageSearch();
    ImageSearch(cv::Mat frame, int frame_count, double confidence);

    // Getters
    double result_confidence() const;
    int    result_frame_count() const;

    // Search Functions
    static bool isImageWithinFrame(const cv::Mat& image, const cv::Mat& frame, double& confidence);
    MatchStatus isImageWithinVideo(const cv::Mat& target_image, cv::VideoCapture& source_video);
    static MatchStatus searchVideoForImage(const std::string& image_path, const std::string& video_path, ImageSearch& result);    

    // I/O Functions
    void       exportResultFrame();
  
  private:
    cv::Mat d_result_frame;
    int     d_result_frame_count;
    double  d_result_confidence;
};

#endif
