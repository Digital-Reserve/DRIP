# DRIP

**Digital Reserve and Information Protocol**

## What is DRIP?

DRIP is Bitcoin, reset to zero. 

Same proven technology. Same 21 million supply. Same SHA256 mining. But starting fresh - no $100K entry barrier, no whales from 2009 sitting on millions of coins. Just a fair launch where everyone starts equal.

### Why DRIP?

Bitcoin works. The code is battle-tested over 15+ years. But the distribution is done - early adopters hold massive bags while newcomers buy at all-time highs.

DRIP fixes this by resetting the clock:
- **Same Bitcoin codebase** - proven, secure, reliable
- **Fresh genesis block** - December 25, 2025
- **Starting price: $0** - early days again
- **Faster blocks** - 5 minutes instead of 10
- **Fair launch** - no premine, no ICO, no VC allocation

## Network Specifications

| Parameter | Value |
|-----------|-------|
| Block Time | 5 minutes (2x faster than BTC) |
| Total Supply | 21,000,000 DRIP |
| Block Reward | 50 DRIP (halves every 210,000 blocks) |
| Algorithm | SHA256d (same as Bitcoin) |
| P2P Port | 58333 |
| RPC Port | 58332 |
| Address Prefix | `D` (base58) / `drip` (bech32) |

## Genesis Block

| Field | Value |
|-------|-------|
| Message | "WSJ 25/Dec/2025 The Economic Divide Between Big and Small Companies Is Growing" |
| Timestamp | December 25, 2025 00:00:00 UTC |
| Hash | `00000000ee44411d80e174879fbb52f89f8efeae2423a51486aac4db05907dcc` |

## Quick Start

### Run a Node
```bash
# Windows
dripd.exe -drip

# Linux/macOS
./dripd -drip
```

### Create Wallet & Mine
```bash
drip-cli -drip createwallet "mywallet"
drip-cli -drip getnewaddress
drip-cli -drip generatetoaddress 1 YOUR_ADDRESS
```

### Connect to Peers
```bash
dripd -drip -addnode=IP_ADDRESS:58333
```

## Building from Source

### Windows
```powershell
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=OFF
cmake --build build
```

### Linux/macOS
```bash
cmake -B build -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

Binaries: `build/bin/dripd`, `drip-cli`, `drip-wallet`, `drip-tx`

## Privacy

Run over Tor for anonymity:
```bash
dripd -drip -proxy=127.0.0.1:9050 -onlynet=onion -listen
```

## Technical Details

- All Bitcoin consensus rules apply
- BIP34, BIP65, BIP66, CSV, Segwit, Taproot active from block 1
- Difficulty adjustment every 4032 blocks (2 weeks at 5-min blocks)
- Genesis block reward is unspendable (like Bitcoin)

## Going Public

1. **Seed Nodes** - Run `dripd -drip -listen` on VPS servers
2. **Add Seeds** - Edit `src/kernel/chainparams.cpp`, add to `vSeeds`
3. **Open Port 58333** - Allow incoming connections

## Documentation

- [Build on Windows](doc/build-windows-msvc.md)
- [Build on Linux](doc/build-unix.md)
- [Build on macOS](doc/build-osx.md)
- [Tor Setup](doc/tor.md)

## License

MIT License. See [COPYING](COPYING).

Based on Bitcoin Core - https://bitcoincore.org
