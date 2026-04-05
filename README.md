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

Manual implementations of Gaussian and median filters were developed and compared against OpenCV.

**Findings:**
- PSNR and SSIM are nearly identical → correctness validated  
- OpenCV implementations are **orders of magnitude faster**  
- Demonstrates the importance of optimized libraries in real systems  

Example:
- Manual Gaussian: ~2000 ms  
- OpenCV Gaussian: ~2 ms  

---

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make -j
