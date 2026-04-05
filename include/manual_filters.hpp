#ifndef MANUAL_FILTERS_HPP
#define MANUAL_FILTERS_HPP

#include <opencv2/opencv.hpp>

namespace irp
{

cv::Mat applyManualGaussianBlur(const cv::Mat& image, int kernelSize, double sigma);
cv::Mat applyManualMedianFilter(const cv::Mat& image, int kernelSize);

} // namespace irp

#endif // MANUAL_FILTERS_HPP