#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <windows.h>

struct ProcessInfo {
  DWORD pid;
  std::string name;
  double cpuUsage;     // CPU usage percentage
  uint64_t memoryUsed; // Working set size in bytes
};

class ProcessMonitor {
public:
  ProcessMonitor();
  ~ProcessMonitor() = default;

  std::vector<ProcessInfo> getTopProcesses(int count = 15);

private:
  ULARGE_INTEGER lastSystemTime;
  ULARGE_INTEGER lastIdleTime;
  int numProcessors;
};
