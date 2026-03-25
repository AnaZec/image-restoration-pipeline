#include "pipeline.hpp"
#include "degradation.hpp"
#include "filters.hpp"

#include <opencv2/opencv.hpp>
#include <stdexcept>

cv::Mat runPipelineA(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    cv::Mat noisyImage = addGaussianNoise(image, 0.0, 25.0);
    cv::Mat restoredImage = applyGaussianFilter(noisyImage, 5, 1.0);

    return restoredImage;
}

cv::Mat runPipelineB(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    cv::Mat noisyImage = addSaltAndPepperNoise(image, 0.02, 0.02);
    cv::Mat restoredImage = applyMedianFilter(noisyImage, 5);

    return restoredImage;
}

cv::Mat runPipelineC(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    cv::Mat noisyImage = addSaltAndPepperNoise(image, 0.02, 0.02);
    cv::Mat restoredImage = applyGaussianFilter(noisyImage, 5, 1.0);

    return restoredImage;
}