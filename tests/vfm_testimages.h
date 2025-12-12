#ifndef INCLUDED_VFM_TEST_IMAGES
#define INCLUDED_VFM_TEST_IMAGES

#include <opencv2/core.hpp>
#include <string>
#include <vector>

namespace vfm_test {

// Constants
extern const double CONFIDENCE_THRESHOLD;
extern const double CONFIDENCE_NONE;

extern const int DEFAULT_IMAGE_HEIGHT;
extern const int DEFAULT_IMAGE_WIDTH;

extern const cv::Scalar RGB_BLUE;
extern const cv::Scalar RGB_RED;
extern const cv::Scalar RGB_GREEN;
extern const cv::Scalar RGB_YELLOW;

// Create a test image with specified dimensions and color
cv::Mat createTestImage(
    const int height,
    const int width,
    const cv::Scalar& color);

// Create a test image with specified dimensions and default color
cv::Mat createTestImage(
    const int height,
    const int width);

// Create a test image with default dimensions and specified color
cv::Mat createTestImage(const cv::Scalar& color);

// Create a test image with default dimensions and default color
cv::Mat createTestImage();

// Create a test video with specified frames
std::string createTestVideo(
    const std::vector<cv::Mat>& frames,
    const std::string& filename = "test_video.mp4");

// Clean up test video file
void cleanupTestVideo(const std::string& filename);

// Pattern generation functions

// Add horizontal stripes to an image
void addHorizontalStripes(
    cv::Mat& image,
    const cv::Scalar& stripe_color,
    int stripe_height = 5,
    int stripe_spacing = 10);

// Add vertical stripes to an image
void addVerticalStripes(
    cv::Mat& image,
    const cv::Scalar& stripe_color,
    int stripe_width = 5,
    int stripe_spacing = 10);

// Add checkerboard pattern to an image
void addCheckerboard(
    cv::Mat& image,
    const cv::Scalar& alternate_color,
    int square_size = 10);

// Add a circle to an image
void addCircle(
    cv::Mat& image,
    const cv::Point& center,
    int radius,
    const cv::Scalar& color);

// Add a rectangle to an image
void addRectangle(
    cv::Mat& image,
    const cv::Point& top_left,
    const cv::Point& bottom_right,
    const cv::Scalar& color);

// Add horizontal lines to an image
void addHorizontalLines(
    cv::Mat& image,
    const cv::Scalar& line_color,
    int line_spacing = 10,
    int line_thickness = 2);

// Create an image with horizontal stripes
cv::Mat createImageWithHorizontalStripes(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Scalar& stripe_color,
    int stripe_height = 5,
    int stripe_spacing = 10);

// Create an image with vertical stripes
cv::Mat createImageWithVerticalStripes(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Scalar& stripe_color,
    int stripe_width = 5,
    int stripe_spacing = 10);

// Create an image with checkerboard pattern
cv::Mat createImageWithCheckerboard(
    int height,
    int width,
    const cv::Scalar& color1,
    const cv::Scalar& color2,
    int square_size = 10);

// Create an image with a circle
cv::Mat createImageWithCircle(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Point& center,
    int radius,
    const cv::Scalar& circle_color);

// Create an image with a rectangle
cv::Mat createImageWithRectangle(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Point& top_left,
    const cv::Point& bottom_right,
    const cv::Scalar& rect_color);

// Create an image with horizontal lines
cv::Mat createImageWithHorizontalLines(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Scalar& line_color,
    int line_spacing = 10,
    int line_thickness = 2);

} // namespace vfm_test

#endif
