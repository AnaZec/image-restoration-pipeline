#include "experiment_runner.hpp"

#include "io.hpp"
#include "metrics.hpp"
#include "pipeline.hpp"
#include "reporting.hpp"
#include "timer.hpp"
#include "visualization.hpp"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace irp
{
namespace
{

std::string makeIndexedFileName(int index,
                                const std::string& label,
                                const std::string& extension = ".jpg")
{
    std::ostringstream out;
    if (index < 10) {
        out << "0";
    }
    out << index << "_" << label << extension;
    return out.str();
}

} // namespace

ExperimentResult runExperiment(const cv::Mat& original, const ExperimentConfig& config)
{
    if (original.empty()) {
        throw std::invalid_argument("Original image is empty");
    }

    ExperimentResult result;
    result.config = config;
    result.originalImage = original.clone();

    result.degradedImage = applyDegradation(original, config.degradation);

    result.stageOutputs.push_back(
        StageResult{
            "degraded",
            makeIndexedFileName(1, "degraded_" + degradationTypeToString(config.degradation.type)),
            result.degradedImage.clone()
        });

    cv::Mat current = result.degradedImage.clone();

    Timer timer;
    timer.start();

    for (std::size_t i = 0; i < config.stages.size(); ++i) {
        const StageConfig& stageCfg = config.stages[i];
        current = applyStage(current, stageCfg);

        const int fileIndex = static_cast<int>(i) + 2;
        result.stageOutputs.push_back(
            StageResult{
                stageTypeToFileLabel(stageCfg.type),
                makeIndexedFileName(fileIndex, stageTypeToFileLabel(stageCfg.type)),
                current.clone()
            });
    }

    timer.stop();

    result.finalImage = current.clone();

    result.metrics.degradedMSE = computeMSE(original, result.degradedImage);
    result.metrics.degradedPSNR = computePSNR(original, result.degradedImage);
    result.metrics.degradedSSIM = computeSSIM(original, result.degradedImage);
    result.metrics.finalMSE = computeMSE(original, result.finalImage);
    result.metrics.finalPSNR = computePSNR(original, result.finalImage);
    result.metrics.finalSSIM = computeSSIM(original, result.finalImage);
    result.metrics.psnrImprovement = result.metrics.finalPSNR - result.metrics.degradedPSNR;
    result.metrics.runtimeMs = timer.elapsedMilliseconds();
    result.metrics.ssimImprovement =  result.metrics.finalSSIM - result.metrics.degradedSSIM;

    return result;
}

void saveExperimentOutputs(const ExperimentResult& result, const std::string& outputRoot)
{
    if (result.originalImage.empty() || result.degradedImage.empty() || result.finalImage.empty()) {
        throw std::runtime_error("Cannot save experiment outputs: result images are incomplete");
    }

    const std::string dir = outputRoot + "/" + result.config.outputDir;

    io::ensureOutputDir(dir);
    io::saveOutputImageToDir(dir, "00_original.jpg", result.originalImage);

    for (const auto& stage : result.stageOutputs) {
        io::saveOutputImageToDir(dir, stage.filename, stage.image);
    }

    const int finalIndex = static_cast<int>(result.stageOutputs.size()) + 1;
    io::saveOutputImageToDir(
        dir,
        makeIndexedFileName(finalIndex, "final_result"),
        result.finalImage);

    std::vector<cv::Mat> stripImages;
    stripImages.push_back(result.originalImage);
    stripImages.push_back(result.degradedImage);

    for (const auto& stage : result.stageOutputs) {
        if (stage.label != "degraded") {
            stripImages.push_back(stage.image);
        }
    }

    io::saveOutputImageToDir(
        dir,
        "99_comparison_strip.jpg",
        makeComparisonStrip(stripImages));

    io::saveTextFileToDir(dir, "report.txt", buildExperimentReport(result));
}

} // namespace irp