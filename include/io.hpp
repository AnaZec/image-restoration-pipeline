#ifndef IO_HPP
#define IO_HPP

#include <opencv2/opencv.hpp>
#include <string>

namespace io {

std::string inputPath(const std::string& filename);
std::string outputPath(const std::string& path);
std::string outputPath(const std::string& dir, const std::string& filename);

cv::Mat loadInputImage(const std::string& imagePath);

bool ensureOutputDir(const std::string& dir);

bool saveOutputImage(const std::string& path, const cv::Mat& image);
bool saveOutputImageToDir(const std::string& dir,
                          const std::string& filename,
                          const cv::Mat& image);

bool saveTextFile(const std::string& path, const std::string& content);
bool saveTextFileToDir(const std::string& dir,
                       const std::string& filename,
                       const std::string& content);

} // namespace io

#endif