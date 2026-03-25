#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

#include "io.hpp"
#include "degradation.hpp"
#include "filters.hpp"
#include "metrics.hpp"
#include "timer.hpp"

namespace {

cv::Mat makeComparisonStrip(const std::vector<cv::Mat>& images) {
    if (images.empty()) {
        return cv::Mat();
    }

    std::vector<cv::Mat> resizedImages;
    const int targetHeight = 300;

    for (const auto& img : images) {
        if (img.empty()) {
            continue;
        }

        double scale = static_cast<double>(targetHeight) / img.rows;
        int targetWidth = static_cast<int>(img.cols * scale);

        cv::Mat resized;
        cv::resize(img, resized, cv::Size(targetWidth, targetHeight));
        resizedImages.push_back(resized);
    }

    if (resizedImages.empty()) {
        return cv::Mat();
    }

    cv::Mat strip;
    cv::hconcat(resizedImages, strip);
    return strip;
}

void appendMetrics(std::ostringstream& report,
                   const std::string& title,
                   const std::string& relativeDir,
                   double degradedMSE,
                   double degradedPSNR,
                   double finalMSE,
                   double finalPSNR,
                   double runtimeMs) {
    report << title << " [" << relativeDir << "]\n";
    report << "Degraded MSE: " << degradedMSE << "\n";
    report << "Degraded PSNR: " << degradedPSNR << " dB\n";
    report << "Final MSE: " << finalMSE << "\n";
    report << "Final PSNR: " << finalPSNR << " dB\n";
    report << "PSNR Improvement: " << (finalPSNR - degradedPSNR) << " dB\n";
    report << "Runtime: " << runtimeMs << " ms\n\n";
}

struct PipelineAConfig {
    std::string name;
    double noiseStddev;
    int bilateralDiameter;
    double sigmaColor;
    double sigmaSpace;
    int unsharpKernel;
    double unsharpSigma;
    double unsharpAmount;
};

struct PipelineBConfig {
    std::string name;
    double saltProb;
    double pepperProb;
    int medianKernel;
    int unsharpKernel;
    double unsharpSigma;
    double unsharpAmount;
};

struct PipelineCConfig {
    std::string name;
    double saltProb;
    double pepperProb;
    int gaussianKernel;
    double gaussianSigma;
};

struct PipelineDConfig {
    std::string name;
    double contrastAlpha;
    double noiseStddev;
    int bilateralDiameter;
    double sigmaColor;
    double sigmaSpace;
    double claheClipLimit;
    cv::Size claheGrid;
};

struct PipelineEConfig {
    std::string name;
    int motionKernel;
    double motionAngle;
    int unsharpKernel;
    double unsharpSigma;
    double unsharpAmount;
    double laplacianAmount;
};

} // namespace

