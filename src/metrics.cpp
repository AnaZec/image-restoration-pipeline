#include "metrics.hpp"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <cmath>
#include <limits>

double computeMSE(const cv::Mat& original, const cv::Mat& processed) {
    if (original.empty() || processed.empty()) {
        throw std::invalid_argument("One or both images are empty");
    }

    if (original.size() != processed.size() || original.type() != processed.type()) {
        throw std::invalid_argument("Images must have the same size and type");
    }

    cv::Mat diff;
    cv::absdiff(original, processed, diff);

    diff.convertTo(diff, CV_32F);
    diff = diff.mul(diff);

    cv::Scalar sumSq = cv::sum(diff);

    double sse = 0.0;
    for (int i = 0; i < diff.channels(); ++i) {
        sse += sumSq[i];
    }

    double totalValues =
        static_cast<double>(original.rows) *
        static_cast<double>(original.cols) *
        static_cast<double>(original.channels());

    return sse / totalValues;
}

double computePSNR(const cv::Mat& original, const cv::Mat& processed) {
    double mse = computeMSE(original, processed);

    if (mse == 0.0) {
        return std::numeric_limits<double>::infinity();
    }

    const double maxPixelValue = 255.0;
    return 10.0 * std::log10((maxPixelValue * maxPixelValue) / mse);
}