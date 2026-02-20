#pragma once

#include <cstdint>
#include <string>
#include <windows.h>

struct NetworkInfo {
  // Traffic data
  uint64_t totalBytesSent;
  uint64_t totalBytesReceived;
  double uploadSpeed;   // bytes/sec
  double downloadSpeed; // bytes/sec

  // Peak speeds
  double peakDownloadSpeed;
  double peakUploadSpeed;

  // Adapter details
  std::string adapterName;
  std::string ipAddress;
  std::string macAddress;
  std::string connectionType; // "Ethernet", "WiFi", "Unknown"
  uint64_t linkSpeedMbps;     // Link speed in Mbps

  // Packet counters
  uint64_t packetsSent;
  uint64_t packetsReceived;

  // Active connections
  int activeTcpConnections;

  // Ping latency
  double pingLatencyMs; // -1 if failed

  bool available;
};

class NetworkMonitor {
public:
  NetworkMonitor();
  ~NetworkMonitor() = default;

  NetworkInfo getNetworkInfo();

private:
  uint64_t prevBytesSent;
  uint64_t prevBytesReceived;
  ULARGE_INTEGER prevTime;
  bool firstCall;

  double peakDownload;
  double peakUpload;

  // Helper methods
  void getAdapterDetails(NetworkInfo &info);
  int getActiveTcpConnectionCount();
  double pingHost();
  int pingCounter;
  double lastPingValue;
};
