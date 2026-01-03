# Windows Build Guide (MSVC)

This guide describes how to build DRIP on Windows using Visual Studio and vcpkg.

For cross-compiling from Linux/WSL, see [`build-windows.md`](./build-windows.md).

---

## Quick Start

Already have Visual Studio and vcpkg installed? Run these commands from the DRIP source directory.

**CMD (Command Prompt):**
```cmd
cd C:\path\to\DRIP

cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
cmake --build build --config Release
```

**PowerShell:**
```powershell
cd C:\path\to\DRIP

cmake -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
cmake --build build --config Release
```

> **Important:** Replace `C:/vcpkg` with your actual vcpkg install path if different.

Binaries will be in `build\bin\Release\` → `dripd.exe`, `drip-cli.exe`, etc.

---

## Full Setup Guide

### Step 1: Install Prerequisites

| Software | Download | Notes |
|----------|----------|-------|
| Visual Studio 2022 | [visualstudio.microsoft.com](https://visualstudio.microsoft.com/) | Select **"Desktop development with C++"** workload |
| Git for Windows | [git-scm.com](https://git-scm.com/downloads/win) | Use default settings |
| CMake 3.22+ | [cmake.org](https://cmake.org/download/) | Check "Add CMake to PATH" during install |

> **Note:** Visual Studio 2019 or later works fine. The Community edition is free.

### Step 2: Install vcpkg

Open **PowerShell** and run:

```powershell
# Choose where to install vcpkg (adjust path as needed)
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Set environment variable so CMake can find it
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "User")
$env:VCPKG_ROOT = "C:\vcpkg"
```

> **Tip:** You can install vcpkg anywhere you want. Just update `VCPKG_ROOT` accordingly.

### Step 3: Get the DRIP Source Code

```powershell
cd C:\
git clone https://github.com/Digital-Reserve/DRIP.git
cd DRIP
```

Or if you already have the source code, just navigate to it:

```powershell
cd C:\path\to\your\DRIP
```

### Step 4: Open Developer Command Prompt

**Important:** You must use the Visual Studio Developer command prompt, not regular PowerShell/CMD.

**How to open it:**
1. Press Windows key
2. Search for **"Developer PowerShell for VS 2022"** (or your VS version)
3. Run it

Then navigate to your DRIP source:

```powershell
cd C:\path\to\your\DRIP
```

### Step 5: Configure the Build

From your DRIP directory, run:

```powershell
# Clean any previous build attempts (if any)
if (Test-Path build) { Remove-Item -Recurse -Force build }

# Configure with vcpkg
cmake -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
```

**For CMD instead of PowerShell:**
```cmd
if exist build rmdir /s /q build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
```

> **Note:** Replace `C:/vcpkg` with your actual vcpkg install path if different.

> **Note:** The first build takes 10-20 minutes while vcpkg downloads and compiles dependencies. Subsequent builds are much faster.

### Step 6: Build

```powershell
cmake --build build --config Release
```

For faster parallel builds, add the `-j` flag with number of CPU cores:

```powershell
cmake --build build --config Release -j 8
```

### Step 7: Verify the Build

```powershell
.\build\bin\Release\dripd.exe --version
.\build\bin\Release\drip-cli.exe --version
```

You should see version output if the build succeeded.

---

## Running DRIP

From your DRIP directory:

```powershell
# Start the daemon
.\build\bin\Release\dripd.exe -drip

# In another terminal, use the CLI
.\build\bin\Release\drip-cli.exe -drip getblockchaininfo
```

---

## Build Options

| Option | Description |
|--------|-------------|
| `-DBUILD_GUI=ON` | Build Qt GUI wallet (requires additional Qt6 setup) |
| `-DENABLE_WALLET=OFF` | Disable wallet functionality |
| `-DWITH_ZMQ=ON` | Enable ZMQ notifications |

To see all available options:
```powershell
cmake -B build -LH
```

---

## Troubleshooting

### "Could not find CMakeLists.txt" or "The source directory does not exist"

You're not in the DRIP source directory. Make sure you `cd` to the correct location:

```powershell
cd C:\path\to\your\DRIP
dir CMakeLists.txt  # This should show the file exists
```

### "VCPKG_ROOT is not defined" or toolchain file not found

Make sure `VCPKG_ROOT` environment variable is set:

```powershell
# Check if it's set
echo $env:VCPKG_ROOT

# If empty, set it (adjust path to your vcpkg location)
$env:VCPKG_ROOT = "C:\vcpkg"
```

Or use the full path directly:
```powershell
cmake -B build -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
```

### "Generator does not match previous build"

Delete the build folder and reconfigure:

```powershell
Remove-Item -Recurse -Force build
cmake -B build ...  # Run your cmake configure command again
```

### "Could not find any instance of Visual Studio"

- You're not running from Developer PowerShell/Command Prompt
- Open it from: Start Menu → "Developer PowerShell for VS 2022"

### "Buildtrees path is too long"

Windows has path length limits. Add a shorter vcpkg build path:

**CMD:**
```cmd
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF -DVCPKG_INSTALL_OPTIONS=--x-buildtrees-root=C:\bt
```

**PowerShell:**
```powershell
cmake -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF -DVCPKG_INSTALL_OPTIONS=--x-buildtrees-root=C:\bt
```

### "Paths with embedded space may be handled incorrectly"

Clone to a path without spaces:
- ✅ Good: `C:\DRIP` or `C:\dev\DRIP`
- ❌ Bad: `C:\My Projects\DRIP`

### Build is very slow

1. Add your project folder to Windows Defender exclusions
2. Add vcpkg folder to exclusions too
3. Use `-j N` flag for parallel builds (e.g., `-j 8`)

---

## Summary Cheat Sheet

**CMD (most common):**
```cmd
cd C:\path\to\your\DRIP
if exist build rmdir /s /q build
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
cmake --build build --config Release -j 8
build\bin\Release\dripd.exe --version
```

**PowerShell:**
```powershell
cd C:\path\to\your\DRIP
if (Test-Path build) { Remove-Item -Recurse -Force build }
cmake -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
cmake --build build --config Release -j 8
.\build\bin\Release\dripd.exe --version
```

> Replace `C:/vcpkg` with your vcpkg install path.
