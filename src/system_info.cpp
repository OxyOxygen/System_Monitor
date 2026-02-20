#include "system_info.h"
#include <sstream>
#include <windows.h>


#pragma comment(lib, "advapi32.lib")

SystemInfoMonitor::SystemInfoMonitor() { gatherStaticInfo(); }

void SystemInfoMonitor::gatherStaticInfo() {
  info = {};

  // Computer name
  char compName[MAX_COMPUTERNAME_LENGTH + 1] = {};
  DWORD compSize = sizeof(compName);
  if (GetComputerNameA(compName, &compSize)) {
    info.computerName = compName;
  } else {
    info.computerName = "Unknown";
  }

  // User name
  char userName[256] = {};
  DWORD userSize = sizeof(userName);
  if (GetUserNameA(userName, &userSize)) {
    info.userName = userName;
  } else {
    info.userName = "Unknown";
  }

  // OS Version — read from registry for modern Windows
  {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
                      KEY_READ, &hKey) == ERROR_SUCCESS) {
      char productName[256] = {};
      DWORD bufSize = sizeof(productName);
      DWORD type = REG_SZ;
      if (RegQueryValueExA(hKey, "ProductName", nullptr, &type,
                           (LPBYTE)productName, &bufSize) == ERROR_SUCCESS) {
        info.osVersion = productName;
      }

      // Append build number
      char buildNum[64] = {};
      bufSize = sizeof(buildNum);
      if (RegQueryValueExA(hKey, "CurrentBuildNumber", nullptr, &type,
                           (LPBYTE)buildNum, &bufSize) == ERROR_SUCCESS) {
        info.osVersion += " (Build ";
        info.osVersion += buildNum;
        info.osVersion += ")";
      }

      RegCloseKey(hKey);
    }

    if (info.osVersion.empty()) {
      info.osVersion = "Windows";
    }
  }

  // CPU name from registry
  {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0,
                      KEY_READ, &hKey) == ERROR_SUCCESS) {
      char cpuName[256] = {};
      DWORD bufSize = sizeof(cpuName);
      DWORD type = REG_SZ;
      if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, &type,
                           (LPBYTE)cpuName, &bufSize) == ERROR_SUCCESS) {
        info.cpuName = cpuName;
        // Trim leading/trailing spaces
        size_t start = info.cpuName.find_first_not_of(' ');
        size_t end = info.cpuName.find_last_not_of(' ');
        if (start != std::string::npos) {
          info.cpuName = info.cpuName.substr(start, end - start + 1);
        }
      }
      RegCloseKey(hKey);
    }
    if (info.cpuName.empty()) {
      info.cpuName = "Unknown CPU";
    }
  }

  // CPU cores
  SYSTEM_INFO sysInfo;
  GetSystemInfo(&sysInfo);
  info.cpuLogicalCores = sysInfo.dwNumberOfProcessors;

  // Physical core count (approximate using GetLogicalProcessorInformation)
  {
    DWORD bufLen = 0;
    GetLogicalProcessorInformation(nullptr, &bufLen);
    if (bufLen > 0) {
      SYSTEM_LOGICAL_PROCESSOR_INFORMATION *buf =
          (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(bufLen);
      if (buf && GetLogicalProcessorInformation(buf, &bufLen)) {
        int physCores = 0;
        DWORD count = bufLen / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        for (DWORD i = 0; i < count; i++) {
          if (buf[i].Relationship == RelationProcessorCore) {
            physCores++;
          }
        }
        info.cpuCores = physCores;
      }
      free(buf);
    }
    if (info.cpuCores == 0)
      info.cpuCores = info.cpuLogicalCores;
  }

  // Total RAM
  MEMORYSTATUSEX memStatus;
  memStatus.dwLength = sizeof(memStatus);
  if (GlobalMemoryStatusEx(&memStatus)) {
    info.totalRAMBytes = memStatus.ullTotalPhys;
  }

  // Initial uptime
  updateDynamic();
}

void SystemInfoMonitor::updateDynamic() {
  // System uptime using GetTickCount64
  info.uptimeSeconds = GetTickCount64() / 1000;
}
