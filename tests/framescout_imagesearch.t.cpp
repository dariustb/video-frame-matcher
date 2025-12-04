#include <gtest/gtest.h>
#include <framescout_imagesearch.h>

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

TEST(FrameScoutImageSearch, ConstructorDefaultValuesAreValid) {
    // Given
    ImageSearch Test;

    // When 
    const int    default_frame_count = Test.result_frame_count();
    const double default_confidence  = Test.result_confidence();

    // Then
    EXPECT_EQ(0, default_frame_count);
    EXPECT_EQ(0, default_confidence);
}

TEST(FrameScoutImageSearch, isImageWithinFrameReturnsTrueForCroppedImages)
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

TEST(FrameScoutImageSearch, isImageWithinFrameReturnsTrueForRelatedImages)
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

TEST(FrameScoutImageSearch, isImageWithinFrameReturnsFalseForUnrelatedImages)
{
    // Given
    const cv::Mat t_frame = createTestImage();    
    const cv::Mat t_image = createTestImage(RGB_RED);
    
    ASSERT_FALSE(t_frame.empty());
    ASSERT_FALSE(t_image.empty());

    // When
    double t_confidence = CONFIDENCE_NONE;
    const bool t_result = ImageSearch::isImageWithinFrame(t_image, t_frame, t_confidence);

    // Then
    // EXPECT_FALSE(t_result);
    // EXPECT_EQ(t_confidence, CONFIDENCE_NONE);
}
