***# System Architecture

## Overview

This project is designed as a **modular image restoration and evaluation framework**.
The goal is to separate concerns between degradation simulation, restoration pipelines, and evaluation.

The system follows a clear processing flow:
Input Image ->
Degradation Module ->
Restoration Pipeline ->
Evaluation (MSE, PSNR, SSIM) ->
Reporting & Visualization


---

## Design Principles

- **Modularity** → each component has a single responsibility  
- **Extensibility** → new filters, degradations, or metrics can be added easily  
- **Reproducibility** → experiments are defined declaratively  
- **Separation of concerns** → processing, evaluation, and reporting are isolated  

---

## Core Components

### 1. Experiment Registry (`experiment_registry`)

Defines all experiment configurations.

Each experiment specifies:
- degradation type and parameters
- sequence of restoration stages
- output directory
- description

This allows:
- reproducible experiments
- easy comparison between pipelines

---

### 2. Experiment Runner (`experiment_runner`)

Responsible for:
- executing experiments
- applying degradation
- applying restoration stages
- computing metrics
- collecting outputs

This is the **execution engine** of the system.

---

### 3. Pipeline (`pipeline`)

Handles the application of:
- degradation functions
- restoration stages

Each stage is defined using a `StageType` abstraction.

Example:
Gaussian noise -> Bilateral filter -> Unsharp mask


This abstraction allows flexible pipeline composition.

---

### 4. Degradation Module (`degradation`)

Simulates real-world image corruption:

- Gaussian noise
- Salt-and-pepper noise
- Contrast reduction + noise
- Motion blur

These controlled degradations allow meaningful evaluation of restoration methods.

---

### 5. Filters (`filters`)

Implements OpenCV-based filtering operations:

- Gaussian filtering
- Median filtering
- Bilateral filtering
- CLAHE
- Sharpening techniques

These represent practical restoration methods used in imaging systems.

---

### 6. Manual Filters (`manual_filters`)

Contains hand-implemented versions of:

- Gaussian blur
- Median filter

Purpose:
- understand underlying algorithms
- validate correctness against OpenCV
- compare performance vs library implementations

---

### 7. Metrics (`metrics`)

Computes image quality metrics:

- MSE (Mean Squared Error)
- PSNR (Peak Signal-to-Noise Ratio)
- SSIM (Structural Similarity Index)

These provide both:
- pixel-level accuracy
- perceptual similarity

---

### 8. Reporting (`reporting`)

Generates:

- per-experiment reports
- global summary report
- CSV benchmark file

Also includes:
- automatic interpretation of results
- structured metric comparison

---

### 9. Visualization (`visualization`)

Creates:
- comparison strips
- intermediate stage outputs

This helps visually analyze pipeline behavior.

---

## Data Flow

For each experiment:

1. Load original image  
2. Apply degradation  
3. Apply sequence of restoration stages  
4. Compute metrics (before vs after)  
5. Save outputs and reports  

---

## Extensibility

The system can be easily extended by:

- adding new degradation models  
- adding new filters or stages  
- adding new evaluation metrics  
- adding GPU acceleration  
- adding batch processing  

---

## Summary

The architecture transforms the project from a simple image-processing script into a:

> **reusable experimental framework for analyzing image restoration techniques**

This structure mirrors real-world imaging pipelines, where modular processing and evaluation are critical.
