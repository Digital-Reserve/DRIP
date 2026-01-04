# DRIP Technical Overview

**Digital Reserve and Information Protocol**  
*Version 1.0 — January 2026*

---

## Abstract

DRIP is a cryptocurrency built on Bitcoin's battle-tested codebase, launched with a fresh genesis block on December 25, 2025. It preserves Bitcoin's proven security model and 21 million supply cap while providing a fair distribution opportunity—no premine, no ICO, and no early-adopter advantage from over a decade ago.

---

## 1. Motivation

Bitcoin's technology has proven itself over 15+ years as the most secure and reliable cryptocurrency. However, its distribution phase ended long ago. Early adopters hold significant portions of the supply, and new participants face a substantial barrier to entry.

DRIP addresses this by resetting the clock:

- **Same proven technology** — Bitcoin's SHA256d proof-of-work, UTXO model, and consensus rules
- **Fresh genesis block** — Everyone starts from zero
- **Fair launch** — No premine, no venture capital allocation, no insider advantages
- **Faster confirmations** — 5-minute blocks for improved usability

DRIP is not a fork of the Bitcoin blockchain. It is a new chain with its own genesis block, using the Bitcoin Core codebase.

---

## 2. Technical Specifications

| Parameter | Value |
|-----------|-------|
| **Algorithm** | SHA256d (Double SHA-256) |
| **Block Time** | 5 minutes |
| **Block Reward** | 50 DRIP |
| **Halving Interval** | 210,000 blocks (~2 years) |
| **Maximum Supply** | 21,000,000 DRIP |
| **Difficulty Adjustment** | Every 1,008 blocks (~3.5 days)* |
| **Difficulty Fork** | Block 36,108 (activation at 35,000; 4,032 → 1,008 blocks) |
| **P2P Port** | 58333 |
| **RPC Port** | 58332 |

*\*Current value after block 36,108 hard fork. Originally 4,032 blocks (~2 weeks).*

### Address Formats

| Type | Prefix | Example |
|------|--------|---------|
| P2PKH (Legacy) | D | `D7x9w3kZ...` |
| P2SH (Script) | E | `E3mN8pQ...` |
| Bech32 (SegWit) | drip1 | `drip1qxy2...` |
| Bech32m (Taproot) | drip1p | `drip1pxyz...` |

---

## 3. Genesis Block

The DRIP genesis block was mined on December 25, 2025, with a timestamp reference to current events following Bitcoin's tradition.

```
Timestamp:  December 25, 2025 00:00:00 UTC
Message:    "WSJ 25/Dec/2025 The Economic Divide Between Big and Small Companies Is Growing"
Hash:       00000000ef0945a11c9eab83c04c5dc6185477289bf25d695db9cf7cba00130c
Merkle:     9b04bdee3647d6c7ba0c537214e75eb8045d13d63e0eabffbd2aea95a9ebba5e
Nonce:      47837911
```

The genesis block coinbase is unspendable, consistent with Bitcoin's design.

---

## 4. Consensus Rules

DRIP inherits Bitcoin's consensus rules with all modern improvements active from block 1:

- **BIP34** — Block height in coinbase
- **BIP65** — CHECKLOCKTIMEVERIFY
- **BIP66** — Strict DER signatures
- **BIP68/112/113** — Relative lock-time (CSV)
- **BIP141/143/147** — Segregated Witness
- **BIP340/341/342** — Taproot and Schnorr signatures

This eliminates the need for soft fork activation periods and provides modern features immediately.

### Difficulty Adjustment

DRIP underwent a hard fork to improve difficulty responsiveness. The new rules activated at block 35,000, with the first adjustment under the new parameters occurring at block 36,108.

| Phase | Block Range | Adjustment Interval | Period |
|-------|-------------|---------------------|--------|
| **Pre-fork** | 1–36,107 | Every 4,032 blocks | ~14 days |
| **Post-fork** | 36,108+ | Every 1,008 blocks | ~3.5 days |

The fork reduced the adjustment window from 2 weeks to 3.5 days, allowing the network to respond faster to hashrate fluctuations—critical for a young chain with variable mining participation.

---

## 5. Supply Schedule

DRIP follows Bitcoin's emission schedule with 5-minute blocks:

| Era | Block Range | Reward | Era Total | Cumulative | % of Supply |
|-----|-------------|--------|-----------|------------|-------------|
| 1 | 0–209,999 | 50 DRIP | 10,500,000 | 10,500,000 | 50.00% |
| 2 | 210,000–419,999 | 25 DRIP | 5,250,000 | 15,750,000 | 75.00% |
| 3 | 420,000–629,999 | 12.5 DRIP | 2,625,000 | 18,375,000 | 87.50% |
| 4 | 630,000–839,999 | 6.25 DRIP | 1,312,500 | 19,687,500 | 93.75% |
| 5 | 840,000–1,049,999 | 3.125 DRIP | 656,250 | 20,343,750 | 96.88% |
| 6 | 1,050,000–1,259,999 | 1.5625 DRIP | 328,125 | 20,671,875 | 98.44% |
| 7 | 1,260,000–1,469,999 | 0.78125 DRIP | 164,063 | 20,835,938 | 99.22% |
| 8 | 1,470,000–1,679,999 | 0.39063 DRIP | 82,031 | 20,917,969 | 99.61% |

With 5-minute blocks, each era lasts ~2 years. Full supply approaches 21M asymptotically.

---

## 6. Network Architecture

### Peer Discovery

DRIP uses HTTP-based seed discovery via GitHub Pages:
- `https://digital-reserve.github.io/DRIP/seeds.json`

Nodes can also connect directly via:
- Tor onion addresses
- Manual peer addition (`-addnode`)

### Network Magic

The protocol message header uses ASCII "DRIP":
```
0x44 0x52 0x49 0x50 (D R I P)
```

---

## 7. Mining

DRIP uses SHA256d proof-of-work, making it compatible with Bitcoin ASIC miners and any SHA256d mining pool.

### Pool Mining

ASIC hardware recommended due to network difficulty. Available pools:

- **SuperAxe Pool:** https://superaxepool.com/
- **AltCoins Pool:** https://altcoinspool.cc/

Configure your ASIC miner with the pool's stratum address and your DRIP wallet address.

### Getting a Wallet Address

```bash
drip-cli -drip createwallet "mining"
drip-cli -drip getnewaddress
```

*Note: CPU mining via `generatetoaddress` is no longer practical due to network difficulty.*

---

## 8. Privacy

DRIP supports privacy-preserving network configurations via Tor:

```bash
# Tor-only operation
dripd -drip -proxy=127.0.0.1:9050 -onlynet=onion -listen
```

*Note: I2P support is inherited from Bitcoin Core but has not been extensively tested on DRIP.*

---

## 9. Differences from Bitcoin

| Aspect | Bitcoin | DRIP |
|--------|---------|------|
| Genesis Date | January 3, 2009 | December 25, 2025 |
| Block Time | 10 minutes | 5 minutes |
| Address Prefix | 1, 3, bc1 | D, E, drip1 |
| Default Port | 8333 | 58333 |
| Soft Fork Activation | Gradual | All active from block 1 |
| Distribution | ~94% mined | Fresh start |

---

## 10. Resources

- **Website:** https://dripscan.io
- **Source Code:** https://github.com/Digital-Reserve/DRIP
- **Discord:** https://discord.gg/jHy2yCMpWH
- **Block Explorer:** https://dripscan.io

---

## License

DRIP is released under the MIT License.

Based on Bitcoin Core: https://bitcoincore.org

---

*This document describes DRIP as of version 1.1.x. Specifications may be updated in future releases.*

