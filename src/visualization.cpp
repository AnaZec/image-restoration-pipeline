#include "visualization.hpp"

namespace irp
{

cv::Mat makeComparisonStrip(const std::vector<cv::Mat>& images, int targetHeight)
{
    if (images.empty()) {
        return cv::Mat();
    }

    std::vector<cv::Mat> resizedImages;
    resizedImages.reserve(images.size());

    for (const auto& img : images) {
        if (img.empty()) {
            continue;
        }

        const double scale = static_cast<double>(targetHeight) / static_cast<double>(img.rows);
        const int targetWidth = static_cast<int>(img.cols * scale);

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

} // namespace irp