#include "reporting.hpp"

#include "pipeline.hpp"

#include <iomanip>
#include <sstream>
#include <string>

namespace irp
{
namespace
{

void appendDegradationDetails(std::ostringstream& out, const DegradationConfig& cfg)
{
    out << "Degradation: " << degradationTypeToString(cfg.type) << "\n";

    switch (cfg.type) {
        case DegradationType::GaussianNoise:
            out << "  stddev=" << cfg.primaryValue << "\n";
            break;

        case DegradationType::SaltPepperNoise:
            out << "  saltProb=" << cfg.primaryValue
                << ", pepperProb=" << cfg.secondaryValue << "\n";
            break;

        case DegradationType::LowContrastGaussianNoise:
            out << "  contrastAlpha=" << cfg.primaryValue
                << ", noiseStddev=" << cfg.secondaryValue << "\n";
            break;

        case DegradationType::MotionBlur:
            out << "  kernelSize=" << cfg.kernelSize
                << ", angleDegrees=" << cfg.angleDegrees << "\n";
            break;
    }
}

void appendStageDetails(std::ostringstream& out, const StageConfig& cfg, std::size_t index)
{
    out << "Stage " << (index + 1) << ": " << stageTypeToString(cfg.type) << "\n";

    switch (cfg.type) {
        case StageType::GaussianFilter:
            out << "  kernelSize=" << cfg.kernelSize
                << ", sigma=" << cfg.sigma << "\n";
            break;

        case StageType::MedianFilter:
            out << "  kernelSize=" << cfg.kernelSize << "\n";
            break;

        case StageType::BilateralFilter:
            out << "  diameter=" << cfg.diameter
                << ", sigmaColor=" << cfg.sigmaColor
                << ", sigmaSpace=" << cfg.sigmaSpace << "\n";
            break;

        case StageType::CLAHE:
            out << "  clipLimit=" << cfg.clipLimit
                << ", tileGridSize=(" << cfg.tileGridSize.width
                << ", " << cfg.tileGridSize.height << ")\n";
            break;

        case StageType::UnsharpMask:
            out << "  kernelSize=" << cfg.kernelSize
                << ", sigma=" << cfg.sigma
                << ", amount=" << cfg.amount << "\n";
            break;

        case StageType::LaplacianSharpen:
            out << "  amount=" << cfg.amount << "\n";
            break;
    }
}

std::string interpretResult(const ExperimentResult& result)
{
    const double psnrGain = result.metrics.psnrImprovement;
    const double ssimGain = result.metrics.ssimImprovement;

    if (psnrGain > 5.0 && ssimGain > 0.05) {
        return "Strong restoration improvement in both numerical and perceptual quality.";
    }

    if (psnrGain > 2.0 && ssimGain > 0.02) {
        return "Moderate restoration improvement with perceptual gain.";
    }

    if (psnrGain > 0.5 && ssimGain > 0.0) {
        return "Slight but positive restoration effect.";
    }

    if (psnrGain > 0.5 && ssimGain <= 0.0) {
        return "PSNR improved, but perceptual quality did not improve accordingly.";
    }

    if (psnrGain < 0.0 && ssimGain < 0.0) {
        return "Restoration pipeline degraded both numerical and perceptual quality.";
    }

    if (psnrGain < 0.0) {
        return "Restoration pipeline degraded numerical quality.";
    }

    return "Minimal overall effect on restoration quality.";
}

} // namespace

std::string buildExperimentReport(const ExperimentResult& result)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(4);

    out << "Experiment Report\n";
    out << "=================\n\n";

    out << "Family: " << result.config.family << "\n";
    out << "Variant: " << result.config.variantName << "\n";
    out << "Description: " << result.config.description << "\n";
    out << "Output directory: " << result.config.outputDir << "\n\n";

    appendDegradationDetails(out, result.config.degradation);
    out << "\n";

    for (std::size_t i = 0; i < result.config.stages.size(); ++i) {
        appendStageDetails(out, result.config.stages[i], i);
    }

    out << "\nMetrics\n";
    out << "-------\n";
    out << "Degraded MSE: " << result.metrics.degradedMSE << "\n";
    out << "Degraded PSNR: " << result.metrics.degradedPSNR << " dB\n";
    out << "Degraded SSIM: " << result.metrics.degradedSSIM << "\n";
    out << "Final MSE: " << result.metrics.finalMSE << "\n";
    out << "Final PSNR: " << result.metrics.finalPSNR << " dB\n";
    out << "Final SSIM: " << result.metrics.finalSSIM << "\n";
    out << "PSNR Improvement: " << result.metrics.psnrImprovement << " dB\n";
    out << "SSIM Improvement: " << result.metrics.ssimImprovement << "\n";
    out << "Runtime: " << result.metrics.runtimeMs << " ms\n";

    out << "\nInterpretation\n";
    out << "--------------\n";
    out << interpretResult(result) << "\n";

    return out.str();
}

std::string buildGlobalReport(const std::vector<ExperimentResult>& results,
                              const std::string& inputName)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(4);

    out << "Parameterized Image Restoration Report\n";
    out << "======================================\n\n";
    out << "Input image: " << inputName << "\n\n";

    for (const auto& result : results) {
        out << result.config.family << " / " << result.config.variantName << "\n";
        out << result.config.description << " [" << result.config.outputDir << "]\n";
        out << "Degraded MSE: " << result.metrics.degradedMSE << "\n";
        out << "Degraded PSNR: " << result.metrics.degradedPSNR << " dB\n";
        out << "Degraded SSIM: " << result.metrics.degradedSSIM << "\n";
        out << "Final MSE: " << result.metrics.finalMSE << "\n";
        out << "Final PSNR: " << result.metrics.finalPSNR << " dB\n";
        out << "Final SSIM: " << result.metrics.finalSSIM << "\n";
        out << "PSNR Improvement: " << result.metrics.psnrImprovement << " dB\n";
        out << "SSIM Improvement: " << result.metrics.ssimImprovement << "\n";
        out << "Runtime: " << result.metrics.runtimeMs << " ms\n";
        out << "Interpretation: " << interpretResult(result) << "\n\n";
    }

    return out.str();
}

std::string buildCSVReport(const std::vector<ExperimentResult>& results)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(6);

    out << "Family,Variant,Description,PSNR_before,PSNR_after,PSNR_gain,"
           "SSIM_before,SSIM_after,SSIM_gain,MSE_before,MSE_after,Runtime_ms\n";

    for (const auto& r : results) {
        out << r.config.family << ","
            << r.config.variantName << ","
            << "\"" << r.config.description << "\"" << ","
            << r.metrics.degradedPSNR << ","
            << r.metrics.finalPSNR << ","
            << r.metrics.psnrImprovement << ","
            << r.metrics.degradedSSIM << ","
            << r.metrics.finalSSIM << ","
            << r.metrics.ssimImprovement << ","
            << r.metrics.degradedMSE << ","
            << r.metrics.finalMSE << ","
            << r.metrics.runtimeMs << "\n";
    }

    return out.str();
}

} // namespace irp