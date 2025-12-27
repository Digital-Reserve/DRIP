# DRIP v1.0.2 Release

**Digital Reserve and Information Protocol**

## ⚠️ CRITICAL: Genesis Block Fix - Upgrade Required

**This release fixes a genesis block timestamp error. All nodes must upgrade and reset their blockchain data.**

The previous genesis block had timestamp `1766611200` (Dec 24, 2025 21:20:00 UTC) but was documented as Dec 25, 2025. This has been corrected to `1766620800` (Dec 25, 2025 00:00:00 UTC).

## Downloads

| Platform | File | Size |
|----------|------|------|
| Linux x86_64 | `drip-v1.0.2-linux-x86_64.tar.gz` | ~13 MB |
| Windows | Coming soon (use WSL or build from source) | - |

## What's New in v1.0.2

### Critical Fixes

- **Genesis Block Timestamp Correction**: Fixed genesis block timestamp from `1766611200` (Dec 24, 2025 21:20:00 UTC) to `1766620800` (Dec 25, 2025 00:00:00 UTC). This ensures the genesis message date matches the actual block time.

- **New Genesis Block Hash**: `00000000ef0945a11c9eab83c04c5dc6185477289bf25d695db9cf7cba00130c`

- **Updated Genesis Nonce**: `47837911` (0x02d9f2d7)

### Code Cleanup

- Updated remaining "RAW" references to "DRIP" in genesis miner code

## Upgrade Instructions

**⚠️ IMPORTANT: This is a breaking change. You must delete your blockchain data.**

```bash
# Stop your node
./drip-cli -chain=drip stop

# Delete blockchain data (keeps wallet!)
rm -rf ~/.drip/drip/blocks ~/.drip/drip/chainstate
rm -f ~/.drip/drip/peers.dat ~/.drip/drip/banlist.json

# Extract new release
tar -xzf drip-v1.0.2-linux-x86_64.tar.gz
cd drip-v1.0.2-linux-x86_64

# Start node
./dripd -chain=drip -printtoconsole
```

Your wallet file is preserved but will have 0 balance since the blockchain was reset.

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
tar -xzf drip-v1.0.2-linux-x86_64.tar.gz
cd drip-v1.0.2-linux-x86_64

# Run node
./dripd -chain=drip -printtoconsole

# In another terminal, create wallet and get address
./drip-cli -chain=drip createwallet mywallet
./drip-cli -chain=drip getnewaddress
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

- **Hash:** `00000000ef0945a11c9eab83c04c5dc6185477289bf25d695db9cf7cba00130c`
- **Timestamp:** `1766620800` (Dec 25, 2025 00:00:00 UTC)
- **Nonce:** `47837911`
- **Message:** "WSJ 25/Dec/2025 The Economic Divide Between Big and Small Companies Is Growing"
- **Merkle Root:** `9b04bdee3647d6c7ba0c537214e75eb8045d13d63e0eabffbd2aea95a9ebba5e`

## Verification

```bash
# Verify genesis hash
./drip-cli -chain=drip getblockhash 0
# Should return: 00000000ef0945a11c9eab83c04c5dc6185477289bf25d695db9cf7cba00130c

# Verify genesis timestamp
./drip-cli -chain=drip getblock $(./drip-cli -chain=drip getblockhash 0) | grep time
# Should show: "time": 1766620800
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
