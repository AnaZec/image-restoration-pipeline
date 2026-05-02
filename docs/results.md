# Experimental Results & Analysis

## Overview

The framework evaluates multiple restoration pipelines under controlled degradations using:

- MSE (pixel-level error)
- PSNR (signal fidelity)
- SSIM (perceptual similarity)
- Runtime measurements for each restoration pipeline

The goal is to understand how different filters behave under different image degradations and how implementation choices affect quality and performance.

---

## 1. Noise-Specific Behavior

### Gaussian Noise

Strong performing approach:

- Bilateral filtering + sharpening

Observations:

- Gaussian filtering reduces noise but can blur edges and fine texture.
- Bilateral filtering preserves edges better than standard Gaussian smoothing.
- Unsharp masking can restore perceived sharpness after denoising.
- Restoration quality depends on balancing noise reduction against detail preservation.

---

### Salt-and-Pepper Noise

Strong performing approach:

- Median filtering

Observations:

- Median filtering is highly effective for impulse noise because it rejects extreme outlier pixels.
- Gaussian filtering performs poorly for impulse noise because it averages corrupted pixels into their neighborhood.
- Median + sharpening produces strong visual results for this degradation type.

---

### Low Contrast + Noise

Strong performing approach:

- Bilateral filtering + CLAHE

Observations:

- CLAHE improves local contrast and makes details more visible.
- If applied aggressively, contrast enhancement can also make noise more noticeable.
- This degradation is harder than pure noise because the pipeline must handle both illumination/contrast and corruption.

---

### Motion Blur

Strong performing approach:

- Sharpening-based restoration provides limited improvement.

Observations:

- Sharpening can enhance edges but cannot truly recover information lost through motion blur.
- Motion blur is directionally structured, so simple spatial sharpening has limited effectiveness.
- This pipeline demonstrates an important limitation of naive restoration methods.

---

## 2. Pipeline Comparisons

### Pipeline A — Gaussian Noise

- Moderate PSNR and SSIM improvement.
- Good visual quality.
- Balanced smoothing and sharpening.

### Pipeline B — Salt-and-Pepper Noise + Median Filtering

- Strong improvement in both PSNR and SSIM.
- Clearly superior for impulse noise.
- Demonstrates the importance of choosing filters based on the degradation model.

### Pipeline C — Gaussian Baseline on Salt-and-Pepper Noise

- Lower performance than median filtering.
- Demonstrates that generic smoothing is not appropriate for impulse noise.

### Pipeline D — Low Contrast + Gaussian Noise

- CLAHE improves local contrast.
- Noise remains partially visible after enhancement.
- Highlights the complexity of combined degradations.

### Pipeline E — Motion Blur

- Sharpening does not fully recover lost detail.
- PSNR may not improve significantly.
- Demonstrates limitations of simple restoration methods for blur.

---

## 3. Manual vs OpenCV Implementations

Manual Gaussian and median filters were compared against OpenCV implementations.

The manual implementations are useful for understanding the algorithms internally, while OpenCV demonstrates the kind of performance expected from optimized production libraries.

### Updated Benchmark Results

Benchmarks were collected over 6 full pipeline runs using:

```bash
./build/image_pipeline --input images/input/test.jpg --output output
```

| Variant | Implementation | Average Runtime |
|---|---|---:|
| F1 | OpenCV Gaussian | 0.3411 ms |
| F2 | Manual Gaussian, optimized | 113.7217 ms |
| G1 | OpenCV Median | 2.5209 ms |
| G2 | Manual Median | 2228.4269 ms |

### Key Findings

- The optimized manual Gaussian preserves the same output quality metrics while improving the implementation structure.
- OpenCV remains orders of magnitude faster because it uses low-level optimizations such as SIMD/vectorization, cache-friendly memory access, and highly tuned kernels.
- Manual median filtering is useful for understanding the algorithm but is not suitable for real-time use in its current form.
- These results demonstrate the gap between correct algorithm implementation and production-grade optimized image processing.

### Raw Benchmark Runs

| Run | F1 OpenCV Gaussian | F2 Manual Gaussian | G1 OpenCV Median | G2 Manual Median |
|---:|---:|---:|---:|---:|
| 1 | 0.4220 ms | 111.7884 ms | 2.5101 ms | 2227.8407 ms |
| 2 | 0.3235 ms | 113.7408 ms | 2.4416 ms | 2217.8954 ms |
| 3 | 0.3684 ms | 117.5173 ms | 2.6681 ms | 2261.8102 ms |
| 4 | 0.3029 ms | 112.6760 ms | 2.5436 ms | 2230.4364 ms |
| 5 | 0.3361 ms | 111.8006 ms | 2.5235 ms | 2219.2005 ms |
| 6 | 0.2937 ms | 114.8073 ms | 2.4385 ms | 2213.3779 ms |

### Manual Gaussian Optimization Summary

The manual Gaussian implementation was optimized through three changes:

1. Replaced naive 2D convolution with separable 1D convolution.
2. Switched the Gaussian kernel and intermediate buffer from `CV_64F` to `CV_32F`.
3. Pre-padded the image using `cv::copyMakeBorder` to remove `reflectIndex()` calls from the convolution hot loop.

The optimized manual Gaussian preserved the same quality metrics in the smoke test:

| Metric | Value |
|---|---:|
| Final MSE | 166.8145 |
| Final PSNR | 25.9085 dB |
| Final SSIM | 0.6398 |

### Interpretation

- Manual Gaussian filtering is now more algorithmically efficient than the original naive implementation.
- OpenCV remains orders of magnitude faster because it uses highly optimized implementations, including vectorization, cache-aware memory access, and low-level CPU optimizations.
- Manual median filtering is correct and educational, but very slow compared with OpenCV.
- For real-time or production image-processing systems, optimized libraries are essential.

### Key Insight

> Manual implementations are valuable for learning and verification, but optimized libraries are necessary for performance-critical imaging pipelines.

---

## 4. Metric Interpretation

### MSE

- Measures pixel-wise difference.
- Sensitive to noise.
- Does not always reflect perceived visual quality.

### PSNR

- Log-scale representation of MSE.
- Useful for numerical comparison.
- Higher PSNR usually means the restored image is closer to the original.

### SSIM

- Measures structural similarity.
- Better reflects human perception than MSE/PSNR alone.
- Useful for evaluating whether important image structures are preserved.

---

## Important Observation

> Improvements in PSNR do not always correspond perfectly to improvements in SSIM.

This is especially visible in:

- sharpening-heavy pipelines
- motion blur scenarios
- cases where visual contrast improves but pixel-wise similarity does not

---

## 5. Key Takeaways

- Different degradation types require different restoration strategies.
- Median filtering is highly effective for impulse noise.
- Bilateral filtering preserves edges better than Gaussian smoothing.
- CLAHE improves local contrast but can also amplify noise.
- Sharpening alone is insufficient for true deblurring.
- PSNR and SSIM should be evaluated together.
- Manual implementations help explain algorithm behavior.
- Optimized libraries significantly outperform manual implementations in runtime.

---

## Conclusion

The experiments demonstrate that:

> Effective image restoration requires understanding the degradation model and selecting appropriate algorithms rather than applying generic filtering.

The framework enables systematic evaluation of restoration quality, runtime, and implementation tradeoffs.