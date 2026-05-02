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

    cv::Mat kernel(1, kernelSize, CV_32F);

    const int radius = kernelSize / 2;
    const double sigma2 = sigma * sigma;

    float sum = 0.0F;

    for (int x = -radius; x <= radius; ++x) {
        const double exponent = -static_cast<double>(x * x) / (2.0 * sigma2);
        const float value = static_cast<float>(std::exp(exponent));

        kernel.at<float>(0, x + radius) = value;
        sum += value;
    }

    kernel /= sum;
    return kernel;
}

cv::Mat applyManualGaussianBlurSingleChannel(const cv::Mat& image, int kernelSize, double sigma)
{
    const cv::Mat kernel = buildGaussianKernel1D(kernelSize, sigma);

    const int radius = kernelSize / 2;

    cv::Mat horizontal(image.size(), CV_32F);
    cv::Mat output(image.size(), CV_8U);

    cv::Mat paddedForHorizontal;
    cv::copyMakeBorder(
        image,
        paddedForHorizontal,
        0,
        0,
        radius,
        radius,
        cv::BORDER_REFLECT
    );

    // First pass: horizontal 1D convolution.
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            float acc = 0.0F;

            for (int kx = -radius; kx <= radius; ++kx) {
                const int paddedCol = col + kx + radius;

                const float pixel =
                    static_cast<float>(paddedForHorizontal.at<uchar>(row, paddedCol));
                const float weight =
                    kernel.at<float>(0, kx + radius);

                acc += pixel * weight;
            }

            horizontal.at<float>(row, col) = acc;
        }
    }

    cv::Mat paddedForVertical;
    cv::copyMakeBorder(
        horizontal,
        paddedForVertical,
        radius,
        radius,
        0,
        0,
        cv::BORDER_REFLECT
    );

    // Second pass: vertical 1D convolution.
    for (int row = 0; row < image.rows; ++row) {
        for (int col = 0; col < image.cols; ++col) {
            float acc = 0.0F;

            for (int ky = -radius; ky <= radius; ++ky) {
                const int paddedRow = row + ky + radius;

                const float pixel = paddedForVertical.at<float>(paddedRow, col);
                const float weight =
                    kernel.at<float>(0, ky + radius);

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