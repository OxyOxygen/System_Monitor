#include "process_monitor.h"
#include <algorithm>
#include <psapi.h>
#include <tlhelp32.h>

ProcessMonitor::ProcessMonitor() : numProcessors(1) {
  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);
  numProcessors = sysInfo.dwNumberOfProcessors;
  if (numProcessors < 1)
    numProcessors = 1;

  lastSystemTime.QuadPart = 0;
  lastIdleTime.QuadPart = 0;
}

std::vector<ProcessInfo> ProcessMonitor::getTopProcesses(int count) {
  std::vector<ProcessInfo> processes;

  // Take a snapshot of all processes
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE) {
    return processes;
  }

  PROCESSENTRY32 pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(snapshot, &pe32)) {
    CloseHandle(snapshot);
    return processes;
  }

  do {
    // Skip System Idle Process (PID 0)
    if (pe32.th32ProcessID == 0)
      continue;

    ProcessInfo info = {};
    info.pid = pe32.th32ProcessID;
    info.name = pe32.szExeFile;
    info.cpuUsage = 0.0;
    info.memoryUsed = 0;

    // Open process to get memory info
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE, pe32.th32ProcessID);
    if (hProcess != nullptr) {
      PROCESS_MEMORY_COUNTERS pmc;
      if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        info.memoryUsed = pmc.WorkingSetSize;
      }
      CloseHandle(hProcess);
    }

    processes.push_back(info);
  } while (Process32Next(snapshot, &pe32));

  CloseHandle(snapshot);

  // Sort by memory usage (descending) as primary metric
  std::sort(processes.begin(), processes.end(),
            [](const ProcessInfo &a, const ProcessInfo &b) {
              return a.memoryUsed > b.memoryUsed;
            });

  // Limit to requested count
  if (static_cast<int>(processes.size()) > count) {
    processes.resize(count);
  }

  return processes;
}
