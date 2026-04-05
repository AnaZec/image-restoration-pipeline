#include "pipeline.hpp"

#include "degradation.hpp"
#include "filters.hpp"
#include "manual_filters.hpp"

#include <stdexcept>

namespace irp
{

cv::Mat applyDegradation(const cv::Mat& image, const DegradationConfig& cfg)
{
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    switch (cfg.type) {
        case DegradationType::GaussianNoise:
            return addGaussianNoise(image, 0.0, cfg.primaryValue);

        case DegradationType::SaltPepperNoise:
            return addSaltAndPepperNoise(image, cfg.primaryValue, cfg.secondaryValue);

        case DegradationType::LowContrastGaussianNoise: {
            cv::Mat reduced = contrastReduction(image, cfg.primaryValue, 0.0);
            return addGaussianNoise(reduced, 0.0, cfg.secondaryValue);
        }

        case DegradationType::MotionBlur:
            return motionBlur(image, cfg.kernelSize, cfg.angleDegrees);
    }

    throw std::runtime_error("Unsupported degradation type");
}

cv::Mat applyStage(const cv::Mat& image, const StageConfig& cfg)
{
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    switch (cfg.type) {
        case StageType::GaussianFilter:
            return applyGaussianFilter(image, cfg.kernelSize, cfg.sigma);

        case StageType::MedianFilter:
            return applyMedianFilter(image, cfg.kernelSize);

        case StageType::BilateralFilter:
            return applyBilateralFilter(image, cfg.diameter, cfg.sigmaColor, cfg.sigmaSpace);

        case StageType::CLAHE:
            return applyCLAHE(image, cfg.clipLimit, cfg.tileGridSize);

        case StageType::UnsharpMask:
            return applyUnsharpMask(image, cfg.kernelSize, cfg.sigma, cfg.amount);

        case StageType::LaplacianSharpen:
            return applyLaplacianSharpening(image, cfg.amount);

        case StageType::ManualGaussianFilter:
            return applyManualGaussianBlur(image, cfg.kernelSize, cfg.sigma);
        
        case StageType::ManualMedianFilter:
            return applyManualMedianFilter(image, cfg.kernelSize);
        
        
        
    }

    throw std::runtime_error("Unsupported stage type");
}

std::string degradationTypeToString(DegradationType type)
{
    switch (type) {
        case DegradationType::GaussianNoise:
            return "gaussian_noise";
        case DegradationType::SaltPepperNoise:
            return "salt_and_pepper_noise";
        case DegradationType::LowContrastGaussianNoise:
            return "low_contrast_gaussian_noise";
        case DegradationType::MotionBlur:
            return "motion_blur";
    }

    return "unknown_degradation";
}

std::string stageTypeToString(StageType type)
{
    switch (type) {
        case StageType::GaussianFilter:
            return "Gaussian filter";
        case StageType::MedianFilter:
            return "Median filter";
        case StageType::BilateralFilter:
            return "Bilateral filter";
        case StageType::CLAHE:
            return "CLAHE";
        case StageType::UnsharpMask:
            return "Unsharp mask";
        case StageType::LaplacianSharpen:
            return "Laplacian sharpening";
        case StageType::ManualGaussianFilter:
            return "Manual Gaussian filter";
        case StageType::ManualMedianFilter:
            return "Manual Median filter";
    }

    return "Unknown stage";
}

std::string stageTypeToFileLabel(StageType type)
{
    switch (type) {
        case StageType::GaussianFilter:
            return "gaussian_filter";
        case StageType::MedianFilter:
            return "median_filter";
        case StageType::BilateralFilter:
            return "bilateral_filter";
        case StageType::CLAHE:
            return "clahe";
        case StageType::UnsharpMask:
            return "unsharp_mask";
        case StageType::LaplacianSharpen:
            return "laplacian_sharpen";
        case StageType::ManualGaussianFilter:
            return "manual_gaussian_filter";
        case StageType::ManualMedianFilter:
            return "manual_median_filter";
    }

    return "unknown_stage";
}

} // namespace irp