#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <fstream>

#include <vfm_imagesearch.h>
#include <vfm_testimages.h>

using namespace vfm_test;

TEST(VFMImageSearch, ConstructorDefaultValuesAreValid) {
    // Given
    ImageSearch test;

    // When
    const int    default_frame_count = test.result_frame_count();
    const double default_confidence  = test.result_confidence();

    // Then
    EXPECT_EQ(0, default_frame_count);
    EXPECT_EQ(0, default_confidence);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueForCroppedImages)
{
    // Given
    const cv::Mat frame = createTestImage();
    const cv::Mat image = createTestImage(50, 50);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    double confidence = CONFIDENCE_NONE;
    const bool result = ImageSearch::isImageWithinFrame(image, frame, confidence);

    // Then
    EXPECT_TRUE(result);
    EXPECT_GT(confidence, CONFIDENCE_THRESHOLD);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueForIdenticalImages)
{
    // Given
    const cv::Mat frame = createTestImage();
    const cv::Mat image = createTestImage();

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    double confidence = CONFIDENCE_NONE;
    const bool result = ImageSearch::isImageWithinFrame(image, frame, confidence);

    // Then
    EXPECT_TRUE(result);
    EXPECT_GT(confidence, CONFIDENCE_THRESHOLD);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsFalseForUnrelatedImages)
{
    // Given images with different patterns
    cv::Mat frame = createImageWithHorizontalStripes(
        DEFAULT_IMAGE_HEIGHT, DEFAULT_IMAGE_WIDTH, RGB_BLUE, RGB_RED);
    cv::Mat image = createImageWithVerticalStripes(
        DEFAULT_IMAGE_HEIGHT, DEFAULT_IMAGE_WIDTH, RGB_RED, RGB_BLUE);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    double confidence = CONFIDENCE_NONE;
    const bool result = ImageSearch::isImageWithinFrame(image, frame, confidence);

    // Then
    EXPECT_FALSE(result);
    EXPECT_LT(confidence, CONFIDENCE_THRESHOLD);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueWhenUsingSmallImage)
{
    // Given very small image within larger frame
    const cv::Mat frame = createTestImage(200, 200);
    const cv::Mat image = createTestImage(10, 10);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    double confidence = CONFIDENCE_NONE;
    const bool result = ImageSearch::isImageWithinFrame(image, frame, confidence);

    // Then
    EXPECT_TRUE(result);
    EXPECT_GT(confidence, CONFIDENCE_THRESHOLD);
}

TEST(VFMImageSearch, isImageWithinFrameUpdatesConfidenceParameter)
{
    // Given
    const cv::Mat frame = createTestImage();
    const cv::Mat image = createTestImage(50, 50);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    double confidence = CONFIDENCE_NONE;
    ImageSearch::isImageWithinFrame(image, frame, confidence);

    // Then
    EXPECT_NE(confidence, CONFIDENCE_NONE);
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueWhenPartialMatch)
{
    // Given
    cv::Mat frame = createImageWithRectangle(
        200, 200, RGB_BLUE, cv::Point(0, 0), cv::Point(50, 50), RGB_RED);
    cv::Mat image = createTestImage(50, 50, RGB_RED);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    double confidence = CONFIDENCE_NONE;
    const bool result = ImageSearch::isImageWithinFrame(image, frame, confidence);

    // Then
    EXPECT_TRUE(result);
    EXPECT_GT(confidence, CONFIDENCE_THRESHOLD);
}

TEST(VFMImageSearch, isImageWithinVideoReturnsSuccessWhenImageFound)
{
    // Given
    cv::Mat target_image = createTestImage(50, 50, RGB_GREEN);

    std::vector<cv::Mat> frames;
    frames.push_back(createTestImage(100, 100, RGB_BLUE));   // Frame 1: no match
    frames.push_back(createTestImage(100, 100, RGB_RED));    // Frame 2: no match

    // Embed target image into one of the frames
    cv::Mat frame_with_target = createTestImage(100, 100, RGB_BLUE);
    target_image.copyTo(frame_with_target(cv::Rect(0, 0, 50, 50)));
    frames.push_back(frame_with_target);

    std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    ImageSearch test;
    ImageSearch::ReturnCode result = test.isImageWithinVideo(target_image, video);
    video.release();

    // Then
    EXPECT_EQ(ImageSearch::e_SUCCESS, result);
    EXPECT_GT(test.result_confidence(), CONFIDENCE_THRESHOLD);
    EXPECT_GT(test.result_frame_count(), 0);

    // Cleanup
    cleanupTestVideo(video_path);
}

TEST(VFMImageSearch, isImageWithinVideoReturnsNoMatchWhenImageNotFound)
{
    // Given
    cv::Mat target_image = createImageWithCheckerboard(50, 50, RGB_GREEN, RGB_YELLOW);

    std::vector<cv::Mat> frames;
    cv::Mat frame1 = createImageWithHorizontalStripes(100, 100, RGB_BLUE, RGB_RED, 10, 20);
    frames.push_back(frame1);

    // Frame 2 Vertical stripes
    cv::Mat frame2 = createImageWithVerticalStripes(100, 100, RGB_RED, RGB_BLUE, 10, 20);
    frames.push_back(frame2);

    // Frame 3 Solid yellow
    frames.push_back(createTestImage(100, 100, RGB_YELLOW));

    std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    ImageSearch test;
    ImageSearch::ReturnCode result = test.isImageWithinVideo(target_image, video);
    video.release();

    // Then
    EXPECT_EQ(ImageSearch::e_NO_MATCH_FOUND, result);

    // Cleanup
    cleanupTestVideo(video_path);
}

