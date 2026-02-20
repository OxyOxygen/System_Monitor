#include "energy_monitor.h"
#include <windows.h>

// ─── CPU TDP Lookup ──────────────────────────────────────────────────────────
std::string EnergyMonitor::detectCpuName() {
  char cpuName[256] = {};
  HKEY hKey;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                    "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0,
                    KEY_READ, &hKey) == ERROR_SUCCESS) {
    DWORD size = sizeof(cpuName);
    RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr,
                     (LPBYTE)cpuName, &size);
    RegCloseKey(hKey);
  }
  return std::string(cpuName);
}

int EnergyMonitor::detectCpuTdp() {
  std::string name = detectCpuName();

  // Intel 14th/13th gen
  if (name.find("14900") != std::string::npos ||
      name.find("13900") != std::string::npos)
    return 253;
  if (name.find("14700") != std::string::npos ||
      name.find("13700") != std::string::npos)
    return 219;
  if (name.find("14600") != std::string::npos ||
      name.find("13600") != std::string::npos)
    return 181;
  if (name.find("14400") != std::string::npos ||
      name.find("13400") != std::string::npos)
    return 148;
  // Intel 12th gen
  if (name.find("12900") != std::string::npos)
    return 241;
  if (name.find("12700") != std::string::npos)
    return 190;
  if (name.find("12600") != std::string::npos)
    return 150;
  if (name.find("12400") != std::string::npos)
    return 117;
  if (name.find("12100") != std::string::npos)
    return 89;
  // Intel 11th/10th gen
  if (name.find("11900") != std::string::npos)
    return 125;
  if (name.find("11700") != std::string::npos)
    return 125;
  if (name.find("11600") != std::string::npos)
    return 65;
  if (name.find("11400") != std::string::npos)
    return 65;
  if (name.find("10900") != std::string::npos)
    return 125;
  if (name.find("10700") != std::string::npos)
    return 65;
  if (name.find("10600") != std::string::npos)
    return 65;
  if (name.find("10400") != std::string::npos)
    return 65;
  // Intel laptop U/P series
  if (name.find("1355U") != std::string::npos ||
      name.find("1360P") != std::string::npos)
    return 28;
  if (name.find("1255U") != std::string::npos ||
      name.find("1260P") != std::string::npos)
    return 28;
  if (name.find("1235U") != std::string::npos)
    return 15;
  // AMD Ryzen 7000
  if (name.find("9950X") != std::string::npos)
    return 170;
  if (name.find("9900X") != std::string::npos)
    return 120;
  if (name.find("9700X") != std::string::npos)
    return 65;
  if (name.find("9600X") != std::string::npos)
    return 65;
  if (name.find("7950X") != std::string::npos)
    return 170;
  if (name.find("7900X") != std::string::npos)
    return 170;
  if (name.find("7800X") != std::string::npos)
    return 105;
  if (name.find("7700X") != std::string::npos)
    return 105;
  if (name.find("7600X") != std::string::npos)
    return 105;
  if (name.find("7600") != std::string::npos)
    return 65;
  // AMD Ryzen 5000
  if (name.find("5950X") != std::string::npos)
    return 105;
  if (name.find("5900X") != std::string::npos)
    return 105;
  if (name.find("5800X") != std::string::npos)
    return 105;
  if (name.find("5700X") != std::string::npos)
    return 65;
  if (name.find("5600X") != std::string::npos)
    return 65;
  if (name.find("5600") != std::string::npos)
    return 65;
  // AMD laptop
  if (name.find("7840U") != std::string::npos ||
      name.find("7840HS") != std::string::npos)
    return 28;
  if (name.find("6800U") != std::string::npos)
    return 28;
  // Default
  if (name.find("i9") != std::string::npos)
    return 125;
  if (name.find("i7") != std::string::npos)
    return 95;
  if (name.find("i5") != std::string::npos)
    return 65;
  if (name.find("i3") != std::string::npos)
    return 55;
  if (name.find("Ryzen 9") != std::string::npos)
    return 120;
  if (name.find("Ryzen 7") != std::string::npos)
    return 95;
  if (name.find("Ryzen 5") != std::string::npos)
    return 65;
  if (name.find("Ryzen 3") != std::string::npos)
    return 55;
  return 65; // safe default
}

// ─── Constructor ─────────────────────────────────────────────────────────────
EnergyMonitor::EnergyMonitor() : totalEnergyJoules(0.0) {
  info = {};
  info.costPerKwh = 4.20; // Default: Turkey average ~4.20 TL/kWh
  info.currency = "TL";
  info.systemBaseWatts = 50.0; // Motherboard + RAM + drives
  info.cpuTdpWatts = detectCpuTdp();
  info.gpuTdpWatts = 150; // Will be updated from GpuMonitor

  sessionStartTime = GetTickCount64();
  lastUpdateTime = sessionStartTime;
}

// ─── Update ──────────────────────────────────────────────────────────────────
void EnergyMonitor::update(double cpuUsagePercent, double gpuUsagePercent,
                           int cpuTdp, int gpuTdp) {
  uint64_t now = GetTickCount64();
  double deltaSeconds = (double)(now - lastUpdateTime) / 1000.0;
  lastUpdateTime = now;

  // Use provided TDPs if valid
  if (cpuTdp > 0)
    info.cpuTdpWatts = cpuTdp;
  if (gpuTdp > 0)
    info.gpuTdpWatts = gpuTdp;

  // CPU power: idle base + load-proportional
  double cpuLoad = cpuUsagePercent / 100.0;
  info.cpuWatts =
      (info.cpuTdpWatts * 0.15) + (info.cpuTdpWatts * 0.75 * cpuLoad);

  // GPU power: idle base + load-proportional
  double gpuLoad = gpuUsagePercent / 100.0;
  info.gpuWatts =
      (info.gpuTdpWatts * 0.10) + (info.gpuTdpWatts * 0.85 * gpuLoad);

  // Total system power
  info.systemBaseWatts = 50.0;
  info.totalWatts = info.cpuWatts + info.gpuWatts + info.systemBaseWatts;

  // Accumulate energy (joules = watts × seconds)
  if (deltaSeconds > 0.0 && deltaSeconds < 10.0) { // Sanity check
    totalEnergyJoules += info.totalWatts * deltaSeconds;
  }

  // Session duration
  info.sessionHours = (double)(now - sessionStartTime) / 3600000.0;

  // Convert joules to Wh (1 Wh = 3600 J)
  info.sessionEnergyWh = totalEnergyJoules / 3600.0;

  // Session cost
  double sessionKwh = info.sessionEnergyWh / 1000.0;
  info.sessionCost = sessionKwh * info.costPerKwh;

  // Daily estimate (extrapolate from current average)
  if (info.sessionHours > 0.001) {
    double avgWatts = info.sessionEnergyWh / info.sessionHours;
    info.dailyEstimateKwh = (avgWatts * 24.0) / 1000.0;
    info.dailyEstimateCost = info.dailyEstimateKwh * info.costPerKwh;
    info.monthlyEstimateCost = info.dailyEstimateCost * 30.0;
  }
}

EnergyInfo EnergyMonitor::getEnergyInfo() const { return info; }

void EnergyMonitor::setCostPerKwh(double cost) { info.costPerKwh = cost; }
void EnergyMonitor::setCurrency(const std::string &symbol) {
  info.currency = symbol;
}
