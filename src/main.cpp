#include "experiment_registry.hpp"
#include "experiment_runner.hpp"
#include "io.hpp"
#include "reporting.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct CliOptions {
    std::string inputPath;
    std::string outputDir = "output";
};

void printUsage(const std::string& programName) {
    std::cout
        << "Usage:\n"
        << "  " << programName << " --input <image_path> [--output <output_dir>]\n\n"
        << "Options:\n"
        << "  -i, --input    Path to the input image. Required.\n"
        << "  -o, --output   Directory where results will be written. Default: output\n"
        << "  -h, --help     Show this help message.\n\n"
        << "Example:\n"
        << "  " << programName << " --input images/input/test.jpg --output output\n";
}

CliOptions parseArguments(int argc, char* argv[]) {
    CliOptions options;

    if (argc == 1) {
        throw std::invalid_argument("Missing required --input argument.");
    }

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        }

        if (arg == "-i" || arg == "--input") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value after " + arg);
            }
            options.inputPath = argv[++i];
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value after " + arg);
            }
            options.outputDir = argv[++i];
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    if (options.inputPath.empty()) {
        throw std::invalid_argument("Missing required --input argument.");
    }

    if (!std::filesystem::exists(options.inputPath)) {
        throw std::invalid_argument("Input image does not exist: " + options.inputPath);
    }

    if (!std::filesystem::is_regular_file(options.inputPath)) {
        throw std::invalid_argument("Input path is not a regular file: " + options.inputPath);
    }

    return options;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const CliOptions options = parseArguments(argc, argv);

        cv::Mat original = io::loadInputImage(options.inputPath);
        if (original.empty()) {
            std::cerr << "[ERROR] Failed to load input image: " << options.inputPath << "\n";
            return 1;
        }

        std::filesystem::create_directories(options.outputDir);

        const std::vector<irp::ExperimentConfig> experiments = irp::getAllExperiments();
        std::vector<irp::ExperimentResult> results;
        results.reserve(experiments.size());

        for (const auto& experiment : experiments) {
            std::cout << "[INFO] Running "
                      << experiment.family << " / "
                      << experiment.variantName << "...\n";

            irp::ExperimentResult result = irp::runExperiment(original, experiment);
            irp::saveExperimentOutputs(result, options.outputDir);
            results.push_back(std::move(result));
        }

        io::saveTextFile(
            options.outputDir + "/output_report.txt",
            irp::buildGlobalReport(results, options.inputPath)
        );

        io::saveTextFile(
            options.outputDir + "/summary.csv",
            irp::buildCSVReport(results)
        );

        std::cout << "[INFO] Finished.\n";
        std::cout << "[INFO] Report: " << options.outputDir + "/output_report.txt" << "\n";
        std::cout << "[INFO] CSV: " << options.outputDir + "/summary.csv" << "\n";

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "[ERROR] " << ex.what() << "\n\n";
        printUsage(argv[0]);
        return 1;
    }
}