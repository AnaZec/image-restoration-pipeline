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

double computeSSIM(const cv::Mat& img1, const cv::Mat& img2)
{
    CV_Assert(!img1.empty() && !img2.empty());
    CV_Assert(img1.size() == img2.size());
    CV_Assert(img1.type() == img2.type());

    cv::Mat i1, i2;
    img1.convertTo(i1, CV_32F);
    img2.convertTo(i2, CV_32F);

    cv::Mat mu1, mu2;
    cv::GaussianBlur(i1, mu1, cv::Size(11, 11), 1.5);
    cv::GaussianBlur(i2, mu2, cv::Size(11, 11), 1.5);

    cv::Mat mu1_sq = mu1.mul(mu1);
    cv::Mat mu2_sq = mu2.mul(mu2);
    cv::Mat mu1_mu2 = mu1.mul(mu2);

    cv::Mat sigma1_sq, sigma2_sq, sigma12;

    cv::GaussianBlur(i1.mul(i1), sigma1_sq, cv::Size(11, 11), 1.5);
    sigma1_sq -= mu1_sq;

    cv::GaussianBlur(i2.mul(i2), sigma2_sq, cv::Size(11, 11), 1.5);
    sigma2_sq -= mu2_sq;

    cv::GaussianBlur(i1.mul(i2), sigma12, cv::Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    const double C1 = 6.5025;
    const double C2 = 58.5225;

    cv::Mat t1 = 2 * mu1_mu2 + C1;
    cv::Mat t2 = 2 * sigma12 + C2;
    cv::Mat t3 = t1.mul(t2);

    t1 = mu1_sq + mu2_sq + C1;
    t2 = sigma1_sq + sigma2_sq + C2;
    t1 = t1.mul(t2);

    cv::Mat ssim_map;
    cv::divide(t3, t1, ssim_map);

    return cv::mean(ssim_map)[0];
}