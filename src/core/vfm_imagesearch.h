#ifndef INCLUDED_VFM_IMAGESEARCH
#define INCLUDED_VFM_IMAGESEARCH

#include <opencv2/opencv.hpp>

class ImageSearch {
  public:
    // Enums
    enum ReturnCode {
      e_SUCCESS,
      e_NO_MATCH_FOUND,
      e_BAD_FILE
    };

    // Constructor
    ImageSearch();
    ImageSearch(cv::Mat frame, int frame_count, double confidence);

    // Getters
    double result_confidence() const;
    int    result_frame_count() const;

    // Search Functions
    static bool isImageWithinFrame(const cv::Mat& image, const cv::Mat& frame, double& confidence);
    ReturnCode  isImageWithinVideo(const cv::Mat& target_image, cv::VideoCapture& source_video);    

    // I/O Functions
    void       exportResultFrame();
  
  private:
    cv::Mat d_result_frame;
    int     d_result_frame_count;
    double  d_result_confidence;
};

#endif
