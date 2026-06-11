#include <vfm_imagesearch.h>
#include <opencv2/opencv.hpp>

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
        const double confidence = result.at<float>(loc);

        // Check if this location is too close to an existing match
        bool is_duplicate = false;
        for (const Match& existing : matches) {
            if (existing.frame_index == frame_index) {
                const int dx = loc.x - existing.x;
                const int dy = loc.y - existing.y;
                const int distance = std::sqrt(dx*dx + dy*dy);
                if (distance < min_distance) {
                    is_duplicate = true;
                    break;
                }
            }
        }

        if (!is_duplicate) {
            const Match match(
                fps ? (frame_index / fps) : 0.0,
                frame_index,
                confidence,
                true,
                loc.x,
                loc.y,
                image.cols,
                image.rows
            );
            matches.push_back(match);
        }
    }
}

MatchStatus ImageSearch::isImageWithinVideo(const cv::Mat& target_image, cv::VideoCapture& source_video, double threshold, std::vector<Match>& matches)
{
    if (!source_video.isOpened()) {
		return MatchStatus::e_BAD_FILE;
	}

	matches.clear();
	cv::Mat frame;
	const int total_frames = static_cast<int>(source_video.get(cv::CAP_PROP_FRAME_COUNT));
	const double fps = source_video.get(cv::CAP_PROP_FPS);

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
    const cv::Mat image = cv::imread(image_path);

    if (image.empty()) {
        const ImageMetadata badImageMeta(image_path);
        const VideoMetadata badVideoMeta(image_path);

        return MatchResults(MatchStatus::e_BAD_FILE, badImageMeta, badVideoMeta, std::vector<Match>{});
    }

    // Populate metadata
    const ImageMetadata imageMeta = getImageMetadata(image_path);
    const VideoMetadata videoMeta = getVideoMetadata(video_path);

    // Perform the search
    cv::VideoCapture video(video_path);
    std::vector<Match> matches;
    const MatchStatus status = isImageWithinVideo(image, video, threshold, matches);

    return MatchResults(status, imageMeta, videoMeta, matches);
}

// Metadata Functions
ImageMetadata ImageSearch::getImageMetadata(const std::string& image_path)
{
    const cv::Mat image = cv::imread(image_path);

    return image.empty()
           ? ImageMetadata(image_path)
           : ImageMetadata(image_path, image.cols, image.rows, image.channels());
}

VideoMetadata ImageSearch::getVideoMetadata(const std::string& video_path)
{
    cv::VideoCapture video(video_path);

    if (video.isOpened()) {
        const double fps = video.get(cv::CAP_PROP_FPS); 
        const int frame_count = static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
        const double duration = fps ? frame_count / fps : 0.0;
        const int width = static_cast<int>(video.get(cv::CAP_PROP_FRAME_WIDTH));
        const int height = static_cast<int>(video.get(cv::CAP_PROP_FRAME_HEIGHT));

        return VideoMetadata(video_path, fps, frame_count, duration, width, height);
    }

    return VideoMetadata(video_path);
}
