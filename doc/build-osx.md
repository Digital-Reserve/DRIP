# macOS Build Guide

This guide describes how to build DRIP on macOS.

## Quick Start

```bash
# Install dependencies
xcode-select --install
brew install cmake boost pkgconf libevent sqlite

# Clone and build
git clone https://github.com/AnchorCoinDevelopment/DRIP.git
cd DRIP
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(sysctl -n hw.ncpu)
```

Binaries will be in `build/bin/` → `dripd`, `drip-cli`, etc.

---

## Detailed Instructions

### 1. Xcode Command Line Tools

```bash
xcode-select --install
```

Click `Install` when the popup appears.

### 2. Homebrew Package Manager

Install from [brew.sh](https://brew.sh) if not already installed.

### 3. Install Dependencies

```bash
brew install cmake boost pkgconf libevent sqlite
```

**Optional dependencies:**
```bash
brew install capnp      # For IPC support
brew install qt@6       # For GUI
brew install qrencode   # For QR codes in GUI
brew install zeromq     # For ZMQ notifications
```

### 4. Clone DRIP Repository

```bash
git clone https://github.com/AnchorCoinDevelopment/DRIP.git
cd DRIP
```

---

## Building

### 5. Configure

```bash
cmake -B build -DBUILD_GUI=OFF
```

**With GUI:**
```bash
cmake -B build -DBUILD_GUI=ON
```

Run `cmake -B build -LH` to see all available options.

### 6. Compile

```bash
cmake --build build -j$(sysctl -n hw.ncpu)
```

---

## Running DRIP

```bash
# Start the daemon
./build/bin/dripd -drip

# In another terminal
./build/bin/drip-cli -drip getblockchaininfo
./build/bin/drip-cli -drip getnetworkinfo
```

Data directory: `~/Library/Application Support/DRIP/`

---

## Troubleshooting

### Missing dependencies
```bash
brew install cmake boost pkgconf libevent sqlite
```

### Clean rebuild
```bash
rm -rf build
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(sysctl -n hw.ncpu)
```

### Check versions
```bash
cmake --version   # Need 3.22+
brew --version
```
