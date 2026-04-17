# 🚀 HPC System Monitor v7.0: Hardware Intelligence Redefined

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-blueviolet.svg)](https://www.microsoft.com/windows)

**HPC System Monitor** is a professional-grade telemetry and diagnostics suite engineered for high-performance computing. Whether you are a competitive gamer optimizing for FPS stability, an AI engineer managing VRAM-heavy LLMs, or a hardware enthusiast tracking energy costs, this tool provides the deep visibility you need.

---

## 📑 Table of Contents
- [Key Modules](#-key-modules)
- [Gaming Analysis 2.0](#-gaming-analysis-20)
- [HPC Engine Intelligence](#-hpc-engine-intelligence)
- [AI & ML Engineer Dashboard](#-ai--ml-engineer-dashboard)
- [Data Interoperability (Export)](#-data-interoperability-export)
- [Technical Architecture](#-technical-architecture)
- [Installation & Build](#-installation--build)

---

## 🏗️ Key Modules

### 🎮 Gaming Analysis 2.0
The cornerstone of v7.0. A dedicated suite for real-time performance tracking during intensive workloads.
- **High-Precision Telemetry**: Captures FPS trends, GPU power draw (Watts), and thermal signatures at 1Hz.
- **Latency Trend Analysis**: Dedicated network latency (Ping) tracking to separate network lag from hardware stutter.
- **Visual Timelines**: Multi-pane line charts showing simultaneous CPU/GPU/Ping/FPS trends.
- **Dynamic Thermal Alerts**: Color-coded status cards that instantly flag critical temperatures (>82°C) or latency spikes.

### 🧠 HPC Engine Intelligence
A sophisticated heuristic analysis engine that acts as a digital hardware assistant.
- **Bottleneck Diagnostics**: Automatically identifies if a workload is CPU, GPU, or VRAM bound.
- **Actionable Insights**: Provides real-time recommendations for graphics optimization and system tuning.
- **Stability Monitoring**: Detects frame-time variance to assess the smoothness of your performance.

### ⚡ Energy & Economics
- **Real-time Wattage**: Component-level power breakdown with ultra-low overhead.
- **Session Cost**: Live session cost tracking and monthly energy bill projections.
- **TDP Profiles**: Automated hardware detection for accurate power estimation.

---

## 🧠 AI & ML Engineer Dashboard
Designed specifically for the modern AI workflow, supporting over 30+ detection profiles.
- **AI Process Detection**: Automatically flags PyTorch, TensorFlow, Ollama, Stable Diffusion, and more.
- **VRAM Capacity Estimator**: Calculates compatibility for 22+ popular LLMs (Llama 3, SDXL, etc.) based on available memory.
- **Environment Check**: Instant verification of CUDA readiness and Python availability.

---

## 📊 Data Interoperability (Export)
v7.0 introduces professional-grade data handling for engineers and data scientists:
- **JSON Export**: Deep-nested session data, including bottleneck signatures and AI diagnostics, formatted for automated pipelines.
- **CSV Export**: High-resolution timeline data (Timestamp, FPS, Ping, CPU, GPU) ready for analysis in Excel, Pandas, or Grafana.
- **TXT Reports**: Clean, human-readable session summaries for quick reference.

---

## 🛠️ Technical Architecture

### Hardware Abstraction Layer (HAL)
The project utilizes a modular **HAL Architecture** (`IGpuProvider`) that decouples the UI from specific vendor APIs. 
- **Current Support**: NVIDIA NVML (Dynamic Loading), Windows PDH, DXGI.
- **Planned Support**: AMD ROCm, Intel OneAPI, and Linux `sysfs` integration.

### Performance & Stability
- **Asynchronous Polling**: Telemetry is collected in dedicated background threads to ensure zero impact on game/application performance.
- **Dynamic Linking**: NVML and other vendor DLLs are loaded at runtime; the app remains functional even if specific drivers are missing.

---

## 🚀 Installation & Build

### Prerequisites
- **Windows 10/11**
- **CMake 3.15+**
- **Visual Studio 2019/2022** (Desktop C++ Workload)

### Quick Start
```powershell
# Clone and build in one go
cmake -B build
cmake --build build --config Release
./build/Release/SystemMonitor.exe
```

---

## 🤝 Project Credits
Developed as an advanced hardware intelligence tool. For contributions, bug reports, or feature requests, please open an issue or submit a pull request.

---
> [!NOTE]
> *This tool is optimized for zero-overhead background monitoring and does not require periodic administration privileges for core telemetry.*
