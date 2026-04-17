# 🚀 HPC System Monitor v6.0

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://github.com/OxyOxygen/system_monitor)
[![Release](https://img.shields.io/badge/Release-v2.0-green.svg)](https://github.com/OxyOxygen/system_monitor/releases)

**A professional-grade hardware telemetry suite for Competitive Gaming, AI Engineering, and HPC Diagnostics.**

---

## 📸 Preview
![Project Hero Mockup](https://raw.githubusercontent.com/OxyOxygen/system_monitor/main/assets/preview.png)
*(Note: Replace with actual screenshot of the application running)*

---

## 🎯 Why HPC System Monitor?
Most monitors show you "what" is happening. We show you **how it affects your performance**.

### 🕹️ Gaming Performance Hub
*Professional benchmarking for competitive players.*
- **Session Analysis**: Track FPS (Avg/Min/Max), 1% Lows, and GPU Power usage.
- **Latency Tracker**: Real-time Network Ping and stability graphs.
- **Visual Timelines**: High-res charts for thermal stress and FPS stability.
- **Performance Reports**: Auto-generated `.txt` debriefs with AI-driven optimization tips.

### 🧠 AI Engineer Dashboard
*Precision monitoring for ML practitioners.*
- **Process Intelligence**: Auto-detection of PyTorch, TensorFlow, Ollama, and 30+ AI tools.
- **VRAM Breakdown**: Real-time memory allocation per model and process.
- **Hardware Estimator**: See if your system can run Llama 3, SDXL, or Flux before loading them.

### 🌡️ HPC Engine Diagnostics
*Deep hardware heuristics.*
- **Bottleneck Detection**: Real-time identification of CPU/GPU/Network bounds.
- **Thermal Heuristics**: Predict thermal throttling before it hits.
- **Electricity Costing**: Live projection of power costs based on usage.

---

## ⚡ Quick Start

### 📦 Option A: Download & Run (Recommended)
1. Go to the [Releases](https://github.com/OxyOxygen/system_monitor/releases) section.
2. Download the latest `SystemMonitor_Windows.zip`.
3. Extract and run `SystemMonitor.exe`.

### ⚒️ Option B: Build from Source
**Prerequisites**: Windows 10/11, CMake 3.15+, Visual Studio 2019+.
```powershell
cmake -B build
cmake --build build --config Release
./build/Release/SystemMonitor.exe
```

---

## 🛠️ Technical Stack
- **Core**: C++17
- **UI**: Dear ImGui (Custom Dark Theme)
- **Graphics**: OpenGL 3.0 / GLFW
- **APIs**: NVML (NVIDIA), DXGI, Windows PDH, IP Helper, Direct3D 11

---

## 🗺️ Roadmap & Contributing
We welcome contributions! See the `Issues` tab for "Good First Issues".
- [ ] Linux Support (Native `/proc` integration)
- [ ] AMD GPU (ADL/ROCm) Integration
- [ ] Localization (TR/EN)

---

## 📄 License
Licensed under the **MIT License**. See [LICENSE](LICENSE) for details.

---
*Created for gamers and engineers who demand total control.*
