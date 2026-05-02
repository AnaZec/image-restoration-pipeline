#include "io.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace {
    const std::string INPUT_DIR = "../images/input/";
}

namespace io {

std::string inputPath(const std::string& filename) {
    return (fs::path(INPUT_DIR) / filename).string();
}

std::string outputPath(const std::string& filename) {
    return filename;
}

std::string outputPath(const std::string& relativeDir, const std::string& filename) {
    return (fs::path(relativeDir) / filename).string();
}

cv::Mat loadInputImage(const std::string& imagePath) {
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cerr << "Failed to load image: " << imagePath << '\n';
    }

    return image;
}

bool ensureOutputDir(const std::string& dir) {
    try {
        fs::create_directories(dir);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create output directory: "
                  << dir
                  << " | " << e.what() << '\n';
        return false;
    }
}

bool saveOutputImage(const std::string& path, const cv::Mat& image) {
    const fs::path filePath(path);

    if (filePath.has_parent_path() &&
        !ensureOutputDir(filePath.parent_path().string())) {
        return false;
    }

    const bool ok = cv::imwrite(path, image);

    if (!ok) {
        std::cerr << "Failed to save image: " << path << '\n';
    }

    return ok;
}

bool saveOutputImageToDir(const std::string& dir,
                          const std::string& filename,
                          const cv::Mat& image) {
    if (!ensureOutputDir(dir)) {
        return false;
    }

    const std::string path = outputPath(dir, filename);
    const bool ok = cv::imwrite(path, image);

    if (!ok) {
        std::cerr << "Failed to save image: " << path << '\n';
    }

    return ok;
}

bool saveTextFile(const std::string& path, const std::string& content) {
    const fs::path filePath(path);

    if (filePath.has_parent_path() &&
        !ensureOutputDir(filePath.parent_path().string())) {
        return false;
    }

    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to save text file: " << path << '\n';
        return false;
    }

    file << content;
    return true;
}

bool saveTextFileToDir(const std::string& dir,
                       const std::string& filename,
                       const std::string& content) {
    if (!ensureOutputDir(dir)) {
        return false;
    }

    const std::string path = outputPath(dir, filename);
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to save text file: " << path << '\n';
        return false;
    }

    file << content;
    return true;
}

} // namespace io