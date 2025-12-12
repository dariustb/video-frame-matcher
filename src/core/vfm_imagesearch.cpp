#include <vfm_imagesearch.h>
#include <opencv2/opencv.hpp>

namespace {
    const double CONFIDENCE_NONE      = 0;
    const double CONFIDENCE_THRESHOLD = 0.9;
    const double CONFIDENCE_CERTAIN   = 0.99;

	const std::string OUTPUT_DIR = "output/";
}  // namespace

ImageSearch::ImageSearch()
: ImageSearch(cv::Mat(0,0,CV_8UC1), 0, 0)
{
}

ImageSearch::ImageSearch(cv::Mat frame, int frame_count, double confidence)
: d_result_frame(frame)
, d_result_frame_count(frame_count)
, d_result_confidence(confidence)
{
}

// Getters
double ImageSearch::result_confidence() const
{
    return d_result_confidence;
}

int ImageSearch::result_frame_count() const
{
    return d_result_frame_count;
}

// Search Functions
bool ImageSearch::isImageWithinFrame(const cv::Mat& image, const cv::Mat& frame, double& confidence)
{
    cv::Mat   result;
    double    minVal = 0;
	double    maxVal = 0;
    cv::Point minLoc(0,0);
    cv::Point maxLoc(0,0);

    cv::matchTemplate(frame, image, result, cv::TM_CCOEFF_NORMED);
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    if (maxVal >= CONFIDENCE_THRESHOLD) {
        confidence = maxVal;
        return true;
    }

    return false;
}

MatchStatus ImageSearch::isImageWithinVideo(const cv::Mat& target_image, cv::VideoCapture& source_video)
{
    if (!source_video.isOpened()) {
		return MatchStatus::e_BAD_FILE;
	}

	cv::Mat   frame;
	double    frame_confidence;
	const int total_frames      = source_video.get(cv::CAP_PROP_FRAME_COUNT);
	bool      isImageMatchFound = false;

	for (int frame_count = 1; frame_count <= total_frames; ++frame_count) {
		// Add new frame from video
		source_video >> frame;

		const bool isImageinFrame = isImageWithinFrame(target_image, frame, frame_confidence);

		if (isImageinFrame) {
			// Set flag to found
			isImageMatchFound = true;

			// Track highest confidence
			if (frame_confidence > d_result_confidence) {
				d_result_confidence  = frame_confidence;
				d_result_frame       = frame;
				d_result_frame_count = frame_count;

				// If openCV is certain, we can skip the rest of the frames.
				if (d_result_confidence > CONFIDENCE_CERTAIN) {
					frame.release();
					return MatchStatus::e_SUCCESS;
				}
			}
		}

		frame.release();
	}

	return isImageMatchFound ? MatchStatus::e_SUCCESS : MatchStatus::e_NO_MATCH_FOUND;
}

MatchStatus ImageSearch::searchVideoForImage(const std::string& image_path, const std::string& video_path, ImageSearch& result)
{
    cv::VideoCapture video(video_path);
    cv::Mat          image = cv::imread(image_path);

    if (image.empty()) {
        return MatchStatus::e_BAD_FILE;
    }

    return result.isImageWithinVideo(image, video);
}

// File I/O Functions
void ImageSearch::exportResultFrame()
{
    cv::imwrite(OUTPUT_DIR + "result.jpg", d_result_frame);
}
