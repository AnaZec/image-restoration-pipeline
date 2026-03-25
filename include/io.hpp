#ifndef IO_HPP
#define IO_HPP

#include <opencv2/opencv.hpp>
#include <string>

namespace io {
    std::string inputPath(const std::string& filename);
    std::string outputPath(const std::string& filename);
    std::string outputPath(const std::string& relativeDir, const std::string& filename);

    cv::Mat loadInputImage(const std::string& filename);

    bool ensureOutputDir(const std::string& relativeDir);

    bool saveOutputImage(const std::string& filename, const cv::Mat& image);
    bool saveOutputImageToDir(const std::string& relativeDir,
                              const std::string& filename,
                              const cv::Mat& image);

    bool saveTextFile(const std::string& filename, const std::string& content);
    bool saveTextFileToDir(const std::string& relativeDir,
                           const std::string& filename,
                           const std::string& content);
}

#endif