#ifndef IO_HPP
#define IO_HPP

#include <opencv2/opencv.hpp>
#include <string>

namespace io {
    std::string inputPath(const std::string& filename);
    std::string outputPath(const std::string& filename);
    std::string outputPath(const std::string& folder, const std::string& filename);

    cv::Mat loadInputImage(const std::string& filename);

    bool ensureOutputFolder(const std::string& folder);
    bool saveOutputImage(const std::string& filename, const cv::Mat& image);
    bool saveOutputImageToFolder(const std::string& folder,
                                 const std::string& filename,
                                 const cv::Mat& image);
    bool saveTextFile(const std::string& filename, const std::string& content);
}

#endif