TEST(VFMImageSearch, isImageWithinVideoReturnsBadFileForInvalidVideo)
{
    // Given
    cv::VideoCapture video("nonexistent_video.mp4");
    cv::Mat target_image = createTestImage(50, 50);

    // When
    ImageSearch test;
    ImageSearch::ReturnCode result = test.isImageWithinVideo(target_image, video);

    // Then
    EXPECT_EQ(ImageSearch::e_BAD_FILE, result);
}

TEST(VFMImageSearch, isImageWithinVideoTracksHighestConfidenceFrame)
{
    // Given
    cv::Mat target_image = createTestImage(50, 50, RGB_GREEN);

    std::vector<cv::Mat> frames;

    // Frame 1: partial match (target in corner)
    cv::Mat frame1 = createTestImage(100, 100, RGB_BLUE);
    target_image.copyTo(frame1(cv::Rect(0, 0, 50, 50)));
    frames.push_back(frame1);

    // Frame 2: better match (exact match, full frame)
    cv::Mat frame2 = createTestImage(100, 100, RGB_BLUE);
    target_image.copyTo(frame2(cv::Rect(25, 25, 50, 50))); // Centered for potentially better match
    frames.push_back(frame2);

    std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    ImageSearch test;
    ImageSearch::ReturnCode result = test.isImageWithinVideo(target_image, video);
    video.release();

    // Then
    EXPECT_EQ(ImageSearch::e_SUCCESS, result);
    EXPECT_GT(test.result_confidence(), CONFIDENCE_THRESHOLD);
    EXPECT_GE(test.result_frame_count(), 1);
    EXPECT_LE(test.result_frame_count(), 2);

    // Cleanup
    cleanupTestVideo(video_path);
}

TEST(VFMImageSearch, isImageWithinVideoStoresCorrectFrameCount)
{
    // Given
    cv::Mat target_image = createImageWithCircle(
        50, 50, RGB_GREEN, cv::Point(25, 25), 20, RGB_YELLOW);

    std::vector<cv::Mat> frames;

    cv::Mat frame1 = createImageWithRectangle(
        100, 100, RGB_BLUE, cv::Point(10, 10), cv::Point(40, 40), RGB_RED);
    frames.push_back(frame1);

    cv::Mat frame2 = createImageWithCircle(
            100, 100, RGB_RED, cv::Point(50, 50), 30, RGB_BLUE);
    frames.push_back(frame2);

    cv::Mat frame3 = createImageWithHorizontalLines(
        100, 100, RGB_YELLOW, RGB_RED);
    frames.push_back(frame3);


    cv::Mat frame_with_target = createTestImage(100, 100, RGB_BLUE);
    target_image.copyTo(frame_with_target(cv::Rect(10, 10, 50, 50)));
    frames.push_back(frame_with_target);

    std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    ImageSearch test;
    ImageSearch::ReturnCode result = test.isImageWithinVideo(target_image, video);
    video.release();

    // Then
    EXPECT_EQ(ImageSearch::e_SUCCESS, result);
    EXPECT_EQ(4, test.result_frame_count());

    // Cleanup
    cleanupTestVideo(video_path);
}

TEST(VFMImageSearch, isImageWithinVideoWithSingleFrameVideo)
{
    // Given
    cv::Mat target_image = createTestImage(50, 50, RGB_GREEN);

    std::vector<cv::Mat> frames;
    cv::Mat frame = createTestImage(100, 100, RGB_BLUE);
    target_image.copyTo(frame(cv::Rect(0, 0, 50, 50)));
    frames.push_back(frame);

    std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    ImageSearch test;
    ImageSearch::ReturnCode result = test.isImageWithinVideo(target_image, video);
    video.release();

    // Then
    EXPECT_EQ(ImageSearch::e_SUCCESS, result);
    EXPECT_EQ(1, test.result_frame_count());

    // Cleanup
    cleanupTestVideo(video_path);
}

TEST(VFMImageSearch, exportResultFrameCreatesFile)
{
    // Given - ImageSearch with a result frame
    cv::Mat test_frame = createTestImage(100, 100, RGB_BLUE);
    ImageSearch test(test_frame, 1, 0.95);

    // Create output directory if it doesn't exist
    system("mkdir -p output");

    // When
    test.exportResultFrame();

    // Then - Check if file was created
    std::ifstream file("output/result.jpg");
    EXPECT_TRUE(file.good());
    file.close();

    // Cleanup
    std::remove("output/result.jpg");
}

TEST(VFMImageSearch, exportResultFrameWithEmptyFrame)
{
    // Given
    cv::Mat empty_frame;
    ImageSearch test(empty_frame, 0, 0.0);

    // Create output directory if it doesn't exist
    system("mkdir -p output");

    // When - OpenCV will throw exception for empty frame
    // This is expected behavior, so we test that it throws
    EXPECT_THROW(test.exportResultFrame(), cv::Exception);

    // Cleanup (file may or may not exist)
    std::remove("output/result.jpg");
}
