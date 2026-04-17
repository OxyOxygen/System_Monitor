#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
 * @brief Unified GPU metrics for the Hardware Abstraction Layer (HAL).
 * decoupling the UI from specific vendor APIs (NVML, ROCm, DXGI).
 */
struct UniversalGpuMetrics {
    std::string name;
    std::string vendor;      // "NVIDIA", "AMD", "Intel", "Generic"
    
    double loadPercent = 0;
    double memLoadPercent = 0;
    
    double vramUsedBytes = 0;
    double vramTotalBytes = 0;
    
    double tempC = -1.0;     // -1 if unknown
    double powerWatts = 0;
    uint32_t fanSpeedPct = 0;
    
    uint32_t coreClockMhz = 0;
    uint32_t memClockMhz = 0;

    bool available = false;
};

/**
 * @brief Interface for GPU monitoring providers.
 * Implementations can be vendor-specific (NVML) or platform-specific (DXGI/sysfs).
 */
class IGpuProvider {
public:
    virtual ~IGpuProvider() = default;
    
    virtual bool isAvailable() const = 0;
    virtual std::string getProviderName() const = 0;
    
    virtual std::vector<UniversalGpuMetrics> getAllGpus() = 0;
};
