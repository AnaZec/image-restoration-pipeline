#include "io.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <exception>

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

std::string outputPath(const std::string& relativeDir, const std::string& filename) {
    return OUTPUT_DIR + relativeDir + "/" + filename;
}

cv::Mat loadInputImage(const std::string& imagePath) {
    cv::Mat image = cv::imread(imagePath);

    if (image.empty()) {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
    }

    return image;
}

bool ensureOutputDir(const std::string& relativeDir) {
    try {
        fs::create_directories(OUTPUT_DIR + relativeDir);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create output directory: "
                  << OUTPUT_DIR + relativeDir
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

bool saveOutputImageToDir(const std::string& relativeDir,
                          const std::string& filename,
                          const cv::Mat& image) {
    if (!ensureOutputDir(relativeDir)) {
        return false;
    }

    const std::string path = outputPath(relativeDir, filename);
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

bool saveTextFileToDir(const std::string& relativeDir,
                       const std::string& filename,
                       const std::string& content) {
    if (!ensureOutputDir(relativeDir)) {
        return false;
    }

    const std::string path = outputPath(relativeDir, filename);
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to save text file: " << path << '\n';
        return false;
    }

    file << content;
    return true;
}

} // namespace io