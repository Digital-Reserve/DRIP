# Building and Running DRIP on Ubuntu

This guide covers building DRIP from source and setting up a node on Ubuntu.

## Prerequisites

Install build dependencies:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake pkgconf python3 libevent-dev libboost-dev libsqlite3-dev
```

**Minimum versions:** CMake 3.22+, GCC 11+ or Clang 16+

Check your versions:
```bash
cmake --version
gcc --version
```

## Building DRIP

```bash
# Clone the repository (skip if you already have it)
git clone https://github.com/AnchorCoinDevelopment/DRIP.git
cd DRIP

# Configure the build
cmake -B build -DBUILD_GUI=OFF

# Build (use -j$(nproc) for parallel compilation)
cmake --build build -j$(nproc)
```

The binaries will be in `build/bin/`:
- `dripd` - The DRIP daemon
- `drip-cli` - Command-line interface
- `drip-tx` - Transaction tool
- `drip-util` - Utility tool
- `drip-wallet` - Wallet tool

## Troubleshooting Build Issues

### CMake not found or too old
```bash
# Install latest CMake from Kitware repository
sudo apt-get install -y software-properties-common
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc | sudo apt-key add -
sudo apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main"
sudo apt-get update && sudo apt-get install cmake
```

### Low memory errors during compilation
```bash
# Use fewer parallel jobs, or add swap
cmake --build build -j2  # Use 2 cores instead of all

# Or reduce debug info
cmake -B build -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g0"
cmake --build build -j$(nproc)
```

### Missing dependencies error
```bash
# Check what's missing from the cmake output, common ones:
sudo apt-get install libzmq3-dev         # For ZMQ support
sudo apt-get install qt6-base-dev        # For GUI
sudo apt-get install libcapnp-dev capnproto  # For IPC
```

### Clean rebuild
```bash
rm -rf build
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

## Setting Up a DRIP Node with Tor

### Step 1: Run the Setup Script

The setup script will install Tor, configure it, and create the DRIP configuration:

```bash
chmod +x contrib/setup-drip-node.sh
./contrib/setup-drip-node.sh
```

This script will:
- Install Tor if not already installed
- Configure Tor with ControlPort for onion service creation
- Add your user to the `debian-tor` group
- Create DRIP configuration at `~/.drip/bitcoin.conf`

**Important:** After running the setup script, you must log out and back in (or restart your terminal session) for the `debian-tor` group membership to take effect. Alternatively, you can use the startup script which handles this automatically.

### Step 2: Start the Node

Use the provided startup script (handles Tor group permissions automatically):

```bash
./start-drip-node.sh
```

Or start manually:

```bash
# If you're in the debian-tor group:
./build/bin/dripd -drip -printtoconsole

# Or use sg to run with the group:
sg debian-tor -c "./build/bin/dripd -drip -printtoconsole"
```

### Step 3: Get Your Onion Address

Wait 1-2 minutes for the onion address to be generated, then check:

```bash
./build/bin/drip-cli -drip getnetworkinfo | jq '.localaddresses'
```

Or use the helper script:

```bash
./get-onion-address.sh
```

### Step 4: Connect to Other Nodes

To connect to other DRIP nodes, add them to your `~/.drip/bitcoin.conf`:

```
addnode=xc4dnfjzk7kyyda7jms5xngaldzvysi3lxkfpknhc3rb3ugyj7v2ahid.onion:58333
```

Then restart your node.

## Troubleshooting

### Tor Authentication Issues

If you see errors about Tor authentication:
- Make sure you're in the `debian-tor` group: `groups | grep debian-tor`
- If not, log out and back in after running the setup script
- Or use `sg debian-tor -c "./build/bin/dripd -drip -printtoconsole"`

### Onion Address Not Appearing

- Wait 2-3 minutes after starting the node
- Check that Tor is running: `systemctl status tor`
- Check node logs for Tor connection messages
- Verify Tor control port is accessible

## Useful Commands

```bash
# Check network info
./build/bin/drip-cli -drip getnetworkinfo

# Check connections
./build/bin/drip-cli -drip getconnectioncount

# Check blockchain info
./build/bin/drip-cli -drip getblockchaininfo

# Check mining info
./build/bin/drip-cli -drip getmininginfo
```

