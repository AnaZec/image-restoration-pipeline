#include "manual_filters.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <vector>

namespace irp
{
namespace
{

int reflectIndex(int index, int limit)
{
    if (limit <= 1) {
        return 0;
    }

    while (index < 0 || index >= limit) {
        if (index < 0) {
            index = -index - 1;
        } else {
            index = 2 * limit - index - 1;
        }
    }

    return index;
}

cv::Mat buildGaussianKernel1D(int kernelSize, double sigma)
{
    if (kernelSize <= 0 || kernelSize % 2 == 0) {
        throw std::invalid_argument("Gaussian kernel size must be positive and odd");
    }

    if (sigma <= 0.0) {
        throw std::invalid_argument("Gaussian sigma must be > 0");
    }

    cv::Mat kernel(1, kernelSize, CV_64F);

    const int radius = kernelSize / 2;
    const double sigma2 = sigma * sigma;

    double sum = 0.0;

    for (int x = -radius; x <= radius; ++x) {
        const double exponent = -static_cast<double>(x * x) / (2.0 * sigma2);
        const double value = std::exp(exponent);

        kernel.at<double>(0, x + radius) = value;
        sum += value;
    }

    kernel /= sum;
    return kernel;
}

cv::Mat applyManualGaussianBlurSingleChannel(const cv::Mat& image, int kernelSize, double sigma)
{
    const cv::Mat kernel = buildGaussianKernel1D(kernelSize, sigma);

    const int radius = kernelSize / 2;

    cv::Mat horizontal(image.size(), CV_64F);
    cv::Mat output(image.size(), CV_8U);

    // First pass: horizontal 1D convolution.
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            double acc = 0.0;

            for (int kx = -radius; kx <= radius; ++kx) {
                const int srcCol = reflectIndex(col + kx, image.cols);

                const double pixel =
                    static_cast<double>(image.at<uchar>(row, srcCol));
                const double weight =
                    kernel.at<double>(0, kx + radius);

                acc += pixel * weight;
            }

            horizontal.at<double>(row, col) = acc;
        }
    }

    // Second pass: vertical 1D convolution.
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            double acc = 0.0;

            for (int ky = -radius; ky <= radius; ++ky) {
                const int srcRow = reflectIndex(row + ky, image.rows);

                const double pixel = horizontal.at<double>(srcRow, col);
                const double weight =
                    kernel.at<double>(0, ky + radius);

                acc += pixel * weight;
            }

            output.at<uchar>(row, col) = cv::saturate_cast<uchar>(std::round(acc));
        }
    }

    return output;
}

cv::Mat applyManualMedianFilterSingleChannel(const cv::Mat& image, int kernelSize)
{
    if (kernelSize <= 0 || kernelSize % 2 == 0) {
        throw std::invalid_argument("Median kernel size must be positive and odd");
    }

    cv::Mat output(image.size(), CV_8U);

    const int radius = kernelSize / 2;
    std::vector<uchar> window;
    window.reserve(kernelSize * kernelSize);

    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            window.clear();

            for (int ky = -radius; ky <= radius; ++ky) {
                for (int kx = -radius; kx <= radius; ++kx) {
                    const int srcRow = reflectIndex(row + ky, image.rows);
                    const int srcCol = reflectIndex(col + kx, image.cols);

                    window.push_back(image.at<uchar>(srcRow, srcCol));
                }
            }

            const auto midIt = window.begin() + static_cast<std::ptrdiff_t>(window.size() / 2);
            std::nth_element(window.begin(), midIt, window.end());
            output.at<uchar>(row, col) = *midIt;
        }
    }

    return output;
}

cv::Mat processPerChannel(const cv::Mat& image,
                          const std::function<cv::Mat(const cv::Mat&)>& processor)
{
    if (image.channels() == 1) {
        return processor(image);
    }

    if (image.channels() == 3) {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);

        for (auto& channel : channels) {
            channel = processor(channel);
        }

        cv::Mat merged;
        cv::merge(channels, merged);
        return merged;
    }

    throw std::invalid_argument("Only 1-channel and 3-channel images are supported");
}

} // namespace

cv::Mat applyManualGaussianBlur(const cv::Mat& image, int kernelSize, double sigma)
{
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    if (image.depth() != CV_8U) {
        throw std::invalid_argument("Manual Gaussian blur expects CV_8U image");
    }

    return processPerChannel(
        image,
        [&](const cv::Mat& singleChannel) {
            return applyManualGaussianBlurSingleChannel(singleChannel, kernelSize, sigma);
        });
}

cv::Mat applyManualMedianFilter(const cv::Mat& image, int kernelSize)
{
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    if (image.depth() != CV_8U) {
        throw std::invalid_argument("Manual median filter expects CV_8U image");
    }

    return processPerChannel(
        image,
        [&](const cv::Mat& singleChannel) {
            return applyManualMedianFilterSingleChannel(singleChannel, kernelSize);
        });
}

} // namespace irp