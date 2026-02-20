#pragma once

#include <cstdint>
#include <string>


struct SystemInfo {
  std::string computerName;
  std::string userName;
  std::string osVersion;
  std::string cpuName;
  int cpuCores;        // Physical cores
  int cpuLogicalCores; // Logical processors
  uint64_t totalRAMBytes;
  uint64_t uptimeSeconds; // System uptime in seconds
};

class SystemInfoMonitor {
public:
  SystemInfoMonitor();
  ~SystemInfoMonitor() = default;

  // Static info — gathered once
  const SystemInfo &getInfo() const { return info; }

  // Dynamic info — call periodically
  void updateDynamic();

private:
  SystemInfo info;
  void gatherStaticInfo();
};
