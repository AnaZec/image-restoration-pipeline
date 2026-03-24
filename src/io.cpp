#include "io.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace {
    const std::string INPUT_DIR = "../images/input/";
    const std::string OUTPUT_DIR = "../images/output/";
}

namespace io {

std::string inputPath(const std::string& filename) {
    return INPUT_DIR + filename;
}

std::string outputPath(const std::string& filename) {
    return OUTPUT_DIR + filename;
}

std::string outputPath(const std::string& folder, const std::string& filename) {
    return OUTPUT_DIR + folder + "/" + filename;
}

cv::Mat loadInputImage(const std::string& filename) {
    const std::string path = inputPath(filename);
    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Failed to load image: " << path << '\n';
    }

    return image;
}

bool ensureOutputFolder(const std::string& folder) {
    try {
        fs::create_directories(OUTPUT_DIR + folder);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create folder: " << OUTPUT_DIR + folder
                  << " | " << e.what() << '\n';
        return false;
    }
}

bool saveOutputImage(const std::string& filename, const cv::Mat& image) {
    const std::string path = outputPath(filename);
    const bool ok = cv::imwrite(path, image);

    if (!ok) {
        std::cerr << "Failed to save image: " << path << '\n';
    }

    return ok;
}

bool saveOutputImageToFolder(const std::string& folder,
                             const std::string& filename,
                             const cv::Mat& image) {
    if (!ensureOutputFolder(folder)) {
        return false;
    }

    const std::string path = outputPath(folder, filename);
    const bool ok = cv::imwrite(path, image);

    if (!ok) {
        std::cerr << "Failed to save image: " << path << '\n';
    }

    return ok;
}

bool saveTextFile(const std::string& filename, const std::string& content) {
    const std::string path = outputPath(filename);
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to save text file: " << path << '\n';
        return false;
    }

    file << content;
    return true;
}

}