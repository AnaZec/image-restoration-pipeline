#include "degradation.hpp"
#include <opencv2/opencv.hpp>
#include <stdexcept>

cv::Mat addGaussianNoise(const cv::Mat& image, double mean, double stddev) {
    
    if(image.empty()){
        throw std::invalid_argument("Input image empty");
    }

    cv::Mat floatImage;
    image.convertTo(floatImage, CV_32F);

    cv::Mat noise(image.size(), floatImage.type());
    cv::randn(noise, mean, stddev);

    cv::Mat noisyImage = floatImage + noise;

    cv::Mat result;
    noisyImage.convertTo(result, image.type());

    return result;
}

cv::Mat addSaltAndPepperNoise(const cv::Mat& image, double saltProb, double pepperProb) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    if (saltProb < 0.0 || pepperProb < 0.0 || saltProb > 1.0 || pepperProb > 1.0) {
        throw std::invalid_argument("Salt and pepper probabilities must be in range [0, 1]");
    }

    if (saltProb + pepperProb > 1.0) {
        throw std::invalid_argument("Sum of salt and pepper probabilities must not exceed 1");
    }

    cv::Mat result = image.clone();

    for (int row = 0; row < result.rows; ++row) {
        for (int col = 0; col < result.cols; ++col) {
            double r = static_cast<double>(std::rand()) / RAND_MAX;

            if (r < pepperProb) {
                if (result.channels() == 1) {
                    result.at<uchar>(row, col) = 0;
                } else if (result.channels() == 3) {
                    result.at<cv::Vec3b>(row, col) = cv::Vec3b(0, 0, 0);
                }
            } else if (r < pepperProb + saltProb) {
                if (result.channels() == 1) {
                    result.at<uchar>(row, col) = 255;
                } else if (result.channels() == 3) {
                    result.at<cv::Vec3b>(row, col) = cv::Vec3b(255, 255, 255);
                }
            }
        }
    }

    return result;
}

cv::Mat contrastReduction(const cv::Mat& image, double alpha, double beta){
    if(image.empty()){
        throw std::invalid_argument("Input image empty");
    }
    if(alpha < 0.0 || alpha > 1.0){
        throw std::invalid_argument("Alpha should be in range [0,1]");
    }

    cv::Mat result;
    image.convertTo(result, image.type(), alpha, beta);

    return result;

}
cv::Mat motionBlur(const cv::Mat& image, int kernelSize, double motionAngle){
    if(image.empty()){
        throw std::invalid_argument("Input image empty");
    }
    if(kernelSize <= 0){
        throw std::invalid_argument("Kernel size must be positive");
    }

    if(kernelSize % 2 == 0){
        kernelSize++;
    }

    // Horizontal motion kernel
    cv::Mat kernel = cv::Mat::zeros(kernelSize, kernelSize, CV_32F);
    int center = kernelSize / 2;

    for(int i = 0; i < kernelSize; i++){
        kernel.at<float>(center,i) = 1.0f;
    }

    // Kernel rotation to the desired motion angle
    cv::Point2f rotationCenter(kernelSize / 2.0f, kernelSize / 2.0f);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(rotationCenter, motionAngle, 1.0);

    cv::Mat rotatedKernel;
    cv::warpAffine(kernel, rotatedKernel, rotationMatrix, kernel.size());

    //Normalization
    double kernelSum = cv::sum(rotatedKernel)[0];
    if (kernelSum > 0.0) {
        rotatedKernel /= kernelSum;
    }
    
    cv::Mat result;
    cv::filter2D(image, result, -1, rotatedKernel);

    return result;
}

