# Adaptive Image Restoration and Evaluation Framework (C++ / OpenCV)

## Overview

This project implements a **modular image restoration framework** that simulates real-world image degradations and evaluates restoration pipelines under controlled conditions.

The system enables systematic analysis of how different filtering strategies behave depending on the type of degradation, using both **numerical** and **perceptual** quality metrics.

The goal is not only to restore images, but to understand **why certain methods work better under specific conditions**, similar to challenges in real imaging pipelines.

---

## Key Features

- Modular pipeline architecture (degradation → restoration → evaluation)
- Multiple degradation models:
  - Gaussian noise
  - Salt-and-pepper noise
  - Low contrast + noise
  - Motion blur
- Multiple restoration techniques:
  - Gaussian filtering
  - Median filtering
  - Bilateral filtering
  - CLAHE (contrast enhancement)
  - Unsharp masking
  - Laplacian sharpening
- Manual implementations of core filters:
  - Gaussian blur
  - Median filter
- Quantitative evaluation:
  - MSE (Mean Squared Error)
  - PSNR (Peak Signal-to-Noise Ratio)
  - SSIM (Structural Similarity Index)
- Automated experiment execution and reporting
- CSV export for structured benchmarking

---

## Architecture

The project is structured as a reusable experimental framework:

- **experiment_registry** → defines all experiment configurations  
- **experiment_runner** → executes pipelines and collects results  
- **pipeline** → applies degradation and restoration stages  
- **metrics** → computes MSE, PSNR, SSIM  
- **manual_filters** → custom implementations of filters  
- **reporting** → generates reports and CSV summaries  
- **visualization** → creates comparison outputs  

See [`docs/architecture.md`](docs/architecture.md) for detailed design.

---

## Example Pipelines

### Gaussian Noise Restoration
Gaussian noise → Bilateral filter → Unsharp mask

### Salt-and-Pepper Noise Restoration
Salt & pepper noise → Median filter → Unsharp mask

### Low Contrast Enhancement
Contrast reduction + noise → Bilateral filter → CLAHE

### Motion Blur Case
Motion blur → Sharpening (demonstrates limitations of naive restoration)

---

## Results & Key Insights

The framework reveals several important behaviors:

- Median filtering significantly outperforms Gaussian filtering for impulse noise  
- Bilateral filtering preserves edges better than standard smoothing  
- CLAHE improves contrast but does not fully recover heavily degraded images  
- Sharpening alone is ineffective for motion blur and may degrade quality  
- Improvements in PSNR do not always correspond to perceptual improvements (SSIM)  

Detailed analysis is available in [`docs/results.md`](docs/results.md).

---
## Manual vs OpenCV Comparison

Manual implementations of Gaussian and median filters were developed and compared against OpenCV equivalents.

The goal of the manual implementations is educational and analytical: they show how the algorithms work internally and allow direct comparison against optimized library implementations.

### Benchmark Summary

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

---

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make -j
```