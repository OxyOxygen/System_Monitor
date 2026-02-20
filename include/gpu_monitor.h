#pragma once

#include <cstdint>
#include <pdh.h>
#include <string>
#include <windows.h>

struct GpuInfo {
  std::string gpuName;     // GPU model name (e.g. "NVIDIA GeForce RTX 3060")
  double gpuUsage;         // GPU utilization percentage
  uint64_t vramUsed;       // Used VRAM in bytes
  uint64_t vramTotal;      // Total VRAM in bytes
  double vramUsagePercent; // VRAM usage percentage
  double temperatureC;     // GPU temperature in Celsius (-1 if unavailable)
  double powerDrawWatts;   // Estimated power draw in watts
  int tdpWatts;            // GPU TDP in watts (for energy calculation)
  bool available;          // GPU monitoring available
};

class GpuMonitor {
public:
  GpuMonitor();
  ~GpuMonitor();

  GpuInfo getGpuInfo();

private:
  PDH_HQUERY gpuQuery;
  PDH_HCOUNTER gpuCounter;
  PDH_HCOUNTER vramCounter;
  bool initialized;

  // DXGI-detected values (set once in constructor)
  std::string detectedGpuName;
  uint64_t detectedVramTotal;
  int detectedTdp;

  void detectGpuViaDXGI();
  int lookupGpuTdp(const std::string &gpuName);
};
