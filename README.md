# System Monitor v2.0

A modern desktop application for real-time system resource monitoring with a premium dark UI.

## Features

### Overview Tab
- **CPU**: Total usage with live graph + per-core usage bars
- **Memory**: RAM usage with live graph + usage breakdown
- **Disk**: C: drive usage with progress bar
- **GPU**: GPU utilization with live graph + VRAM tracking
- **Power**: Battery status, AC/Battery mode, remaining time

### Processes Tab
- Top 15 processes sorted by memory usage
- PID, process name, memory size, and visual memory bar
- Scrollable table with resizable columns

### Network Tab
- Real-time download/upload speed graphs
- Speed display in B/s, KB/s, or MB/s
- Total traffic statistics
- Active network adapter info

### Sidebar
- Circular gauge widgets for CPU, Memory, Disk, GPU, and Battery
- Smooth animated value transitions
- Color-coded status (green/yellow/red)

## Requirements

### Software
- Windows 10 or later
- CMake 3.15 or later
- Visual Studio 2019/2022 or MinGW compiler
- OpenGL support (usually comes with the system)

### Hardware
- Any Windows computer
- Compatible GPU drivers for GPU monitoring (optional)

## Installation and Build

### Method 1: Using Build Script (Recommended)

```powershell
# Navigate to project directory
cd "d:/C_C++ Projects/System Monitor"

# Run build script
./build.ps1
```

### Method 2: Manual CMake

```powershell
# Navigate to project directory
cd "d:/C_C++ Projects/System Monitor"

# Generate build files
cmake -B build -G "Visual Studio 17 2022"

# Build project
cmake --build build --config Release

# Run the program
./build/Release/SystemMonitor.exe
```

### Method 3: Using Visual Studio

1. Open Visual Studio 2019 or 2022
2. Select File > Open > CMake
3. Choose the CMakeLists.txt file
4. Build > Build All (F7)
5. Debug > Start (F5)

## Usage

The application opens with three navigation tabs:

- **Overview**: Live graphs for CPU, Memory, GPU. Per-core CPU bars. Disk and Power status panels.
- **Processes**: Table of top 15 memory-consuming processes with visual bars.
- **Network**: Download/Upload speed graphs with traffic statistics.

A sidebar on the left shows circular gauge widgets for quick system health overview. All data updates every second with smooth animations.

## Technical Details

### Technologies Used
- C++17 standard
- ImGui (Dear ImGui) - GUI framework with custom drawn widgets
- GLFW - Window management
- OpenGL 3.0 - Graphics rendering
- Windows PDH API - CPU and GPU monitoring
- Windows API - Memory, disk, power, process, and network monitoring
- PSAPI - Process memory information
- IP Helper API - Network traffic statistics

### Project Structure
```
System Monitor/
├── src/                       # Source code files
│   ├── main.cpp              # Main program
│   ├── cpu_monitor.cpp       # CPU monitoring (total + per-core)
│   ├── memory_monitor.cpp    # Memory monitoring
│   ├── disk_monitor.cpp      # Disk monitoring
│   ├── gpu_monitor.cpp       # GPU monitoring
│   ├── power_monitor.cpp     # Power monitoring
│   ├── process_monitor.cpp   # Process enumeration
│   ├── network_monitor.cpp   # Network traffic monitoring
│   └── gui.cpp               # Graphical interface
├── include/                   # Header files
├── external/                  # External libraries
│   ├── imgui/                # ImGui library
│   └── glfw/                 # GLFW library
└── CMakeLists.txt            # CMake configuration
```

## Troubleshooting

### "Windows.h not found" error
Try compiling using the MSVC compiler in Visual Studio. You can ignore IntelliSense errors in VS Code.

### GPU monitoring not working
GPU performance counters may not be available on some systems. The program will display a "not available" message in the GPU widget.

### CMake not found
If Visual Studio is installed, you can open the CMakeLists.txt file directly in Visual Studio.

## License

MIT License

## Developer Notes

This project was developed for learning purposes. It is suitable for practicing Windows system programming, GUI development, and real-time data monitoring.
