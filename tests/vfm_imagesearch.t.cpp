#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

#include <vfm_imagesearch.h>
#include <vfm_testimages.h>

using namespace vfm_test;

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueForCroppedImages)
{
    // Given
    const cv::Mat frame = createTestImage();
    const cv::Mat image = createTestImage(50, 50);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    std::vector<Match> matches;
    ImageSearch::isImageWithinFrame(image, frame, 1, 30.0, CONFIDENCE_THRESHOLD, matches);

    // Then
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        EXPECT_GT(matches[0].score, CONFIDENCE_THRESHOLD);
    }
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueForIdenticalImages)
{
    // Given
    const cv::Mat frame = createTestImage();
    const cv::Mat image = createTestImage();

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    std::vector<Match> matches;
    ImageSearch::isImageWithinFrame(image, frame, 1, 30.0, CONFIDENCE_THRESHOLD, matches);

    // Then
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        EXPECT_GT(matches[0].score, CONFIDENCE_THRESHOLD);
    }
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
    std::vector<Match> matches;
    ImageSearch::isImageWithinFrame(image, frame, 1, 30.0, CONFIDENCE_THRESHOLD, matches);

    // Then
    EXPECT_TRUE(matches.empty());
}

TEST(VFMImageSearch, isImageWithinFrameReturnsTrueWhenUsingSmallImage)
{
    // Given very small image within larger frame
    const cv::Mat frame = createTestImage(200, 200);
    const cv::Mat image = createTestImage(10, 10);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    std::vector<Match> matches;
    ImageSearch::isImageWithinFrame(image, frame, 1, 30.0, CONFIDENCE_THRESHOLD, matches);

    // Then
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        EXPECT_GT(matches[0].score, CONFIDENCE_THRESHOLD);
    }
}

TEST(VFMImageSearch, isImageWithinFrameUpdatesConfidenceParameter)
{
    // Given
    const cv::Mat frame = createTestImage();
    const cv::Mat image = createTestImage(50, 50);

    ASSERT_FALSE(frame.empty());
    ASSERT_FALSE(image.empty());

    // When
    std::vector<Match> matches;
    ImageSearch::isImageWithinFrame(image, frame, 1, 30.0, CONFIDENCE_THRESHOLD, matches);

    // Then
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        EXPECT_NE(matches[0].score, CONFIDENCE_NONE);
    }
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
    std::vector<Match> matches;
    ImageSearch::isImageWithinFrame(image, frame, 1, 30.0, CONFIDENCE_THRESHOLD, matches);

    // Then
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        EXPECT_GT(matches[0].score, CONFIDENCE_THRESHOLD);
    }
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

    const std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    std::vector<Match> matches;
    const MatchStatus result = ImageSearch::isImageWithinVideo(target_image, video, CONFIDENCE_THRESHOLD, matches);
    video.release();

    // Then
    EXPECT_EQ(MatchStatus::e_SUCCESS, result);
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        EXPECT_GT(matches[0].score, CONFIDENCE_THRESHOLD);
        EXPECT_GT(matches[0].frame_index, 0);
    }

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

    const std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    std::vector<Match> matches;
    const MatchStatus result = ImageSearch::isImageWithinVideo(target_image, video, CONFIDENCE_THRESHOLD, matches);
    video.release();

    // Then
    EXPECT_EQ(MatchStatus::e_NO_MATCH_FOUND, result);

    // Cleanup
    cleanupTestVideo(video_path);
}

TEST(VFMImageSearch, isImageWithinVideoReturnsBadFileForInvalidVideo)
{
    // Given
    cv::VideoCapture video("nonexistent_video.mp4");
    cv::Mat target_image = createTestImage(50, 50);

    // When
    std::vector<Match> matches;
    const MatchStatus result = ImageSearch::isImageWithinVideo(target_image, video, CONFIDENCE_THRESHOLD, matches);

    // Then
    EXPECT_EQ(MatchStatus::e_BAD_FILE, result);
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

    const std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    std::vector<Match> matches;
    MatchStatus result = ImageSearch::isImageWithinVideo(target_image, video, CONFIDENCE_THRESHOLD, matches);
    video.release();

    // Then
    EXPECT_EQ(MatchStatus::e_SUCCESS, result);
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        auto best_match = std::max_element(matches.begin(), matches.end(),
            [](const Match& a, const Match& b) { return a.score < b.score; });
        EXPECT_GT(best_match->score, CONFIDENCE_THRESHOLD);
        EXPECT_GE(best_match->frame_index, 1);
        EXPECT_LE(best_match->frame_index, 2);
    }

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

    const std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    std::vector<Match> matches;
    const MatchStatus result =
        ImageSearch::isImageWithinVideo(target_image, video, CONFIDENCE_THRESHOLD, matches);
    video.release();

    // Then
    EXPECT_EQ(MatchStatus::e_SUCCESS, result);
    EXPECT_FALSE(matches.empty());

    // Find match at frame 4
    auto frame4_match = std::find_if(matches.begin(), matches.end(),
        [](const Match& m) { return m.frame_index == 4; });
    EXPECT_NE(frame4_match, matches.end());

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

    const std::string video_path = createTestVideo(frames);
    ASSERT_FALSE(video_path.empty());

    // When
    cv::VideoCapture video(video_path);
    std::vector<Match> matches;
    MatchStatus result = ImageSearch::isImageWithinVideo(target_image, video, CONFIDENCE_THRESHOLD, matches);
    video.release();

    // Then
    EXPECT_EQ(MatchStatus::e_SUCCESS, result);
    EXPECT_FALSE(matches.empty());
    if (!matches.empty()) {
        EXPECT_EQ(1, matches[0].frame_index);
    }

    // Cleanup
    cleanupTestVideo(video_path);
}

