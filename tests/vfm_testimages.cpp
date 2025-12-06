#include "vfm_testimages.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <cstdio>

namespace vfm_test {

// Constants
const double CONFIDENCE_THRESHOLD = 0.9;
const double CONFIDENCE_NONE      = 0;

const int DEFAULT_IMAGE_HEIGHT = 100; // 100px
const int DEFAULT_IMAGE_WIDTH  = 100; // 100px

const cv::Scalar RGB_BLUE   = cv::Scalar(255, 0, 0);
const cv::Scalar RGB_RED    = cv::Scalar(0, 0, 255);
const cv::Scalar RGB_GREEN  = cv::Scalar(0, 255, 0);
const cv::Scalar RGB_YELLOW = cv::Scalar(0, 255, 255);

// Create a test image with specified dimensions and color
cv::Mat createTestImage(
    const int height,
    const int width,
    const cv::Scalar& color)
{
    cv::Mat test_image(height, width, CV_8UC3);
    test_image.setTo(color);
    return test_image;
}

// Create a test image with specified dimensions and default color
cv::Mat createTestImage(
    const int height,
    const int width)
{
    return createTestImage(height, width, RGB_BLUE);
}

// Create a test image with default dimensions and specified color
cv::Mat createTestImage(const cv::Scalar& color)
{
    return createTestImage(DEFAULT_IMAGE_HEIGHT, DEFAULT_IMAGE_WIDTH, color);
}

// Create a test image with default dimensions and default color
cv::Mat createTestImage()
{
    return createTestImage(DEFAULT_IMAGE_HEIGHT, DEFAULT_IMAGE_WIDTH, RGB_BLUE);
}

// Create a test video with specified frames
std::string createTestVideo(const std::vector<cv::Mat>& frames, const std::string& filename)
{
    if (frames.empty()) {
        return "";
    }

    int frame_width = frames[0].cols;
    int frame_height = frames[0].rows;

    cv::VideoWriter writer(filename, cv::VideoWriter::fourcc('m','p','4','v'), 30,
                          cv::Size(frame_width, frame_height));

    if (!writer.isOpened()) {
        return "";
    }

    for (const auto& frame : frames) {
        writer.write(frame);
    }

    writer.release();
    return filename;
}

// Clean up test video file
void cleanupTestVideo(const std::string& filename)
{
    if (!filename.empty()) {
        std::remove(filename.c_str());
    }
}

// Pattern generation functions

// Add horizontal stripes to an image
void addHorizontalStripes(
    cv::Mat& image,
    const cv::Scalar& stripe_color,
    int stripe_height,
    int stripe_spacing)
{
    for (int i = 0; i < image.rows; i += stripe_spacing) {
        cv::rectangle(image, cv::Point(0, i), cv::Point(image.cols, i + stripe_height), stripe_color, -1);
    }
}

// Add vertical stripes to an image
void addVerticalStripes(
    cv::Mat& image,
    const cv::Scalar& stripe_color,
    int stripe_width,
    int stripe_spacing)
{
    for (int i = 0; i < image.cols; i += stripe_spacing) {
        cv::rectangle(image, cv::Point(i, 0), cv::Point(i + stripe_width, image.rows), stripe_color, -1);
    }
}

// Add checkerboard pattern to an image
void addCheckerboard(
    cv::Mat& image,
    const cv::Scalar& alternate_color,
    int square_size)
{
    for (int i = 0; i < image.rows; i += square_size) {
        for (int j = 0; j < image.cols; j += square_size) {
            if ((i/square_size + j/square_size) % 2 == 0) {
                cv::rectangle(image, cv::Point(j, i), cv::Point(j + square_size, i + square_size), alternate_color, -1);
            }
        }
    }
}

// Add a circle to an image
void addCircle(
    cv::Mat& image,
    const cv::Point& center,
    int radius,
    const cv::Scalar& color)
{
    cv::circle(image, center, radius, color, -1);
}

// Add a rectangle to an image
void addRectangle(
    cv::Mat& image,
    const cv::Point& top_left,
    const cv::Point& bottom_right,
    const cv::Scalar& color)
{
    cv::rectangle(image, top_left, bottom_right, color, -1);
}

// Add horizontal lines to an image
void addHorizontalLines(
    cv::Mat& image,
    const cv::Scalar& line_color,
    int line_spacing,
    int line_thickness)
{
    for (int i = 0; i < image.rows; i += line_spacing) {
        cv::line(image, cv::Point(0, i), cv::Point(image.cols, i), line_color, line_thickness);
    }
}

// Create an image with horizontal stripes
cv::Mat createImageWithHorizontalStripes(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Scalar& stripe_color,
    int stripe_height,
    int stripe_spacing)
{
    cv::Mat image = createTestImage(height, width, base_color);
    addHorizontalStripes(image, stripe_color, stripe_height, stripe_spacing);
    return image;
}

// Create an image with vertical stripes
cv::Mat createImageWithVerticalStripes(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Scalar& stripe_color,
    int stripe_width,
    int stripe_spacing)
{
    cv::Mat image = createTestImage(height, width, base_color);
    addVerticalStripes(image, stripe_color, stripe_width, stripe_spacing);
    return image;
}

// Create an image with checkerboard pattern
cv::Mat createImageWithCheckerboard(
    int height,
    int width,
    const cv::Scalar& color1,
    const cv::Scalar& color2,
    int square_size)
{
    cv::Mat image = createTestImage(height, width, color1);
    addCheckerboard(image, color2, square_size);
    return image;
}

// Create an image with a circle
cv::Mat createImageWithCircle(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Point& center,
    int radius,
    const cv::Scalar& circle_color)
{
    cv::Mat image = createTestImage(height, width, base_color);
    addCircle(image, center, radius, circle_color);
    return image;
}

// Create an image with a rectangle
cv::Mat createImageWithRectangle(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Point& top_left,
    const cv::Point& bottom_right,
    const cv::Scalar& rect_color)
{
    cv::Mat image = createTestImage(height, width, base_color);
    addRectangle(image, top_left, bottom_right, rect_color);
    return image;
}

// Create an image with horizontal lines
cv::Mat createImageWithHorizontalLines(
    int height,
    int width,
    const cv::Scalar& base_color,
    const cv::Scalar& line_color,
    int line_spacing,
    int line_thickness)
{
    cv::Mat image = createTestImage(height, width, base_color);
    addHorizontalLines(image, line_color, line_spacing, line_thickness);
    return image;
}

} // namespace vfm_test
