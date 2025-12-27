# DRIP v1.0.1 Release

**Digital Reserve and Information Protocol**

## Downloads

| Platform | File | Size |
|----------|------|------|
| Linux x86_64 | `drip-v1.0.1-linux-x86_64.tar.gz` | ~12 MB |
| Windows | Coming soon (use WSL or build from source) | - |

## What's New in v1.0.1

### Critical Fixes

- **BIP9 Deployment Period Fix**: Corrected deployment periods from 2016 to 4032 blocks to match DRIP's 5-minute block time. Threshold adjusted proportionally (1815 → 3629 for 90%).

- **Security: HTTP Seed Validation**: Fixed potential shell injection vulnerability in HTTP seed fetching. Added strict URL validation that only allows safe characters.

- **BIP34Hash Correction**: Updated to actual block 1 hash (`00000000d39fcd1295730934cc51a9e39d7afc97ba98c1c7a35566763d9c05f0`).

### Upgrade Instructions

This is a recommended upgrade for all node operators. Simply replace your binaries and restart:

```bash
# Stop your node
./drip-cli -drip stop

# Extract new release
tar -xzf drip-v1.0.1-linux-x86_64.tar.gz

# Start node
./drip-v1.0.1-linux-x86_64/dripd -drip -printtoconsole
```

Your existing wallet and blockchain data will continue to work.

---

## What's Included

- `dripd` - DRIP daemon (full node)
- `drip-cli` - Command-line interface
- `drip-wallet` - Wallet tool
- `drip-tx` - Transaction tool
- `drip-util` - Utility tool

## Quick Start

```bash
# Extract
tar -xzf drip-v1.0.1-linux-x86_64.tar.gz
cd drip-v1.0.1-linux-x86_64

# Run node
./dripd -drip -printtoconsole

# In another terminal, create wallet and get address
./drip-cli -drip createwallet mywallet
./drip-cli -drip getnewaddress
```

## Network Specifications

| Parameter | Value |
|-----------|-------|
| Block Time | 5 minutes |
| Total Supply | 21,000,000 DRIP |
| Block Reward | 50 DRIP |
| P2P Port | 58333 |
| RPC Port | 58332 |
| Address Prefix | D (legacy) / drip (bech32) |

## Genesis Block

- **Hash:** `00000000ee44411d80e174879fbb52f89f8efeae2423a51486aac4db05907dcc`
- **Message:** "WSJ 25/Dec/2025 The Economic Divide Between Big and Small Companies Is Growing"

## Verification

```bash
# Verify genesis hash
./drip-cli -drip getblockhash 0
# Should return: 00000000ee44411d80e174879fbb52f89f8efeae2423a51486aac4db05907dcc
```

## Building from Source

```bash
git clone https://github.com/digital-reserve/DRIP.git
cd DRIP
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=OFF
cmake --build build -j$(nproc)
```

## License

MIT License - Based on Bitcoin Core
