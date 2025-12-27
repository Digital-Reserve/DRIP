# Windows Build Guide

This guide describes how to build DRIP on Windows using Visual Studio.

For cross-compiling options, please see [`build-windows.md`](./build-windows.md).

## Quick Start

If you just want to build DRIP quickly, run these commands in **Developer PowerShell** or **Developer Command Prompt**:

```powershell
# One-time setup: Install vcpkg
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat

# Clone and build DRIP
git clone https://github.com/AnchorCoinDevelopment/DRIP.git C:\drip
cd C:\drip
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
cmake --build build --config Release
```

Binaries will be in `build\bin\Release\` → `dripd.exe`, `drip-cli.exe`, etc.

---

## Detailed Instructions

### 1. Prerequisites

| Software | Download |
|----------|----------|
| Visual Studio 2019/2022/2024 | [visualstudio.microsoft.com](https://visualstudio.microsoft.com/) |
| Git for Windows | [git-scm.com](https://git-scm.com/downloads/win) |
| CMake 3.22+ | [cmake.org](https://cmake.org/download/) |

**Visual Studio Requirements:**
- Install the **"Desktop development with C++"** workload
- Any recent version works (2019, 2022, or later) - CMake auto-detects

### 2. Install vcpkg Package Manager

vcpkg handles all C++ dependencies automatically. Install it once:

**PowerShell:**
```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

**CMD:**
```cmd
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
```

### 3. Clone DRIP Repository

```powershell
git clone https://github.com/AnchorCoinDevelopment/DRIP.git C:\drip
cd C:\drip
```

### 4. Configure Build

Open **Developer PowerShell for VS** or **Developer Command Prompt** (not regular PowerShell/CMD):

**Clean any previous build (if exists):**

PowerShell:
```powershell
if (Test-Path build) { Remove-Item -Recurse -Force build }
```

CMD:
```cmd
if exist build rmdir /s /q build
```

**Configure:**
```powershell
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF
```

> **Note:** First configuration takes 10-20 minutes as vcpkg downloads and compiles dependencies.

### 5. Build

```powershell
cmake --build build --config Release
```

Add `-j N` for parallel builds (e.g., `-j 8` for 8 cores).

### 6. Verify

```powershell
.\build\bin\Release\dripd.exe --version
.\build\bin\Release\drip-cli.exe --version
```

---

## Troubleshooting

### "Generator does not match previous build"

Delete the build folder and reconfigure:
```powershell
Remove-Item -Recurse -Force build
# Then run cmake -B build ... again
```

### "Buildtrees path is too long"

Add a shorter path for vcpkg build files:
```powershell
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DBUILD_GUI=OFF -DVCPKG_INSTALL_OPTIONS=--x-buildtrees-root=C:\vcpkg\bt
```

### "Could not find any instance of Visual Studio"

- Make sure you're running from **Developer PowerShell/Command Prompt**, not regular PowerShell/CMD
- Or set the VS path manually: Start Menu → Visual Studio → "Developer PowerShell for VS 20XX"

### "Paths with embedded space may be handled incorrectly"

Clone to a path without spaces (e.g., `C:\drip` not `C:\My Projects\drip`)

### Antivirus Slowing Build

Add `C:\drip` and `C:\vcpkg` to Windows Defender exclusions for faster builds.

---

## Build Options

| Option | Description |
|--------|-------------|
| `-DBUILD_GUI=ON` | Build Qt GUI (requires Qt6) |
| `-DCMAKE_BUILD_TYPE=Release` | Optimized build |
| `-DENABLE_WALLET=OFF` | Disable wallet functionality |
| `-DWITH_ZMQ=ON` | Enable ZMQ notifications |

Run `cmake -B build -LH` to see all available options.

---

## Running DRIP

```powershell
# Start the daemon
.\build\bin\Release\dripd.exe -drip

# In another terminal, use CLI
.\build\bin\Release\drip-cli.exe -drip getblockchaininfo
```
