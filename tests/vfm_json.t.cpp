#include <gtest/gtest.h>
#include <vfm_json.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

// Test fixture to clean up test files
class VfmJsonTest : public ::testing::Test {
protected:
    void TearDown() override {
        // Clean up any test files created
        for (const auto& file : test_files_) {
            std::filesystem::remove(file);
        }
    }

    void trackFile(const std::string& filename) {
        test_files_.push_back(filename);
    }

private:
    std::vector<std::string> test_files_;
};

TEST_F(VfmJsonTest, matchResultsToJsonCreatesValidJsonGivenSuccessfulMatchWithBbox) {
    // Given
    MatchResults results(
        MatchStatus::e_SUCCESS,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );
    const Match match(
        5.5,
        165,
        0.95,
        true,
        100, 200, 300, 400
    );
    results.matches.push_back(match);

    // When
    nlohmann::json j = vfm::matchResultsToJson(results);

    // Then
    EXPECT_EQ(j["status"], "SUCCESS");
    EXPECT_EQ(j["video"]["path"], "video.mp4");
    EXPECT_DOUBLE_EQ(j["video"]["fps"], 30.0);
    EXPECT_EQ(j["video"]["frame_count"], 900);
    EXPECT_DOUBLE_EQ(j["video"]["duration_sec"], 30.0);
    EXPECT_EQ(j["video"]["width"], 1920);
    EXPECT_EQ(j["video"]["height"], 1080);
    EXPECT_EQ(j["image"]["path"], "image.jpg");
    EXPECT_EQ(j["image"]["width"], 640);
    EXPECT_EQ(j["image"]["height"], 480);
    EXPECT_EQ(j["image"]["channels"], 3);
    ASSERT_EQ(j["matches"].size(), 1);
    EXPECT_DOUBLE_EQ(j["matches"][0]["time_seconds"], 5.5);
    EXPECT_EQ(j["matches"][0]["frame_index"], 165);
    EXPECT_DOUBLE_EQ(j["matches"][0]["score"], 0.95);
    EXPECT_TRUE(j["matches"][0]["has_bbox"]);
    EXPECT_EQ(j["matches"][0]["bbox"]["x"], 100);
    EXPECT_EQ(j["matches"][0]["bbox"]["y"], 200);
    EXPECT_EQ(j["matches"][0]["bbox"]["w"], 300);
    EXPECT_EQ(j["matches"][0]["bbox"]["h"], 400);
}

TEST_F(VfmJsonTest, matchResultsToJsonExcludesBboxGivenMatchWithoutBbox) {
    // Given
    MatchResults results(
        MatchStatus::e_SUCCESS,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );

    const Match match(
        12.3,
        369,
        0.87,
        false,
        0, 0, 0, 0
    );
    results.matches.push_back(match);

    // When
    nlohmann::json j = vfm::matchResultsToJson(results);

    // Then
    ASSERT_EQ(j["matches"].size(), 1);
    EXPECT_FALSE(j["matches"][0]["has_bbox"]);
    EXPECT_FALSE(j["matches"][0].contains("bbox"));
}

TEST_F(VfmJsonTest, matchResultsToJsonHandlesNoMatchFoundStatus) {
    // Given
    const MatchResults results(
        MatchStatus::e_NO_MATCH_FOUND,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );

    // When
    nlohmann::json j = vfm::matchResultsToJson(results);

    // Then
    EXPECT_EQ(j["status"], "NO_MATCH_FOUND");
    EXPECT_EQ(j["matches"].size(), 0);
}

TEST_F(VfmJsonTest, matchResultsToJsonHandlesBadFileStatus) {
    // Given
    const MatchResults results(
        MatchStatus::e_BAD_FILE,
        {"image.jpg"},
        {"video.mp4"},
        {}
    );

    // When
    nlohmann::json j = vfm::matchResultsToJson(results);

    // Then
    EXPECT_EQ(j["status"], "BAD_FILE");
}

TEST_F(VfmJsonTest, matchResultsToJsonHandlesMultipleMatches) {
    // Given
    MatchResults results(
        MatchStatus::e_BAD_FILE,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );

    const Match match1(
        5.5,
        165,
        0.95,
        true,
        100, 200, 300, 400
    );

    const Match match2(
        12.3,
        369,
        0.87,
        false,
        0, 0, 0, 0
    );

    const Match match3(
        20.1,
        603,
        0.92,
        true,
        50, 75, 200, 150
    );

    results.matches.push_back(match1);
    results.matches.push_back(match2);
    results.matches.push_back(match3);

    // When
    nlohmann::json j = vfm::matchResultsToJson(results);

    // Then
    ASSERT_EQ(j["matches"].size(), 3);
    EXPECT_DOUBLE_EQ(j["matches"][0]["time_seconds"], 5.5);
    EXPECT_DOUBLE_EQ(j["matches"][1]["time_seconds"], 12.3);
    EXPECT_DOUBLE_EQ(j["matches"][2]["time_seconds"], 20.1);
    EXPECT_TRUE(j["matches"][0]["has_bbox"]);
    EXPECT_FALSE(j["matches"][1]["has_bbox"]);
    EXPECT_TRUE(j["matches"][2]["has_bbox"]);
}

