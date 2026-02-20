#include "cpu_monitor.h"
#include "disk_monitor.h"
#include "gpu_monitor.h"
#include "gui.h"
#include "memory_monitor.h"
#include "network_monitor.h"
#include "power_monitor.h"
#include "process_monitor.h"
#include "system_info.h"
#include <chrono>
#include <thread>
#include <vector>

int main() {
  // Initialize GUI
  GUI gui;
  if (!gui.init("System Monitor", 1600, 900)) {
    return -1;
  }

  // Initialize monitors
  CpuMonitor cpuMonitor;
  MemoryMonitor memoryMonitor;
  DiskMonitor diskMonitor;
  GpuMonitor gpuMonitor;
  PowerMonitor powerMonitor;
  ProcessMonitor processMonitor;
  NetworkMonitor networkMonitor;
  SystemInfoMonitor systemInfoMonitor;

  // Update interval (in seconds)
  constexpr double updateInterval = 1.0;
  auto lastUpdate = std::chrono::steady_clock::now();

  // Cached values
  double cpuUsage = 0.0;
  std::vector<double> coreUsages;
  MemoryInfo memInfo = {};
  DiskInfo diskInfo = {};
  GpuInfo gpuInfo = {};
  PowerInfo powerInfo = {};
  std::vector<ProcessInfo> processes;
  NetworkInfo netInfo = {};

  // Main loop
  while (!gui.shouldClose()) {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - lastUpdate;

    // Update monitor data at specified interval
    if (elapsed.count() >= updateInterval) {
      cpuUsage = cpuMonitor.getCpuUsage();
      coreUsages = cpuMonitor.getPerCoreUsage();
      memInfo = memoryMonitor.getMemoryInfo();
      diskInfo = diskMonitor.getDiskInfo();
      gpuInfo = gpuMonitor.getGpuInfo();
      powerInfo = powerMonitor.getPowerInfo();
      processes = processMonitor.getTopProcesses(15);
      netInfo = networkMonitor.getNetworkInfo();
      systemInfoMonitor.updateDynamic();

      lastUpdate = now;
    }

    // Render GUI
    gui.beginFrame();
    gui.render(cpuUsage, coreUsages, memInfo, diskInfo, gpuInfo, powerInfo,
               processes, netInfo, systemInfoMonitor.getInfo());
    gui.endFrame();

    // Small sleep to prevent excessive CPU usage by the monitor itself
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
  }

  gui.cleanup();
  return 0;
}
