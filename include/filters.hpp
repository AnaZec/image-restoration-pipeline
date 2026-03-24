#ifndef FILTERS_HPP
#define FILTERS_HPP

#include <opencv2/opencv.hpp>

cv::Mat applyGaussianFilter(const cv::Mat& image, int kernelSize, double sigma);
cv::Mat applyMedianFilter(const cv::Mat& image, int kernelSize);
cv::Mat applyBilateralFilter(const cv::Mat& image, int diameter, double sigmaColor, double sigmaSpace);
cv::Mat applyCLAHE(const cv::Mat& image, double clipLimit, cv::Size tileGridSize);
cv::Mat applyUnsharpMask(const cv::Mat& image, int kernelSize, double sigma, double amount);
cv::Mat applyLaplacianSharpening(const cv::Mat& image, double amount);


#endif
