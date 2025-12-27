# Linux/Unix Build Guide

This guide describes how to build DRIP on Linux and Unix systems.

## Quick Start

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y build-essential cmake pkgconf python3 libevent-dev libboost-dev libsqlite3-dev
git clone https://github.com/AnchorCoinDevelopment/DRIP.git
cd DRIP
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

### Fedora
```bash
sudo dnf install gcc-c++ cmake make python3 libevent-devel boost-devel sqlite-devel
git clone https://github.com/AnchorCoinDevelopment/DRIP.git
cd DRIP
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

### Arch Linux
```bash
sudo pacman -S cmake boost gcc git libevent make python sqlite
git clone https://github.com/AnchorCoinDevelopment/DRIP.git
cd DRIP
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

### Alpine
```bash
apk add build-base cmake linux-headers pkgconf python3 libevent-dev boost-dev sqlite-dev
git clone https://github.com/AnchorCoinDevelopment/DRIP.git
cd DRIP
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

Binaries: `build/bin/` → `dripd`, `drip-cli`, `drip-wallet`, `drip-tx`

---

## Build Options

| Option | Description |
|--------|-------------|
| `-DBUILD_GUI=ON` | Build Qt GUI (requires Qt6) |
| `-DENABLE_WALLET=OFF` | Disable wallet (no SQLite needed) |
| `-DWITH_ZMQ=ON` | Enable ZMQ notifications |
| `-DCMAKE_BUILD_TYPE=Release` | Optimized build |

Run `cmake -B build -LH` to see all options.

---

## Optional Dependencies

**GUI (Qt6):**
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-tools-dev qt6-l10n-tools libqrencode-dev

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qttools-devel qrencode-devel
```

**ZMQ notifications:**
```bash
# Ubuntu/Debian
sudo apt install libzmq3-dev

# Fedora
sudo dnf install zeromq-devel
```

---

## Running DRIP

```bash
# Start the daemon
./build/bin/dripd -drip

# Use CLI (in another terminal)
./build/bin/drip-cli -drip getblockchaininfo
./build/bin/drip-cli -drip getnetworkinfo
```

---

## Troubleshooting

### Low memory during compilation
```bash
# Use fewer parallel jobs
cmake --build build -j2

# Or disable debug symbols
cmake -B build -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g0" -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

### Clean rebuild
```bash
rm -rf build
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

### Use Clang instead of GCC
```bash
cmake -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DBUILD_GUI=OFF
```

---

For BSD systems, see `build-freebsd.md`, `build-openbsd.md`, or `build-netbsd.md`.
