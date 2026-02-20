#pragma once

#include <pdh.h>
#include <vector>
#include <windows.h>

class CpuMonitor {
public:
  CpuMonitor();
  ~CpuMonitor();

  double getCpuUsage();
  std::vector<double> getPerCoreUsage();
  int getNumCores() const { return numCores; }

private:
  PDH_HQUERY cpuQuery;
  PDH_HCOUNTER cpuCounter;

  PDH_HQUERY coreQuery;
  std::vector<PDH_HCOUNTER> coreCounters;
  int numCores;

  bool initialized;
  bool coresInitialized;
};
