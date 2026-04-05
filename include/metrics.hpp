#ifndef METRICS_HPP
#define METRICS_HPP

#include <opencv2/opencv.hpp>

double computeMSE(const cv::Mat& original, const cv::Mat& processed);
double computePSNR(const cv::Mat& original, const cv::Mat& processed);
double computeSSIM(const cv::Mat& img1, const cv::Mat& img2);

#endif
