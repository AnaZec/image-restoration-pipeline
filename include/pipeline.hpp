#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "experiment_types.hpp"

#include <opencv2/opencv.hpp>
#include <string>

namespace irp
{

cv::Mat applyDegradation(const cv::Mat& image, const DegradationConfig& cfg);
cv::Mat applyStage(const cv::Mat& image, const StageConfig& cfg);

std::string degradationTypeToString(DegradationType type);
std::string stageTypeToString(StageType type);
std::string stageTypeToFileLabel(StageType type);

} // namespace irp

#endif // PIPELINE_HPP