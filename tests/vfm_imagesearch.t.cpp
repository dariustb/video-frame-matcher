#include <gtest/gtest.h>
#include <vfm_imagesearch.h>

namespace {
    const double CONFIDENCE_THRESHOLD = 0.9;
    const double CONFIDENCE_NONE      = 0;

    const int DEFAULT_IMAGE_HEIGHT = 100; // 100px
    const int DEFAULT_IMAGE_WIDTH  = 100; // 100px

    const cv::Scalar RGB_BLUE = cv::Scalar(255, 0, 0);
    const cv::Scalar RGB_RED  = cv::Scalar(0, 0, 255);
}

// Helper Functions
cv::Mat createTestImage(
    const int height = DEFAULT_IMAGE_HEIGHT, 
    const int width  = DEFAULT_IMAGE_WIDTH, 
    const cv::Scalar& color = RGB_BLUE)
{
    cv::Mat test_image(height, width, CV_8UC3);
    test_image.setTo(color);
    return test_image;
}

cv::Mat createTestImage(const cv::Scalar& color)
{
    return createTestImage(DEFAULT_IMAGE_HEIGHT, DEFAULT_IMAGE_WIDTH, color);
}

TEST(VFMImageSearch, ConstructorDefaultValuesAreValid) {
    // Given
    ImageSearch Test;

    // When 
    const int    default_frame_count = Test.result_frame_count();
    const double default_confidence  = Test.result_confidence();

    // Then
    EXPECT_EQ(0, default_frame_count);
    EXPECT_EQ(0, default_confidence);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueForCroppedImages)
{
    // Given
    const cv::Mat t_frame = createTestImage();
    const cv::Mat t_image = createTestImage(50, 50);

    ASSERT_FALSE(t_frame.empty());
    ASSERT_FALSE(t_image.empty());

    // When
    double t_confidence = CONFIDENCE_NONE;
    const bool t_result = ImageSearch::isImageWithinFrame(t_image, t_frame, t_confidence);

    // Then
    EXPECT_TRUE(t_result);
    EXPECT_GT(t_confidence, CONFIDENCE_THRESHOLD);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueForRelatedImages)
{
    // Given
    const cv::Mat t_frame = createTestImage();    
    const cv::Mat t_image = createTestImage();
    
    ASSERT_FALSE(t_frame.empty());
    ASSERT_FALSE(t_image.empty());

    // When
    double t_confidence = CONFIDENCE_NONE;
    const bool t_result = ImageSearch::isImageWithinFrame(t_image, t_frame, t_confidence);

    // Then
    EXPECT_TRUE(t_result);
    EXPECT_GT(t_confidence, CONFIDENCE_THRESHOLD);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsFalseForUnrelatedImages)
{
    // Given - Images with different patterns, not just solid colors
    // Frame with horizontal stripes
    cv::Mat t_frame = createTestImage();
    for (int i = 0; i < t_frame.rows; i += 10) {
        cv::rectangle(t_frame, cv::Point(0, i), cv::Point(t_frame.cols, i + 5), RGB_RED, -1);
    }

    // Image with vertical stripes
    cv::Mat t_image = createTestImage(RGB_RED);
    for (int i = 0; i < t_image.cols; i += 10) {
        cv::rectangle(t_image, cv::Point(i, 0), cv::Point(i + 5, t_image.rows), RGB_BLUE, -1);
    }

    ASSERT_FALSE(t_frame.empty());
    ASSERT_FALSE(t_image.empty());

    // When
    double t_confidence = CONFIDENCE_NONE;
    const bool t_result = ImageSearch::isImageWithinFrame(t_image, t_frame, t_confidence);

    // Then
    EXPECT_FALSE(t_result);
    EXPECT_LT(t_confidence, CONFIDENCE_THRESHOLD);
}
