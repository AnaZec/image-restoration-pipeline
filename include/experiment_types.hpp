#ifndef EXPERIMENT_TYPES_HPP
#define EXPERIMENT_TYPES_HPP

#include <opencv2/core.hpp>

#include <string>
#include <vector>

namespace irp
{

enum class DegradationType
{
    GaussianNoise,
    SaltPepperNoise,
    LowContrastGaussianNoise,
    MotionBlur
};

enum class StageType
{
    GaussianFilter,
    MedianFilter,
    BilateralFilter,
    CLAHE,
    UnsharpMask,
    LaplacianSharpen,
    ManualGaussianFilter,   
    ManualMedianFilter      
};

struct DegradationConfig
{
    DegradationType type = DegradationType::GaussianNoise;

    double primaryValue = 0.0;
    double secondaryValue = 0.0;

    int kernelSize = 0;
    double angleDegrees = 0.0;
};

struct StageConfig
{
    StageType type = StageType::GaussianFilter;

    // Shared/general parameters
    int kernelSize = 0;
    double sigma = 0.0;

    // Bilateral filter parameters
    int diameter = 0;
    double sigmaColor = 0.0;
    double sigmaSpace = 0.0;

    // CLAHE parameters
    double clipLimit = 0.0;
    cv::Size tileGridSize = cv::Size(8, 8);

    // Sharpening parameters 
    double amount = 0.0;
};

struct ExperimentConfig
{
    std::string family;        // e.g. "pipelineA"
    std::string variantName;   // e.g. "A1"
    std::string description;   // e.g. "Gaussian noise -> Bilateral -> Unsharp"
    std::string outputDir;     // e.g. "output/pipelineA/A1"

    DegradationConfig degradation;
    std::vector<StageConfig> stages;
};

struct StageResult
{
    std::string label;         // e.g. "degraded", "bilateral", "unsharp"
    std::string filename;      // e.g. "01_degraded.png"
    cv::Mat image;
};

struct ExperimentMetrics
{
    double degradedMSE = 0.0;
    double degradedPSNR = 0.0;
    double degradedSSIM = 0.0;

    double finalMSE = 0.0;
    double finalPSNR = 0.0;
    double finalSSIM = 0.0;

    double psnrImprovement = 0.0;
    double ssimImprovement = 0.0;

    double runtimeMs = 0.0;
};

struct ExperimentResult
{
    ExperimentConfig config;

    cv::Mat originalImage;
    cv::Mat degradedImage;
    cv::Mat finalImage;

    std::vector<StageResult> stageOutputs;
    ExperimentMetrics metrics;
};

} 

#endif // EXPERIMENT_TYPES_HPP