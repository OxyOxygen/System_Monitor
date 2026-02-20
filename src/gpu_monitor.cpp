#include "gpu_monitor.h"
#include <cstdlib>
#include <dxgi.h>
#include <iostream>
#include <pdhmsg.h>

#pragma comment(lib, "dxgi.lib")

// ─── GPU TDP Lookup Table ────────────────────────────────────────────────────
int GpuMonitor::lookupGpuTdp(const std::string &name) {
  // NVIDIA RTX 40 series
  if (name.find("4090") != std::string::npos)
    return 450;
  if (name.find("4080") != std::string::npos)
    return 320;
  if (name.find("4070 Ti") != std::string::npos)
    return 285;
  if (name.find("4070") != std::string::npos)
    return 200;
  if (name.find("4060 Ti") != std::string::npos)
    return 160;
  if (name.find("4060") != std::string::npos)
    return 115;
  // NVIDIA RTX 30 series
  if (name.find("3090") != std::string::npos)
    return 350;
  if (name.find("3080") != std::string::npos)
    return 320;
  if (name.find("3070 Ti") != std::string::npos)
    return 290;
  if (name.find("3070") != std::string::npos)
    return 220;
  if (name.find("3060 Ti") != std::string::npos)
    return 200;
  if (name.find("3060") != std::string::npos)
    return 170;
  if (name.find("3050") != std::string::npos)
    return 130;
  // NVIDIA RTX 20 series
  if (name.find("2080 Ti") != std::string::npos)
    return 250;
  if (name.find("2080") != std::string::npos)
    return 215;
  if (name.find("2070") != std::string::npos)
    return 175;
  if (name.find("2060") != std::string::npos)
    return 160;
  // NVIDIA GTX 16 series
  if (name.find("1660") != std::string::npos)
    return 120;
  if (name.find("1650") != std::string::npos)
    return 75;
  // NVIDIA GTX 10 series
  if (name.find("1080 Ti") != std::string::npos)
    return 250;
  if (name.find("1080") != std::string::npos)
    return 180;
  if (name.find("1070") != std::string::npos)
    return 150;
  if (name.find("1060") != std::string::npos)
    return 120;
  // AMD RX 7000 series
  if (name.find("7900 XTX") != std::string::npos)
    return 355;
  if (name.find("7900 XT") != std::string::npos)
    return 300;
  if (name.find("7800 XT") != std::string::npos)
    return 263;
  if (name.find("7700 XT") != std::string::npos)
    return 245;
  if (name.find("7600") != std::string::npos)
    return 165;
  // AMD RX 6000 series
  if (name.find("6900 XT") != std::string::npos)
    return 300;
  if (name.find("6800 XT") != std::string::npos)
    return 300;
  if (name.find("6800") != std::string::npos)
    return 250;
  if (name.find("6700 XT") != std::string::npos)
    return 230;
  if (name.find("6600 XT") != std::string::npos)
    return 160;
  if (name.find("6600") != std::string::npos)
    return 132;
  // Intel Arc
  if (name.find("A770") != std::string::npos)
    return 225;
  if (name.find("A750") != std::string::npos)
    return 225;
  if (name.find("A580") != std::string::npos)
    return 185;
  // Default
  return 150;
}

// ─── DXGI Detection ─────────────────────────────────────────────────────────
// Vendor IDs: NVIDIA=0x10DE, AMD=0x1002, Intel=0x8086, Microsoft=0x1414
void GpuMonitor::detectGpuViaDXGI() {
  IDXGIFactory *pFactory = nullptr;
  HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&pFactory);
  if (FAILED(hr) || !pFactory) {
    detectedGpuName = "Unknown GPU";
    detectedVramTotal = 0;
    detectedTdp = 150;
    return;
  }

  // Enumerate ALL adapters, prefer discrete GPU (NVIDIA/AMD) over Intel iGPU
  std::string bestName;
  uint64_t bestVram = 0;
  int bestTdp = 150;
  bool foundDiscrete = false;

  IDXGIAdapter *pAdapter = nullptr;
  for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
       i++) {
    DXGI_ADAPTER_DESC desc;
    if (SUCCEEDED(pAdapter->GetDesc(&desc))) {
      // Skip Microsoft Basic Render Driver
      if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c) {
        pAdapter->Release();
        continue;
      }

      char gpuName[256] = {};
      WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, gpuName,
                          sizeof(gpuName), nullptr, nullptr);

      bool isDiscrete = (desc.VendorId == 0x10DE || // NVIDIA
                         desc.VendorId == 0x1002);  // AMD

      // Always prefer discrete GPU; if we already found one, skip others
      if (isDiscrete && !foundDiscrete) {
        bestName = gpuName;
        bestVram = desc.DedicatedVideoMemory;
        bestTdp = lookupGpuTdp(bestName);
        foundDiscrete = true;
      } else if (!foundDiscrete && bestName.empty()) {
        // Fallback: use first non-software adapter (Intel iGPU, etc.)
        bestName = gpuName;
        bestVram = desc.DedicatedVideoMemory;
        bestTdp = lookupGpuTdp(bestName);
      }
    }
    pAdapter->Release();
  }

  pFactory->Release();

  if (!bestName.empty()) {
    detectedGpuName = bestName;
    detectedVramTotal = bestVram;
    detectedTdp = bestTdp;
  } else {
    detectedGpuName = "Unknown GPU";
    detectedVramTotal = 0;
    detectedTdp = 150;
  }
}

