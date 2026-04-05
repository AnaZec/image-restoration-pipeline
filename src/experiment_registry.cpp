#include "experiment_registry.hpp"

namespace irp
{
namespace
{

StageConfig makeGaussianStage(int kernelSize, double sigma)
{
    StageConfig stage;
    stage.type = StageType::GaussianFilter;
    stage.kernelSize = kernelSize;
    stage.sigma = sigma;
    return stage;
}

StageConfig makeMedianStage(int kernelSize)
{
    StageConfig stage;
    stage.type = StageType::MedianFilter;
    stage.kernelSize = kernelSize;
    return stage;
}

StageConfig makeBilateralStage(int diameter, double sigmaColor, double sigmaSpace)
{
    StageConfig stage;
    stage.type = StageType::BilateralFilter;
    stage.diameter = diameter;
    stage.sigmaColor = sigmaColor;
    stage.sigmaSpace = sigmaSpace;
    return stage;
}

StageConfig makeClaheStage(double clipLimit, cv::Size tileGridSize)
{
    StageConfig stage;
    stage.type = StageType::CLAHE;
    stage.clipLimit = clipLimit;
    stage.tileGridSize = tileGridSize;
    return stage;
}

StageConfig makeUnsharpStage(int kernelSize, double sigma, double amount)
{
    StageConfig stage;
    stage.type = StageType::UnsharpMask;
    stage.kernelSize = kernelSize;
    stage.sigma = sigma;
    stage.amount = amount;
    return stage;
}

StageConfig makeLaplacianStage(double amount)
{
    StageConfig stage;
    stage.type = StageType::LaplacianSharpen;
    stage.amount = amount;
    return stage;
}

DegradationConfig makeGaussianNoise(double stddev)
{
    DegradationConfig cfg;
    cfg.type = DegradationType::GaussianNoise;
    cfg.primaryValue = stddev;
    return cfg;
}

DegradationConfig makeSaltPepper(double saltProb, double pepperProb)
{
    DegradationConfig cfg;
    cfg.type = DegradationType::SaltPepperNoise;
    cfg.primaryValue = saltProb;
    cfg.secondaryValue = pepperProb;
    return cfg;
}

DegradationConfig makeLowContrastGaussian(double alpha, double noiseStddev)
{
    DegradationConfig cfg;
    cfg.type = DegradationType::LowContrastGaussianNoise;
    cfg.primaryValue = alpha;
    cfg.secondaryValue = noiseStddev;
    return cfg;
}

DegradationConfig makeMotionBlur(int kernelSize, double angleDegrees)
{
    DegradationConfig cfg;
    cfg.type = DegradationType::MotionBlur;
    cfg.kernelSize = kernelSize;
    cfg.angleDegrees = angleDegrees;
    return cfg;
}

ExperimentConfig makeExperiment(const std::string& family,
                                const std::string& variantName,
                                const std::string& description,
                                const DegradationConfig& degradation,
                                const std::vector<StageConfig>& stages)
{
    ExperimentConfig cfg;
    cfg.family = family;
    cfg.variantName = variantName;
    cfg.description = description;
    cfg.outputDir = family + "/" + variantName;
    cfg.degradation = degradation;
    cfg.stages = stages;
    return cfg;
}

} // namespace

std::vector<ExperimentConfig> getAllExperiments()
{
    std::vector<ExperimentConfig> experiments;

    // Pipeline A
    experiments.push_back(
        makeExperiment(
            "pipelineA",
            "A1",
            "Gaussian noise -> Bilateral filter -> Unsharp mask",
            makeGaussianNoise(20.0),
            {
                makeBilateralStage(7, 35.0, 35.0),
                makeUnsharpStage(5, 1.0, 0.4)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineA",
            "A2",
            "Gaussian noise -> Bilateral filter -> Unsharp mask",
            makeGaussianNoise(25.0),
            {
                makeBilateralStage(9, 50.0, 50.0),
                makeUnsharpStage(5, 1.0, 0.5)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineA",
            "A3",
            "Gaussian noise -> Bilateral filter -> Unsharp mask",
            makeGaussianNoise(30.0),
            {
                makeBilateralStage(9, 60.0, 60.0),
                makeUnsharpStage(5, 1.0, 0.35)
            }));

    // Pipeline B
    experiments.push_back(
        makeExperiment(
            "pipelineB",
            "B1",
            "Salt-and-pepper noise -> Median filter -> Unsharp mask",
            makeSaltPepper(0.02, 0.02),
            {
                makeMedianStage(3),
                makeUnsharpStage(5, 1.0, 0.3)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineB",
            "B2",
            "Salt-and-pepper noise -> Median filter -> Unsharp mask",
            makeSaltPepper(0.04, 0.04),
            {
                makeMedianStage(5),
                makeUnsharpStage(5, 1.0, 0.35)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineB",
            "B3",
            "Salt-and-pepper noise -> Median filter -> Unsharp mask",
            makeSaltPepper(0.06, 0.06),
            {
                makeMedianStage(5),
                makeUnsharpStage(5, 1.0, 0.25)
            }));

    // Pipeline C
    experiments.push_back(
        makeExperiment(
            "pipelineC",
            "C1",
            "Salt-and-pepper noise -> Gaussian filter baseline",
            makeSaltPepper(0.04, 0.04),
            {
                makeGaussianStage(5, 1.2)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineC",
            "C2",
            "Salt-and-pepper noise -> Gaussian filter baseline",
            makeSaltPepper(0.06, 0.06),
            {
                makeGaussianStage(7, 1.8)
            }));

    // Pipeline D
    experiments.push_back(
        makeExperiment(
            "pipelineD",
            "D1",
            "Low contrast + Gaussian noise -> Bilateral filter -> CLAHE",
            makeLowContrastGaussian(0.75, 15.0),
            {
                makeBilateralStage(7, 35.0, 35.0),
                makeClaheStage(2.0, cv::Size(8, 8))
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineD",
            "D2",
            "Low contrast + Gaussian noise -> Bilateral filter -> CLAHE",
            makeLowContrastGaussian(0.65, 20.0),
            {
                makeBilateralStage(9, 45.0, 45.0),
                makeClaheStage(2.5, cv::Size(8, 8))
            }));

    // Pipeline E
    experiments.push_back(
        makeExperiment(
            "pipelineE",
            "E1",
            "Motion blur -> Unsharp mask -> Laplacian sharpening",
            makeMotionBlur(9, 15.0),
            {
                makeUnsharpStage(5, 1.0, 0.6),
                makeLaplacianStage(0.2)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineE",
            "E2",
            "Motion blur -> Unsharp mask -> Laplacian sharpening",
            makeMotionBlur(11, 20.0),
            {
                makeUnsharpStage(5, 1.0, 0.9),
                makeLaplacianStage(0.3)
            }));

    // Pipeline F: Gaussian comparison 

    experiments.push_back(
        makeExperiment(
            "pipelineF",
            "F1_opencv_gaussian",
            "Gaussian noise -> OpenCV Gaussian filter",
            makeGaussianNoise(25.0),
            {
                makeGaussianStage(5, 1.2)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineF",
            "F2_manual_gaussian",
            "Gaussian noise -> Manual Gaussian filter",
            makeGaussianNoise(25.0),
            {
                StageConfig{
                    StageType::ManualGaussianFilter,
                    5,
                    1.2
                }
            }));

    // Pipeline G: Median comparison 

    experiments.push_back(
        makeExperiment(
            "pipelineG",
            "G1_opencv_median",
            "Salt & pepper -> OpenCV median filter",
            makeSaltPepper(0.04, 0.04),
            {
                makeMedianStage(5)
            }));

    experiments.push_back(
        makeExperiment(
            "pipelineG",
            "G2_manual_median",
            "Salt & pepper -> Manual median filter",
            makeSaltPepper(0.04, 0.04),
            {
                StageConfig{
                    StageType::ManualMedianFilter,
                    5
                }
            }));

    return experiments;
}

} // namespace irp