int main() {
    try {
        cv::Mat original = io::loadInputImage("test.jpg");
        if (original.empty()) {
            return 1;
        }

        std::ostringstream globalReport;
        globalReport << std::fixed << std::setprecision(4);
        globalReport << "Parameterized Image Restoration Report\n";
        globalReport << "======================================\n\n";
        globalReport << "Input image: test.jpg\n\n";

        const std::vector<PipelineAConfig> pipelineAVariants = {
            {"A1", 20.0, 7, 35.0, 35.0, 5, 1.0, 0.4},
            {"A2", 25.0, 9, 50.0, 50.0, 5, 1.0, 0.5},
            {"A3", 30.0, 9, 60.0, 60.0, 5, 1.0, 0.35}
        };

        const std::vector<PipelineBConfig> pipelineBVariants = {
            {"B1", 0.02, 0.02, 3, 5, 1.0, 0.3},
            {"B2", 0.04, 0.04, 5, 5, 1.0, 0.35},
            {"B3", 0.06, 0.06, 5, 5, 1.0, 0.25}
        };

        const std::vector<PipelineCConfig> pipelineCVariants = {
            {"C1", 0.04, 0.04, 5, 1.2},
            {"C2", 0.06, 0.06, 7, 1.8}
        };

        const std::vector<PipelineDConfig> pipelineDVariants = {
            {"D1", 0.75, 15.0, 7, 35.0, 35.0, 2.0, cv::Size(8, 8)},
            {"D2", 0.65, 20.0, 9, 45.0, 45.0, 2.5, cv::Size(8, 8)}
        };

        const std::vector<PipelineEConfig> pipelineEVariants = {
            {"E1", 9, 15.0, 5, 1.0, 0.6, 0.2},
            {"E2", 11, 20.0, 5, 1.0, 0.9, 0.3}
        };

        // =====================================================
        // Pipeline A
        // Gaussian noise -> Bilateral filter -> Unsharp mask
        // =====================================================
        for (const auto& cfg : pipelineAVariants) {
            const std::string dir = "pipelineA/" + cfg.name;

            cv::Mat degraded = addGaussianNoise(original, 0.0, cfg.noiseStddev);

            Timer timer;
            timer.start();
            cv::Mat stage1 = applyBilateralFilter(
                degraded,
                cfg.bilateralDiameter,
                cfg.sigmaColor,
                cfg.sigmaSpace
            );
            cv::Mat stage2 = applyUnsharpMask(
                stage1,
                cfg.unsharpKernel,
                cfg.unsharpSigma,
                cfg.unsharpAmount
            );
            timer.stop();

            cv::Mat finalResult = stage2;

            double degradedMSE = computeMSE(original, degraded);
            double degradedPSNR = computePSNR(original, degraded);
            double finalMSE = computeMSE(original, finalResult);
            double finalPSNR = computePSNR(original, finalResult);

            io::saveOutputImageToDir(dir, "00_original.jpg", original);
            io::saveOutputImageToDir(dir, "01_degraded_gaussian_noise.jpg", degraded);
            io::saveOutputImageToDir(dir, "02_bilateral_filter.jpg", stage1);
            io::saveOutputImageToDir(dir, "03_unsharp_mask.jpg", stage2);
            io::saveOutputImageToDir(dir, "04_final_result.jpg", finalResult);
            io::saveOutputImageToDir(
                dir,
                "99_comparison_strip.jpg",
                makeComparisonStrip({original, degraded, stage1, finalResult})
            );

            std::ostringstream local;
            local << std::fixed << std::setprecision(4);
            local << "Pipeline A (" << cfg.name << ")\n";
            local << "Gaussian noise -> Bilateral filter -> Unsharp mask\n";
            local << "noiseStddev=" << cfg.noiseStddev
                  << ", bilateralDiameter=" << cfg.bilateralDiameter
                  << ", sigmaColor=" << cfg.sigmaColor
                  << ", sigmaSpace=" << cfg.sigmaSpace
                  << ", unsharpAmount=" << cfg.unsharpAmount << "\n\n";
            local << "Degraded MSE: " << degradedMSE << "\n";
            local << "Degraded PSNR: " << degradedPSNR << " dB\n";
            local << "Final MSE: " << finalMSE << "\n";
            local << "Final PSNR: " << finalPSNR << " dB\n";
            io::saveTextFileToDir(dir, "report.txt", local.str());

            appendMetrics(
                globalReport,
                "Pipeline A: Gaussian noise -> Bilateral filter -> Unsharp mask",
                dir,
                degradedMSE,
                degradedPSNR,
                finalMSE,
                finalPSNR,
                timer.elapsedMilliseconds()
            );
        }

        // =====================================================
        // Pipeline B
        // Salt & pepper -> Median filter -> Unsharp mask
        // =====================================================
        for (const auto& cfg : pipelineBVariants) {
            const std::string dir = "pipelineB/" + cfg.name;

            cv::Mat degraded = addSaltAndPepperNoise(original, cfg.saltProb, cfg.pepperProb);

            Timer timer;
            timer.start();
            cv::Mat stage1 = applyMedianFilter(degraded, cfg.medianKernel);
            cv::Mat stage2 = applyUnsharpMask(
                stage1,
                cfg.unsharpKernel,
                cfg.unsharpSigma,
                cfg.unsharpAmount
            );
            timer.stop();

            cv::Mat finalResult = stage2;

            double degradedMSE = computeMSE(original, degraded);
            double degradedPSNR = computePSNR(original, degraded);
            double finalMSE = computeMSE(original, finalResult);
            double finalPSNR = computePSNR(original, finalResult);

            io::saveOutputImageToDir(dir, "00_original.jpg", original);
            io::saveOutputImageToDir(dir, "01_degraded_salt_pepper.jpg", degraded);
            io::saveOutputImageToDir(dir, "02_median_filter.jpg", stage1);
            io::saveOutputImageToDir(dir, "03_unsharp_mask.jpg", stage2);
            io::saveOutputImageToDir(dir, "04_final_result.jpg", finalResult);
            io::saveOutputImageToDir(
                dir,
                "99_comparison_strip.jpg",
                makeComparisonStrip({original, degraded, stage1, finalResult})
            );

            std::ostringstream local;
            local << std::fixed << std::setprecision(4);
            local << "Pipeline B (" << cfg.name << ")\n";
            local << "Salt & pepper -> Median filter -> Unsharp mask\n";
            local << "saltProb=" << cfg.saltProb
                  << ", pepperProb=" << cfg.pepperProb
                  << ", medianKernel=" << cfg.medianKernel
                  << ", unsharpAmount=" << cfg.unsharpAmount << "\n\n";
            local << "Degraded MSE: " << degradedMSE << "\n";
            local << "Degraded PSNR: " << degradedPSNR << " dB\n";
            local << "Final MSE: " << finalMSE << "\n";
            local << "Final PSNR: " << finalPSNR << " dB\n";
            io::saveTextFileToDir(dir, "report.txt", local.str());

            appendMetrics(
                globalReport,
                "Pipeline B: Salt & pepper -> Median filter -> Unsharp mask",
                dir,
                degradedMSE,
                degradedPSNR,
                finalMSE,
                finalPSNR,
                timer.elapsedMilliseconds()
            );
        }

        // =====================================================
        // Pipeline C
        // Salt & pepper -> Gaussian filter baseline
        // =====================================================
        for (const auto& cfg : pipelineCVariants) {
            const std::string dir = "pipelineC/" + cfg.name;

            cv::Mat degraded = addSaltAndPepperNoise(original, cfg.saltProb, cfg.pepperProb);

            Timer timer;
            timer.start();
            cv::Mat stage1 = applyGaussianFilter(degraded, cfg.gaussianKernel, cfg.gaussianSigma);
            timer.stop();

            cv::Mat finalResult = stage1;

            double degradedMSE = computeMSE(original, degraded);
            double degradedPSNR = computePSNR(original, degraded);
            double finalMSE = computeMSE(original, finalResult);
            double finalPSNR = computePSNR(original, finalResult);

            io::saveOutputImageToDir(dir, "00_original.jpg", original);
            io::saveOutputImageToDir(dir, "01_degraded_salt_pepper.jpg", degraded);
            io::saveOutputImageToDir(dir, "02_gaussian_filter.jpg", stage1);
            io::saveOutputImageToDir(dir, "03_final_result.jpg", finalResult);
            io::saveOutputImageToDir(
                dir,
                "99_comparison_strip.jpg",
                makeComparisonStrip({original, degraded, finalResult})
            );

            std::ostringstream local;
            local << std::fixed << std::setprecision(4);
            local << "Pipeline C (" << cfg.name << ")\n";
            local << "Salt & pepper -> Gaussian filter baseline\n";
            local << "saltProb=" << cfg.saltProb
                  << ", pepperProb=" << cfg.pepperProb
                  << ", gaussianKernel=" << cfg.gaussianKernel
                  << ", gaussianSigma=" << cfg.gaussianSigma << "\n\n";
            local << "Degraded MSE: " << degradedMSE << "\n";
            local << "Degraded PSNR: " << degradedPSNR << " dB\n";
            local << "Final MSE: " << finalMSE << "\n";
            local << "Final PSNR: " << finalPSNR << " dB\n";
            io::saveTextFileToDir(dir, "report.txt", local.str());

            appendMetrics(
                globalReport,
                "Pipeline C: Salt & pepper -> Gaussian filter baseline",
                dir,
                degradedMSE,
                degradedPSNR,
                finalMSE,
                finalPSNR,
                timer.elapsedMilliseconds()
            );
        }

        // =====================================================
        // Pipeline D
        // Contrast reduction + Gaussian noise -> Bilateral -> CLAHE
        // =====================================================
        for (const auto& cfg : pipelineDVariants) {
            const std::string dir = "pipelineD/" + cfg.name;

            cv::Mat lowContrast = contrastReduction(original, cfg.contrastAlpha, 0.0);
            cv::Mat degraded = addGaussianNoise(lowContrast, 0.0, cfg.noiseStddev);

            Timer timer;
            timer.start();
            cv::Mat stage1 = applyBilateralFilter(
                degraded,
                cfg.bilateralDiameter,
                cfg.sigmaColor,
                cfg.sigmaSpace
            );
            cv::Mat stage2 = applyCLAHE(stage1, cfg.claheClipLimit, cfg.claheGrid);
            timer.stop();

            cv::Mat finalResult = stage2;

            double degradedMSE = computeMSE(original, degraded);
            double degradedPSNR = computePSNR(original, degraded);
            double finalMSE = computeMSE(original, finalResult);
            double finalPSNR = computePSNR(original, finalResult);

            io::saveOutputImageToDir(dir, "00_original.jpg", original);
            io::saveOutputImageToDir(dir, "01_low_contrast.jpg", lowContrast);
            io::saveOutputImageToDir(dir, "02_degraded_low_contrast_gaussian_noise.jpg", degraded);
            io::saveOutputImageToDir(dir, "03_bilateral_filter.jpg", stage1);
            io::saveOutputImageToDir(dir, "04_clahe.jpg", stage2);
            io::saveOutputImageToDir(dir, "05_final_result.jpg", finalResult);
            io::saveOutputImageToDir(
                dir,
                "99_comparison_strip.jpg",
                makeComparisonStrip({original, degraded, stage1, finalResult})
            );

            std::ostringstream local;
            local << std::fixed << std::setprecision(4);
            local << "Pipeline D (" << cfg.name << ")\n";
            local << "Contrast reduction + Gaussian noise -> Bilateral -> CLAHE\n";
            local << "contrastAlpha=" << cfg.contrastAlpha
                  << ", noiseStddev=" << cfg.noiseStddev
                  << ", bilateralDiameter=" << cfg.bilateralDiameter
                  << ", claheClipLimit=" << cfg.claheClipLimit << "\n\n";
            local << "Degraded MSE: " << degradedMSE << "\n";
            local << "Degraded PSNR: " << degradedPSNR << " dB\n";
            local << "Final MSE: " << finalMSE << "\n";
            local << "Final PSNR: " << finalPSNR << " dB\n";
            io::saveTextFileToDir(dir, "report.txt", local.str());

            appendMetrics(
                globalReport,
                "Pipeline D: Contrast reduction + Gaussian noise -> Bilateral -> CLAHE",
                dir,
                degradedMSE,
                degradedPSNR,
                finalMSE,
                finalPSNR,
                timer.elapsedMilliseconds()
            );
        }

        // =====================================================
        // Pipeline E
        // Motion blur -> Unsharp -> Laplacian sharpening
        // =====================================================
        for (const auto& cfg : pipelineEVariants) {
            const std::string dir = "pipelineE/" + cfg.name;

            cv::Mat degraded = motionBlur(original, cfg.motionKernel, cfg.motionAngle);

            Timer timer;
            timer.start();
            cv::Mat stage1 = applyUnsharpMask(
                degraded,
                cfg.unsharpKernel,
                cfg.unsharpSigma,
                cfg.unsharpAmount
            );
            cv::Mat stage2 = applyLaplacianSharpening(stage1, cfg.laplacianAmount);
            timer.stop();

            cv::Mat finalResult = stage2;

            double degradedMSE = computeMSE(original, degraded);
            double degradedPSNR = computePSNR(original, degraded);
            double finalMSE = computeMSE(original, finalResult);
            double finalPSNR = computePSNR(original, finalResult);

            io::saveOutputImageToDir(dir, "00_original.jpg", original);
            io::saveOutputImageToDir(dir, "01_degraded_motion_blur.jpg", degraded);
            io::saveOutputImageToDir(dir, "02_unsharp_mask.jpg", stage1);
            io::saveOutputImageToDir(dir, "03_laplacian_sharpening.jpg", stage2);
            io::saveOutputImageToDir(dir, "04_final_result.jpg", finalResult);
            io::saveOutputImageToDir(
                dir,
                "99_comparison_strip.jpg",
                makeComparisonStrip({original, degraded, stage1, finalResult})
            );

            std::ostringstream local;
            local << std::fixed << std::setprecision(4);
            local << "Pipeline E (" << cfg.name << ")\n";
            local << "Motion blur -> Unsharp -> Laplacian sharpening\n";
            local << "motionKernel=" << cfg.motionKernel
                  << ", motionAngle=" << cfg.motionAngle
                  << ", unsharpAmount=" << cfg.unsharpAmount
                  << ", laplacianAmount=" << cfg.laplacianAmount << "\n\n";
            local << "Degraded MSE: " << degradedMSE << "\n";
            local << "Degraded PSNR: " << degradedPSNR << " dB\n";
            local << "Final MSE: " << finalMSE << "\n";
            local << "Final PSNR: " << finalPSNR << " dB\n";
            io::saveTextFileToDir(dir, "report.txt", local.str());

            appendMetrics(
                globalReport,
                "Pipeline E: Motion blur -> Unsharp -> Laplacian sharpening",
                dir,
                degradedMSE,
                degradedPSNR,
                finalMSE,
                finalPSNR,
                timer.elapsedMilliseconds()
            );
        }

        if (!io::saveTextFile("results.txt", globalReport.str())) {
            std::cerr << "Failed to save global report.\n";
            return 1;
        }

        std::cout << "Processing completed successfully.\n\n";
        std::cout << globalReport.str();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}