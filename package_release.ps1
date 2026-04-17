# HPC System Monitor - Release Packaging Script
# This script builds the project in Release mode and packages it into a ZIP file.

$ProjectName = "SystemMonitor"
$Version = "2.0"
$BuildDir = "build_release"
$OutDir = "dist"
$ZipFile = "$OutDir/${ProjectName}_v${Version}_Windows.zip"

Write-Host "--- Starting Release Build for $ProjectName v$Version ---" -ForegroundColor Cyan

# 1. Clean and Create Build Directory
if (Test-Path $BuildDir) {
    Remove-Item -Recurse -Force $BuildDir
}
New-Item -ItemType Directory -Path $BuildDir | Out-Null

# 2. Configure with CMake
Write-Host "Configuring Project..."
cmake -B $BuildDir -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed!"
    exit $LASTEXITCODE
}

# 3. Build Project
Write-Host "Building Project (Release Mode)..."
cmake --build $BuildDir --config Release --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed!"
    exit $LASTEXITCODE
}

# 4. Prepare Distribution Folder
Write-Host "Preparing Distribution Files..."
if (Test-Path $OutDir) {
    Remove-Item -Recurse -Force $OutDir
}
New-Item -ItemType Directory -Path $OutDir | Out-Null

# Define binary path (Visual Studio handles Release subfolder)
$BinPath = "$BuildDir/Release/${ProjectName}.exe"
if (-not (Test-Path $BinPath)) {
    # Fallback for other generators
    $BinPath = "$BuildDir/${ProjectName}.exe"
}

if (-not (Test-Path $BinPath)) {
    Write-Error "Executable not found at $BinPath"
    exit 1
}

# Copy Files
Copy-Item $BinPath -Destination "$OutDir/"
Copy-Item "README.md" -Destination "$OutDir/"
if (Test-Path "LICENSE") {
    Copy-Item "LICENSE" -Destination "$OutDir/"
}

# 5. Zip the Package
Write-Host "Creating ZIP: $ZipFile"
Compress-Archive -Path "$OutDir/*" -DestinationPath $ZipFile -Force

Write-Host "--- Release Package Created Successfully! ---" -ForegroundColor Green
Write-Host "Location: $ZipFile"
