#include "network_monitor.h"
#include <iphlpapi.h>

NetworkMonitor::NetworkMonitor()
    : prevBytesSent(0), prevBytesReceived(0), firstCall(true), peakDownload(0),
      peakUpload(0), pingCounter(0), lastPingValue(-1.0) {
  prevTime.QuadPart = 0;
}

NetworkInfo NetworkMonitor::getNetworkInfo() {
  NetworkInfo info = {};
  info.available = false;
  info.pingLatencyMs = -1.0;
  info.peakDownloadSpeed = peakDownload;
  info.peakUploadSpeed = peakUpload;

  DWORD dwSize = 0;
  GetIfTable(nullptr, &dwSize, FALSE);
  if (dwSize == 0)
    return info;

  MIB_IFTABLE *pIfTable = (MIB_IFTABLE *)malloc(dwSize);
  if (pIfTable == nullptr)
    return info;

  if (GetIfTable(pIfTable, &dwSize, FALSE) != NO_ERROR) {
    free(pIfTable);
    return info;
  }

  uint64_t totalSent = 0;
  uint64_t totalReceived = 0;
  uint64_t totalPktsSent = 0;
  uint64_t totalPktsRecv = 0;
  std::string bestAdapter = "Unknown";
  DWORD bestIfType = 0;
  uint64_t bestSpeed = 0;
  BYTE bestMac[6] = {};
  DWORD bestMacLen = 0;

  for (DWORD i = 0; i < pIfTable->dwNumEntries; i++) {
    MIB_IFROW &row = pIfTable->table[i];
    if (row.dwType == IF_TYPE_SOFTWARE_LOOPBACK)
      continue;
    if (row.dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
      continue;

    totalSent += row.dwOutOctets;
    totalReceived += row.dwInOctets;
    totalPktsSent += row.dwOutUcastPkts + row.dwOutNUcastPkts;
    totalPktsRecv += row.dwInUcastPkts + row.dwInNUcastPkts;

    if (bestAdapter == "Unknown" && (row.dwType == IF_TYPE_ETHERNET_CSMACD ||
                                     row.dwType == IF_TYPE_IEEE80211)) {
      char adapterName[256] = {};
      WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)row.bDescr, row.dwDescrLen,
                          adapterName, sizeof(adapterName), nullptr, nullptr);
      bestAdapter = adapterName;
      bestIfType = row.dwType;
      bestSpeed = row.dwSpeed / 1000000ULL;
      if (row.dwPhysAddrLen <= 6) {
        memcpy(bestMac, row.bPhysAddr, row.dwPhysAddrLen);
        bestMacLen = row.dwPhysAddrLen;
      }
    }
  }

  free(pIfTable);

  info.totalBytesSent = totalSent;
  info.totalBytesReceived = totalReceived;
  info.packetsSent = totalPktsSent;
  info.packetsReceived = totalPktsRecv;
  info.adapterName = bestAdapter;
  info.linkSpeedMbps = bestSpeed;
  info.available = true;

  if (bestIfType == IF_TYPE_IEEE80211)
    info.connectionType = "WiFi";
  else if (bestIfType == IF_TYPE_ETHERNET_CSMACD)
    info.connectionType = "Ethernet";
  else
    info.connectionType = "Other";

  if (bestMacLen >= 6) {
    char macStr[32];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             bestMac[0], bestMac[1], bestMac[2], bestMac[3], bestMac[4],
             bestMac[5]);
    info.macAddress = macStr;
  }

  // Speed calculation
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  ULARGE_INTEGER currentTime;
  currentTime.LowPart = ft.dwLowDateTime;
  currentTime.HighPart = ft.dwHighDateTime;

  if (firstCall) {
    info.uploadSpeed = 0.0;
    info.downloadSpeed = 0.0;
    firstCall = false;
  } else {
    double timeDiff =
        (double)(currentTime.QuadPart - prevTime.QuadPart) / 10000000.0;
    if (timeDiff > 0.0) {
      info.downloadSpeed =
          (double)(totalReceived - prevBytesReceived) / timeDiff;
      info.uploadSpeed = (double)(totalSent - prevBytesSent) / timeDiff;

      if (info.downloadSpeed < 0.0)
        info.downloadSpeed = 0.0;
      if (info.uploadSpeed < 0.0)
        info.uploadSpeed = 0.0;
      if (info.downloadSpeed > peakDownload)
        peakDownload = info.downloadSpeed;
      if (info.uploadSpeed > peakUpload)
        peakUpload = info.uploadSpeed;
    }
  }

  info.peakDownloadSpeed = peakDownload;
  info.peakUploadSpeed = peakUpload;
  prevBytesSent = totalSent;
  prevBytesReceived = totalReceived;
  prevTime = currentTime;

  getAdapterDetails(info);
  info.activeTcpConnections = getActiveTcpConnectionCount();

  // Ping disabled due to SDK header conflicts
  info.pingLatencyMs = -1.0;

  return info;
}

void NetworkMonitor::getAdapterDetails(NetworkInfo &info) {
  ULONG bufLen = 0;
  GetAdaptersInfo(nullptr, &bufLen);
  if (bufLen == 0)
    return;

  IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)malloc(bufLen);
  if (pAdapterInfo == nullptr)
    return;

  if (GetAdaptersInfo(pAdapterInfo, &bufLen) == NO_ERROR) {
    IP_ADAPTER_INFO *pAdapter = pAdapterInfo;
    while (pAdapter) {
      if ((pAdapter->Type == MIB_IF_TYPE_ETHERNET ||
           pAdapter->Type == IF_TYPE_IEEE80211) &&
          std::string(pAdapter->IpAddressList.IpAddress.String) != "0.0.0.0") {
        info.ipAddress = pAdapter->IpAddressList.IpAddress.String;
        break;
      }
      pAdapter = pAdapter->Next;
    }
  }

  free(pAdapterInfo);
  if (info.ipAddress.empty())
    info.ipAddress = "N/A";
}

int NetworkMonitor::getActiveTcpConnectionCount() {
  DWORD dwSize = 0;
  GetTcpTable(nullptr, &dwSize, TRUE);
  if (dwSize == 0)
    return 0;

  MIB_TCPTABLE *pTcpTable = (MIB_TCPTABLE *)malloc(dwSize);
  if (pTcpTable == nullptr)
    return 0;

  int count = 0;
  if (GetTcpTable(pTcpTable, &dwSize, TRUE) == NO_ERROR) {
    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
      if (pTcpTable->table[i].dwState == MIB_TCP_STATE_ESTAB)
        count++;
    }
  }
  free(pTcpTable);
  return count;
}

// Ping disabled - icmpapi.h has header conflicts with this SDK version
double NetworkMonitor::pingHost() { return -1.0; }
