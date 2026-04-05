#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include <opencv2/opencv.hpp>
#include <vector>

namespace irp
{

cv::Mat makeComparisonStrip(const std::vector<cv::Mat>& images, int targetHeight = 300);

} // namespace irp

#endif // VISUALIZATION_HPP