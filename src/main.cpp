#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

#include <vfm_imagesearch.h>

namespace {
	const std::string video_path = "example.mp4";
	const std::string image_path = "example_frame.jpg";
}

int main() {
    ImageSearch ImSearch;

	cv::VideoCapture user_video(video_path);
	cv::Mat          user_image = cv::imread(image_path);

    const ImageSearch::ReturnCode matchStatus = ImSearch.isImageWithinVideo(user_image, user_video);

    switch (matchStatus) {
        case ImageSearch::e_SUCCESS:
            ImSearch.exportResultFrame();
            std::cout << "Selected frame # "    << ImSearch.result_frame_count()
                    << " with confidence of " << ImSearch.result_confidence() * 100
                    << "%." << std::endl;
            break;
        
        case ImageSearch::e_NO_MATCH_FOUND:
            std::cout << "No match found" << std::endl;
            break;

        case ImageSearch::e_BAD_FILE:
            std::cerr << "Error: could not open file" << std::endl;
            break;
        
        default:
            std::cerr << "Error: unexpected return value" << std::endl;
            break;

    }

	return 0;
}