TEST_F(VfmJsonTest, matchResultsToJsonStringCreatesFormattedStringGivenIndent) {
    // Given
    const MatchResults results(
        MatchStatus::e_SUCCESS,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );

    // When
    std::string json_str = vfm::matchResultsToJsonString(results, 4);

    // Then
    EXPECT_TRUE(json_str.find("    ") != std::string::npos); // Contains indentation
    EXPECT_TRUE(json_str.find("\"status\": \"SUCCESS\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"video\"") != std::string::npos);
    EXPECT_TRUE(json_str.find("\"image\"") != std::string::npos);
}

TEST_F(VfmJsonTest, matchResultsToJsonStringCreatesCompactStringGivenNoIndent) {
    // Given
    const MatchResults results(
        MatchStatus::e_SUCCESS,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );
    // When
    std::string json_str = vfm::matchResultsToJsonString(results, -1);

    // Then
    EXPECT_TRUE(json_str.find("\n") == std::string::npos); // No newlines
    EXPECT_TRUE(json_str.find("\"status\":\"SUCCESS\"") != std::string::npos);
}

TEST_F(VfmJsonTest, writeMatchResultsToFileCreatesFileGivenValidPath) {
    // Given
    MatchResults results(
        MatchStatus::e_SUCCESS,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );
    
    const Match match(
        5.5,
        165,
        0.95,
        true,
        100, 200, 300, 400
    );
    results.matches.push_back(match);

    const std::string filename = "test_output_valid.json";
    trackFile(filename);

    // When
    const bool success = vfm::writeMatchResultsToFile(results, filename, 4);

    // Then
    EXPECT_TRUE(success);
    EXPECT_TRUE(std::filesystem::exists(filename));

    // Verify file contents
    std::ifstream file(filename);
    ASSERT_TRUE(file.is_open());
    nlohmann::json j;
    file >> j;
    file.close();

    EXPECT_EQ(j["status"], "SUCCESS");
    EXPECT_EQ(j["video"]["path"], "video.mp4");
    EXPECT_EQ(j["matches"].size(), 1);
    EXPECT_DOUBLE_EQ(j["matches"][0]["score"], 0.95);
}

TEST_F(VfmJsonTest, writeMatchResultsToFileReturnsFalseGivenInvalidPath) {
    // Given
    const MatchResults results(
        MatchStatus::e_SUCCESS,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );

    // When
    const bool success = vfm::writeMatchResultsToFile(
        results, "/invalid/path/that/does/not/exist/output.json", 4);

    // Then
    EXPECT_FALSE(success);
}

TEST_F(VfmJsonTest, writeMatchResultsToFileWritesCompactJsonGivenNegativeIndent) {
    // Given
    MatchResults results(
        MatchStatus::e_NO_MATCH_FOUND,
        {"image.jpg", 640, 480, 3},
        {"video.mp4", 30.0, 900, 30.0, 1920, 1080},
        {}
    );

    const std::string filename = "test_output_compact.json";
    trackFile(filename);

    // When
    const bool success = vfm::writeMatchResultsToFile(results, filename, -1);

    // Then
    EXPECT_TRUE(success);

    std::ifstream file(filename);
    ASSERT_TRUE(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // Compact JSON should not have newlines
    EXPECT_TRUE(content.find("\n") == std::string::npos);
}

TEST_F(VfmJsonTest, writeMatchResultsToFileOverwritesExistingFile) {
    // Given
    std::string filename = "test_overwrite.json";
    trackFile(filename);

    // Create initial file
    const MatchResults results1(
        MatchStatus::e_NO_MATCH_FOUND,
        {"old_image.jpg", 320, 240, 3},
        {"old_video.mp4", 25.0, 750, 30.0, 1280, 720},
        {}
    );
    vfm::writeMatchResultsToFile(results1, filename, 4);

    // Create new results
    const MatchResults results2(
        MatchStatus::e_SUCCESS,
        {"new_image.jpg", 1920, 1080, 3},
        {"new_video.mp4", 60.0, 1800, 30.0, 3840, 2160},
        {}
    );

    // When
    const bool success = vfm::writeMatchResultsToFile(results2, filename, 4);

    // Then
    EXPECT_TRUE(success);

    std::ifstream file(filename);
    nlohmann::json j;
    file >> j;
    file.close();

    EXPECT_EQ(j["status"], "SUCCESS");
    EXPECT_EQ(j["video"]["path"], "new_video.mp4");
    EXPECT_EQ(j["video"]["fps"], 60.0);
}
