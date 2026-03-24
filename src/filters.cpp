#include "filters.hpp"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <vector>

cv::Mat applyGaussianFilter(const cv::Mat& image, int kernelSize, double sigma){

    if(image.empty()){
        throw std::invalid_argument("Input image empty");
    }

    if(kernelSize <= 0){
        throw std::invalid_argument("Kernel size must be positive");
    }
    if(sigma <= 0.0){
        throw std::invalid_argument("Sigma must be positive");
    }
    if(kernelSize % 2 == 0){
        kernelSize++;
    }

    cv::Mat result;
    cv::GaussianBlur(image, result, cv::Size(kernelSize, kernelSize), sigma, sigma);
    return result;
}

cv::Mat applyMedianFilter(const cv::Mat& image, int kernelSize){
    if(image.empty()){
        throw std::invalid_argument("Input image empty");
    }

    if(kernelSize <= 0){
        throw std::invalid_argument("Kernel size must be positive");
    }
    if(kernelSize % 2 == 0){
        kernelSize++;
    }

    cv::Mat result;
    cv::medianBlur(image, result, kernelSize);
    return result;
}

cv::Mat applyBilateralFilter(const cv::Mat& image, int diameter, double sigmaColor, double sigmaSpace) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }
    if (diameter <= 0) {
        throw std::invalid_argument("Diameter must be positive");
    }
    if (sigmaColor <= 0.0 || sigmaSpace <= 0.0) {
        throw std::invalid_argument("Sigma values must be positive");
    }

    cv::Mat result;
    cv::bilateralFilter(image, result, diameter, sigmaColor, sigmaSpace);
    return result;
}

cv::Mat applyCLAHE(const cv::Mat& image, double clipLimit, cv::Size tileGridSize) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }
    if (clipLimit <= 0.0) {
        throw std::invalid_argument("Clip limit must be positive");
    }
    if (tileGridSize.width <= 0 || tileGridSize.height <= 0) {
        throw std::invalid_argument("Tile grid size must be positive");
    }

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clipLimit, tileGridSize);
    cv::Mat result;

    if (image.channels() == 1) {
        clahe->apply(image, result);
    } else if (image.channels() == 3) {
        cv::Mat lab;
        cv::cvtColor(image, lab, cv::COLOR_BGR2Lab);

        std::vector<cv::Mat> channels;
        cv::split(lab, channels);

        clahe->apply(channels[0], channels[0]);

        cv::merge(channels, lab);
        cv::cvtColor(lab, result, cv::COLOR_Lab2BGR);
    } else {
        throw std::invalid_argument("CLAHE supports only 1-ch or 3-ch images");
    }

    return result;
}

cv::Mat applyUnsharpMask(const cv::Mat& image, int kernelSize, double sigma, double amount) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }
    if (kernelSize <= 0) {
        throw std::invalid_argument("Kernel size must be positive");
    }
    if (sigma <= 0.0) {
        throw std::invalid_argument("Sigma must be positive");
    }
    if (amount <= 0.0) {
        throw std::invalid_argument("Amount must be positive");
    }
    if (kernelSize % 2 == 0) {
        kernelSize += 1;
    }

    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(kernelSize, kernelSize), sigma, sigma);

    cv::Mat result;
    cv::addWeighted(image, 1.0 + amount, blurred, -amount, 0.0, result);

    return result;
}

cv::Mat applyLaplacianSharpening(const cv::Mat& image, double amount) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    if (amount <= 0.0) {
        throw std::invalid_argument("Amount must be positive");
    }

    cv::Mat floatImage;
    image.convertTo(floatImage, CV_32F);

    cv::Mat laplacian;
    cv::Laplacian(floatImage, laplacian, CV_32F);

    cv::Mat sharpened = floatImage - amount * laplacian;

    cv::Mat result;
    sharpened.convertTo(result, image.type());

    return result;
}


