#include <gtest/gtest.h>
#include <vfm_args.h>
#include <CLI/CLI.hpp>
#include <vector>
#include <string>

// Helper function to convert vector of strings to argc/argv
class ArgvHelper {
public:
    ArgvHelper(const std::vector<std::string>& args) {
        argc_ = static_cast<int>(args.size());
        argv_ = new char*[argc_];
        for (int i = 0; i < argc_; ++i) {
            argv_[i] = new char[args[i].size() + 1];
            std::memcpy(argv_[i], args[i].c_str(), args[i].size() + 1);
        }
    }

    ~ArgvHelper() {
        for (int i = 0; i < argc_; ++i) {
            delete[] argv_[i];
        }
        delete[] argv_;
    }

    int argc() const { return argc_; }
    char** argv() const { return argv_; }

private:
    int argc_;
    char** argv_;
};

TEST(VfmArgs, parseArgsSetsDefaultValuesGivenVideoAndImageOnly) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_EQ(args.video_path, "test.mp4");
    EXPECT_EQ(args.image_path, "ref.jpg");
    EXPECT_DOUBLE_EQ(args.threshold, 0.85);
    EXPECT_EQ(args.frame_step, 2);
    EXPECT_DOUBLE_EQ(args.start_time, 0.0);
    EXPECT_FALSE(args.end_time.has_value());
    EXPECT_TRUE(args.output_json.empty());
}

TEST(VfmArgs, parseArgsThrowsWhenMissingVideoArgument) {
    // Given
    ArgvHelper helper({"vfm", "--image", "ref.jpg"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ParseError);
}

TEST(VfmArgs, parseArgsThrowsWhenMissingImageArgument) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ParseError);
}

TEST(VfmArgs, parseArgsThrowsWhenMissingBothRequiredArguments) {
    // Given
    ArgvHelper helper({"vfm"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ParseError);
}

TEST(VfmArgs, parseArgsSetsCustomThresholdValue) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--threshold", "0.95"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_DOUBLE_EQ(args.threshold, 0.95);
}

TEST(VfmArgs, parseArgsThrowsWhenThresholdTooLow) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--threshold", "-0.1"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ValidationError);
}

TEST(VfmArgs, parseArgsThrowsWhenThresholdTooHigh) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--threshold", "1.5"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ValidationError);
}

TEST(VfmArgs, parseArgsSetsThresholdWhenThresholdZero) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--threshold", "0.0"});

    // When / Then
    Args args = parseArgs(helper.argc(), helper.argv());
    EXPECT_DOUBLE_EQ(args.threshold, 0.0);
}

TEST(VfmArgs, parseArgsSetsThresholdWhenThresholdOne) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--threshold", "1.0"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());
    
    // Then
    EXPECT_DOUBLE_EQ(args.threshold, 1.0);
}

TEST(VfmArgs, parseArgsSetsCustomFrameStepValue) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--frame-step", "5"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_EQ(args.frame_step, 5);
}

TEST(VfmArgs, parseArgsThrowsWhenFrameStepNegative) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--frame-step", "-1"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ValidationError);
}

TEST(VfmArgs, parseArgsThrowsWhenFrameStepZero) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--frame-step", "0"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ValidationError);
}

TEST(VfmArgs, parseArgsSetsCustomStartTimeValue) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--start-time", "10.5"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_DOUBLE_EQ(args.start_time, 10.5);
}

TEST(VfmArgs, parseArgsThrowsWhenStartTimeNegative) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--start-time", "-5.0"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ValidationError);
}

TEST(VfmArgs, parseArgsSetsStartTimeWhenStartTimeZero) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--start-time", "0.0"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_DOUBLE_EQ(args.start_time, 0.0);
}

TEST(VfmArgs, parseArgsSetsCustomEndTimeValue) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--end-time", "30.5"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    ASSERT_TRUE(args.end_time.has_value());
    EXPECT_DOUBLE_EQ(*args.end_time, 30.5);
}

TEST(VfmArgs, parseArgsThrowsWhenEndTimeEarlierThanStartTime) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--start-time", "20.0", "--end-time", "10.0"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ValidationError);
}

TEST(VfmArgs, parseArgsSetsEndTimeWhenEndTimeEqualToStartTime) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--start-time", "15.0", "--end-time", "15.0"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    ASSERT_TRUE(args.end_time.has_value());
    EXPECT_DOUBLE_EQ(*args.end_time, 15.0);
    EXPECT_DOUBLE_EQ(args.start_time, 15.0);
}

TEST(VfmArgs, parseArgsSetsEndTimeWhenEndTimeGreaterThanStartTime) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--start-time", "5.0", "--end-time", "25.0"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    ASSERT_TRUE(args.end_time.has_value());
    EXPECT_DOUBLE_EQ(*args.end_time, 25.0);
    EXPECT_DOUBLE_EQ(args.start_time, 5.0);
}

TEST(VfmArgs, parseArgsSetsOutputJsonPath) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--output-json", "results.json"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_EQ(args.output_json, "results.json");
}

TEST(VfmArgs, parseArgsSetsAllValuesGivenAllArguments) {
    // Given
    ArgvHelper helper({
        "vfm",
        "--video", "input.mp4",
        "--image", "reference.png",
        "--threshold", "0.90",
        "--frame-step", "10",
        "--start-time", "5.5",
        "--end-time", "60.0",
        "--output-json", "output.json"
    });

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_EQ(args.video_path, "input.mp4");
    EXPECT_EQ(args.image_path, "reference.png");
    EXPECT_DOUBLE_EQ(args.threshold, 0.90);
    EXPECT_EQ(args.frame_step, 10);
    EXPECT_DOUBLE_EQ(args.start_time, 5.5);
    ASSERT_TRUE(args.end_time.has_value());
    EXPECT_DOUBLE_EQ(*args.end_time, 60.0);
    EXPECT_EQ(args.output_json, "output.json");
}

// Test invalid argument name
TEST(VfmArgs, parseArgsThrowsGivenInvalidArgument) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--invalid-arg", "value"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ParseError);
}

TEST(VfmArgs, parseArgsThrowsGivenBadThresholdType) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--threshold", "not-a-number"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ValidationError);
}

TEST(VfmArgs, parseArgsThrowsGivenBadFrameStepType) {
    // Given
    ArgvHelper helper({"vfm", "--video", "test.mp4", "--image", "ref.jpg", "--frame-step", "3.5"});

    // When / Then
    EXPECT_THROW({
        parseArgs(helper.argc(), helper.argv());
    }, CLI::ConversionError);
}

TEST(VfmArgs, parseArgsSetsVideoAndImagePathsWithSpaces) {
    // Given
    ArgvHelper helper({"vfm", "--video", "my video.mp4", "--image", "my image.jpg"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_EQ(args.video_path, "my video.mp4");
    EXPECT_EQ(args.image_path, "my image.jpg");
}

TEST(VfmArgs, parseArgsSetsVideoAndImagePathsWithSpecialCharacters) {
    // Given
    ArgvHelper helper({"vfm", "--video", "/path/to/video-file_123.mp4", "--image", "/path/to/image_ref@2x.png"});

    // When
    Args args = parseArgs(helper.argc(), helper.argv());

    // Then
    EXPECT_EQ(args.video_path, "/path/to/video-file_123.mp4");
    EXPECT_EQ(args.image_path, "/path/to/image_ref@2x.png");
}
