#include "experiment_registry.hpp"
#include "experiment_runner.hpp"
#include "io.hpp"
#include "reporting.hpp"

#include <exception>
#include <iostream>
#include <utility>
#include <vector>

int main()
{
    try {
        const std::string inputFile = "test.jpg";
        const std::string outputRoot = "output";

        cv::Mat original = io::loadInputImage(inputFile);
        if (original.empty()) {
            return 1;
        }

        const std::vector<irp::ExperimentConfig> experiments = irp::getAllExperiments();

        std::vector<irp::ExperimentResult> results;
        results.reserve(experiments.size());

        for (const auto& experiment : experiments) {
            std::cout << "[INFO] Running "
                      << experiment.family << " / "
                      << experiment.variantName << "...\n";

            irp::ExperimentResult result = irp::runExperiment(original, experiment);
            irp::saveExperimentOutputs(result, outputRoot);
            results.push_back(std::move(result));
        }

        io::saveTextFile(
            outputRoot + "/output_report.txt",
            irp::buildGlobalReport(results, inputFile)
        );

        io::saveTextFile(
            outputRoot + "/summary.csv",
            irp::buildCSVReport(results)
        );

        std::cout << "[INFO] Finished.\n";
        std::cout << "[INFO] Report: " << outputRoot + "/output_report.txt" << "\n";
        std::cout << "[INFO] CSV: " << outputRoot + "/summary.csv" << "\n";

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "[ERROR] " << ex.what() << "\n";
        return 1;
    }
}