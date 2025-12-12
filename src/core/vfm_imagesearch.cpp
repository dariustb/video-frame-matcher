#include <vfm_imagesearch.h>
#include <opencv2/opencv.hpp>

namespace {
	const std::string OUTPUT_DIR = "output/";
}  // namespace

// Search Functions
void ImageSearch::isImageWithinFrame(const cv::Mat& image, const cv::Mat& frame, int frame_index, double fps, double threshold, std::vector<Match>& matches)
{
    if (image.empty() || frame.empty()) {
        return;
    }

    cv::Mat   result;
    cv::matchTemplate(frame, image, result, cv::TM_CCOEFF_NORMED);

    // Find all matches above threshold
    cv::Mat mask = result >= threshold;

    std::vector<cv::Point> locations;
    cv::findNonZero(mask, locations);

    // Collect matches, avoiding duplicates from overlapping regions
    const int min_distance = std::max(image.cols, image.rows) / 2;

    for (const auto& loc : locations) {
        double confidence = result.at<float>(loc);

        // Check if this location is too close to an existing match
        bool is_duplicate = false;
        for (const auto& existing : matches) {
            if (existing.frame_index == frame_index) {
                int dx = loc.x - existing.x;
                int dy = loc.y - existing.y;
                int distance = std::sqrt(dx*dx + dy*dy);
                if (distance < min_distance) {
                    is_duplicate = true;
                    break;
                }
            }
        }

        if (!is_duplicate) {
            Match match;
            match.frame_index = frame_index;
            match.time_seconds = (fps > 0) ? (frame_index / fps) : 0.0;
            match.score = confidence;
            match.has_bbox = true;
            match.x = loc.x;
            match.y = loc.y;
            match.w = image.cols;
            match.h = image.rows;

            matches.push_back(match);
        }
    }
}

MatchStatus ImageSearch::isImageWithinVideo(const cv::Mat& target_image, cv::VideoCapture& source_video, double threshold, std::vector<Match>& matches)
{
    if (!source_video.isOpened()) {
		return MatchStatus::e_BAD_FILE;
	}

	cv::Mat   frame;
	const int total_frames = static_cast<int>(source_video.get(cv::CAP_PROP_FRAME_COUNT));
	const double fps = source_video.get(cv::CAP_PROP_FPS);

	matches.clear();

	for (int frame_index = 1; frame_index <= total_frames; ++frame_index) {
		// Read next frame from video
		source_video >> frame;

		if (frame.empty()) {
			break;
		}

		isImageWithinFrame(target_image, frame, frame_index, fps, threshold, matches);

		frame.release();
	}

	return matches.empty() ? MatchStatus::e_NO_MATCH_FOUND : MatchStatus::e_SUCCESS;
}

MatchResults ImageSearch::searchVideoForImage(const std::string& image_path, const std::string& video_path, double threshold)
{
    MatchResults results;

    cv::VideoCapture video(video_path);
    cv::Mat          image = cv::imread(image_path);

    if (image.empty()) {
        results.status = MatchStatus::e_BAD_FILE;
        return results;
    }

    // Populate metadata
    results.image = getImageMetadata(image_path);
    results.video = getVideoMetadata(video_path);

    // Perform the search
    results.status = isImageWithinVideo(image, video, threshold, results.matches);

    return results;
}

// Metadata Functions
ImageMetadata ImageSearch::getImageMetadata(const std::string& image_path)
{
    ImageMetadata metadata;
    metadata.path = image_path;

    cv::Mat image = cv::imread(image_path);

    if (image.empty()) {
        metadata.width = 0;
        metadata.height = 0;
        metadata.channels = 0;
    } else {
        metadata.width = image.cols;
        metadata.height = image.rows;
        metadata.channels = image.channels();
    }

    return metadata;
}

VideoMetadata ImageSearch::getVideoMetadata(const std::string& video_path)
{
    VideoMetadata metadata;
    metadata.path = video_path;

    cv::VideoCapture video(video_path);

    if (!video.isOpened()) {
        metadata.fps = 0.0;
        metadata.frame_count = 0;
        metadata.duration_sec = 0.0;
        metadata.width = 0;
        metadata.height = 0;
    } else {
        metadata.fps = video.get(cv::CAP_PROP_FPS);
        metadata.frame_count = static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
        metadata.width = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
        metadata.height = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));

        if (metadata.fps > 0) {
            metadata.duration_sec = metadata.frame_count / metadata.fps;
        } else {
            metadata.duration_sec = 0.0;
        }
    }

    return metadata;
}
