#ifndef EXPERIMENT_RUNNER_HPP
#define EXPERIMENT_RUNNER_HPP

#include "experiment_types.hpp"

#include <opencv2/opencv.hpp>

namespace irp
{

ExperimentResult runExperiment(const cv::Mat& original, const ExperimentConfig& config);
void saveExperimentOutputs(const ExperimentResult& result, const std::string& outputRoot);

} // namespace irp

#endif // EXPERIMENT_RUNNER_HPP