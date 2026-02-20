#pragma once

#include <cstdint>
#include <string>

struct EnergyInfo {
  // Real-time power estimates (watts)
  double totalWatts;      // Total system power estimate
  double cpuWatts;        // CPU power estimate
  double gpuWatts;        // GPU power estimate
  double systemBaseWatts; // Base system power (mobo, RAM, drives)

  // Session tracking
  double sessionEnergyWh; // Total energy used this session (Wh)
  double sessionCost;     // Session cost in user currency
  double sessionHours;    // Session duration in hours

  // Daily estimates
  double dailyEstimateKwh;  // Projected daily usage (kWh)
  double dailyEstimateCost; // Projected daily cost

  // Monthly estimate
  double monthlyEstimateCost; // Projected monthly cost

  // Configuration
  double costPerKwh;    // Electricity cost per kWh (user configurable)
  std::string currency; // Currency symbol

  // Component TDPs
  int cpuTdpWatts;
  int gpuTdpWatts;
};

class EnergyMonitor {
public:
  EnergyMonitor();
  ~EnergyMonitor() = default;

  // Call every update cycle with current CPU/GPU usage
  void update(double cpuUsagePercent, double gpuUsagePercent, int cpuTdp,
              int gpuTdp);

  EnergyInfo getEnergyInfo() const;

  // Configuration
  void setCostPerKwh(double cost);
  void setCurrency(const std::string &symbol);

private:
  EnergyInfo info;

  // Timing
  uint64_t sessionStartTime; // GetTickCount64 at start
  uint64_t lastUpdateTime;   // Last update timestamp

  // Accumulated energy
  double totalEnergyJoules; // Total energy in Joules (watts × seconds)

  // Detect CPU TDP from name
  int detectCpuTdp();
  std::string detectCpuName();
};
