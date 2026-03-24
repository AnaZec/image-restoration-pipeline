#include <iostream>
#include <sstream>

#include "io.hpp"
#include "degradation.hpp"
#include "filters.hpp"
#include "metrics.hpp"
#include "timer.hpp"

int main() {
    try {
        cv::Mat original = io::loadInputImage("test.jpg");
        if (original.empty()) {
            return 1;
        }

        // -----------------------------
        // Pipeline A: Gaussian noise -> Gaussian filter
        // -----------------------------
        cv::Mat gaussianNoisy = addGaussianNoise(original, 0.0, 25.0);

        Timer timerA;
        timerA.start();
        cv::Mat pipelineAResult = applyGaussianFilter(gaussianNoisy, 5, 1.0);
        timerA.stop();

        double mseA = computeMSE(original, pipelineAResult);
        double psnrA = computePSNR(original, pipelineAResult);
        double timeA = timerA.elapsedMilliseconds();

        // Save Pipeline A outputs
        io::saveOutputImageToFolder("pipelineA", "00_original.jpg", original);
        io::saveOutputImageToFolder("pipelineA", "01_degraded_gaussian_noise.jpg", gaussianNoisy);
        io::saveOutputImageToFolder("pipelineA", "02_gaussian_filter.jpg", pipelineAResult);
        io::saveOutputImageToFolder("pipelineA", "03_final_result.jpg", pipelineAResult);

        // -----------------------------
        // Pipeline B: Salt & pepper noise -> Median filter
        // -----------------------------
        cv::Mat spNoisy = addSaltAndPepperNoise(original, 0.02, 0.02);

        Timer timerB;
        timerB.start();
        cv::Mat pipelineBResult = applyMedianFilter(spNoisy, 5);
        timerB.stop();

        double mseB = computeMSE(original, pipelineBResult);
        double psnrB = computePSNR(original, pipelineBResult);
        double timeB = timerB.elapsedMilliseconds();

        // Save Pipeline B outputs
        io::saveOutputImageToFolder("pipelineB", "00_original.jpg", original);
        io::saveOutputImageToFolder("pipelineB", "01_degraded_salt_pepper.jpg", spNoisy);
        io::saveOutputImageToFolder("pipelineB", "02_median_filter.jpg", pipelineBResult);
        io::saveOutputImageToFolder("pipelineB", "03_final_result.jpg", pipelineBResult);

        // -----------------------------
        // Pipeline C: Salt & pepper noise -> Gaussian filter
        // -----------------------------
        Timer timerC;
        timerC.start();
        cv::Mat pipelineCResult = applyGaussianFilter(spNoisy, 5, 1.0);
        timerC.stop();

        double mseC = computeMSE(original, pipelineCResult);
        double psnrC = computePSNR(original, pipelineCResult);
        double timeC = timerC.elapsedMilliseconds();

        // Save Pipeline C outputs
        io::saveOutputImageToFolder("pipelineC", "00_original.jpg", original);
        io::saveOutputImageToFolder("pipelineC", "01_degraded_salt_pepper.jpg", spNoisy);
        io::saveOutputImageToFolder("pipelineC", "02_gaussian_filter.jpg", pipelineCResult);
        io::saveOutputImageToFolder("pipelineC", "03_final_result.jpg", pipelineCResult);

        // -----------------------------
        // Generate overall report
        // -----------------------------
        std::ostringstream report;
        report << "Image Restoration Report\n";
        report << "========================\n\n";
        report << "Input image: test.jpg\n\n";

        report << "Pipeline A: Gaussian noise -> Gaussian filter\n";
        report << "Noise parameters: mean = 0.0, stddev = 25.0\n";
        report << "Filter parameters: kernelSize = 5, sigma = 1.0\n";
        report << "MSE: " << mseA << "\n";
        report << "PSNR: " << psnrA << " dB\n";
        report << "Runtime: " << timeA << " ms\n\n";

        report << "Pipeline B: Salt & pepper noise -> Median filter\n";
        report << "Noise parameters: saltProb = 0.02, pepperProb = 0.02\n";
        report << "Filter parameters: kernelSize = 5\n";
        report << "MSE: " << mseB << "\n";
        report << "PSNR: " << psnrB << " dB\n";
        report << "Runtime: " << timeB << " ms\n\n";

        report << "Pipeline C: Salt & pepper noise -> Gaussian filter\n";
        report << "Noise parameters: saltProb = 0.02, pepperProb = 0.02\n";
        report << "Filter parameters: kernelSize = 5, sigma = 1.0\n";
        report << "MSE: " << mseC << "\n";
        report << "PSNR: " << psnrC << " dB\n";
        report << "Runtime: " << timeC << " ms\n";

        if (!io::saveTextFile("results.txt", report.str())) {
            std::cerr << "Failed to save report.\n";
            return 1;
        }

        std::cout << "Processing completed successfully.\n\n";
        std::cout << report.str();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}