// ─── Constructor ─────────────────────────────────────────────────────────────
GpuMonitor::GpuMonitor()
    : gpuQuery(nullptr), gpuCounter(nullptr), vramCounter(nullptr),
      initialized(false), detectedVramTotal(0), detectedTdp(150) {

  // Detect GPU via DXGI first
  detectGpuViaDXGI();

  PDH_STATUS status;

  status = PdhOpenQuery(nullptr, 0, &gpuQuery);
  if (status != ERROR_SUCCESS) {
    return;
  }

  status = PdhAddEnglishCounter(
      gpuQuery, "\\GPU Engine(*)\\Utilization Percentage", 0, &gpuCounter);
  if (status != ERROR_SUCCESS) {
    PdhCloseQuery(gpuQuery);
    gpuQuery = nullptr;
    return;
  }

  // VRAM counter (optional)
  PdhAddEnglishCounter(gpuQuery, "\\GPU Adapter Memory(*)\\Dedicated Usage", 0,
                       &vramCounter);

  PdhCollectQueryData(gpuQuery);
  initialized = true;
}

GpuMonitor::~GpuMonitor() {
  if (gpuQuery) {
    PdhCloseQuery(gpuQuery);
  }
}

// ─── Get GPU Info ────────────────────────────────────────────────────────────
GpuInfo GpuMonitor::getGpuInfo() {
  GpuInfo info = {};
  info.gpuName = detectedGpuName;
  info.vramTotal = detectedVramTotal;
  info.tdpWatts = detectedTdp;
  info.temperatureC = -1.0; // Not available without NVML
  info.available = initialized;

  if (!initialized) {
    return info;
  }

  PDH_STATUS status;

  status = PdhCollectQueryData(gpuQuery);
  if (status != ERROR_SUCCESS) {
    info.available = false;
    return info;
  }

  // GPU utilization — wildcard counter returns multiple instances
  // We need PdhGetFormattedCounterArray to read them all
  {
    DWORD bufSize = 0;
    DWORD itemCount = 0;
    status = PdhGetFormattedCounterArray(gpuCounter, PDH_FMT_DOUBLE, &bufSize,
                                         &itemCount, nullptr);
    if (status == PDH_MORE_DATA && bufSize > 0) {
      PDH_FMT_COUNTERVALUE_ITEM *items =
          (PDH_FMT_COUNTERVALUE_ITEM *)malloc(bufSize);
      if (items) {
        status = PdhGetFormattedCounterArray(gpuCounter, PDH_FMT_DOUBLE,
                                             &bufSize, &itemCount, items);
        if (status == ERROR_SUCCESS) {
          // Find the maximum GPU engine utilization
          double maxUsage = 0.0;
          for (DWORD i = 0; i < itemCount; i++) {
            if (items[i].FmtValue.CStatus == PDH_CSTATUS_VALID_DATA) {
              double val = items[i].FmtValue.doubleValue;
              if (val > maxUsage)
                maxUsage = val;
            }
          }
          info.gpuUsage = maxUsage;
          if (info.gpuUsage > 100.0)
            info.gpuUsage = 100.0;
        }
        free(items);
      }
    }
  }

  // VRAM usage — also wildcard, use array approach
  if (vramCounter) {
    DWORD bufSize = 0;
    DWORD itemCount = 0;
    status = PdhGetFormattedCounterArray(vramCounter, PDH_FMT_LARGE, &bufSize,
                                         &itemCount, nullptr);
    if (status == PDH_MORE_DATA && bufSize > 0) {
      PDH_FMT_COUNTERVALUE_ITEM *items =
          (PDH_FMT_COUNTERVALUE_ITEM *)malloc(bufSize);
      if (items) {
        status = PdhGetFormattedCounterArray(vramCounter, PDH_FMT_LARGE,
                                             &bufSize, &itemCount, items);
        if (status == ERROR_SUCCESS) {
          // Sum all adapter dedicated memory usage
          int64_t maxVram = 0;
          for (DWORD i = 0; i < itemCount; i++) {
            if (items[i].FmtValue.CStatus == PDH_CSTATUS_VALID_DATA) {
              if (items[i].FmtValue.largeValue > maxVram)
                maxVram = items[i].FmtValue.largeValue;
            }
          }
          info.vramUsed = (uint64_t)maxVram;
        }
        free(items);
      }
    }
  }

  // Calculate VRAM percentage
  if (info.vramTotal > 0) {
    info.vramUsagePercent = static_cast<double>(info.vramUsed) /
                            static_cast<double>(info.vramTotal) * 100.0;
  }

  // Estimate power draw: idle power + load-proportional power
  double loadFactor = info.gpuUsage / 100.0;
  double idlePower = detectedTdp * 0.10; // ~10% TDP at idle
  info.powerDrawWatts = idlePower + (detectedTdp * 0.85 * loadFactor);

  return info;
}
