#ifndef DEGRADATION_HPP
#define DEGRADATION_HPP

#include <opencv2/opencv.hpp>

cv::Mat addGaussianNoise(const cv::Mat& image, double mean, double stddev);
cv::Mat addSaltAndPepperNoise(const cv::Mat& image, double saltProb, double pepperProb);
cv::Mat contrastReduction(const cv::Mat& image, double alpha, double beta); // alpha [0,1]
cv::Mat motionBlur(const cv::Mat& image, int kernelSize, double motionAngle); // kenel size -> odd

#endif