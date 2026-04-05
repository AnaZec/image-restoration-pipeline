+# Experimental Results & Analysis

## Overview

The framework evaluates multiple restoration pipelines under controlled degradations using:

- MSE (pixel-level error)
- PSNR (signal fidelity)
- SSIM (perceptual similarity)

The goal is to understand how different filters behave under different types of image degradation.

---

## 1. Noise-Specific Behavior

### Gaussian Noise

Best performing approach:
- Bilateral filtering + sharpening

Observations:
- Gaussian filtering reduces noise but blurs edges
- Bilateral filtering preserves edges better
- Unsharp masking restores perceived sharpness

---

### Salt-and-Pepper Noise

Best performing approach:
- Median filtering

Observations:
- Median filtering effectively removes impulse noise
- Gaussian filtering performs poorly for this type of noise
- Median + sharpening produces strong results

---

## 2. Pipeline Comparisons

### Pipeline A (Gaussian Noise)
- Moderate PSNR improvement
- Good visual quality
- Balanced smoothing and sharpening

---

### Pipeline B (Salt & Pepper + Median)
- Strong improvement in both PSNR and SSIM
- Clearly superior for impulse noise
- Demonstrates importance of noise-aware filtering

---

### Pipeline C (Gaussian baseline)
- Lower performance than median filtering
- Shows limitations of generic smoothing

---

### Pipeline D (Low Contrast + Noise)
- CLAHE improves contrast
- Noise remains partially visible
- Highlights complexity of combined degradations

---

### Pipeline E (Motion Blur)
- Sharpening does not recover lost detail
- PSNR may not improve or may degrade
- Demonstrates limitations of naive restoration

---

## 3. Manual vs OpenCV Implementations

Manual implementations of Gaussian and median filters were compared with OpenCV.

### Results

- PSNR: nearly identical
- SSIM: nearly identical
- Runtime: dramatically different

Example:
- Manual implementation: ~2000 ms
- OpenCV implementation: ~2 ms

---

### Interpretation

- Manual implementations confirm correctness of algorithms
- OpenCV achieves massive speedup through optimization:
  - vectorization (SIMD)
  - cache-friendly memory access
  - optimized convolution strategies

---

### Key Insight

> Manual implementations are valuable for understanding, but optimized libraries are essential for real-time systems.

---

## 4. Metric Interpretation

### MSE
- Measures pixel-wise difference
- Sensitive to noise but not perceptual quality

### PSNR
- Log-scale representation of MSE
- Useful for numerical comparison

### SSIM
- Measures structural similarity
- Better reflects human perception

---

## Important Observation

> Improvements in PSNR do not always correspond to improvements in SSIM.

This is especially visible in:
- sharpening-heavy pipelines
- motion blur scenarios

---

## 5. Key Takeaways

- Different noise types require different restoration strategies  
- Median filtering is optimal for impulse noise  
- Bilateral filtering preserves edges better than Gaussian smoothing  
- Sharpening alone is insufficient for deblurring  
- Perceptual metrics (SSIM) are essential alongside PSNR  
- Optimized libraries significantly outperform naive implementations  

---

## Conclusion

The experiments demonstrate that:

> Effective image restoration requires **understanding the degradation model** and selecting appropriate algorithms rather than applying generic filtering.

The framework enables systematic evaluation of these tradeoffs.
