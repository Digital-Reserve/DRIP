// Copyright (c) 2025 The DRIP developers
// Distributed under the MIT software license

#include <kernel/chainparams.h>
#include <pow.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <script/interpreter.h>
#include <consensus/merkle.h>
#include <uint256.h>
#include <util/strencodings.h>

#include <iostream>
#include <iomanip>
#include <chrono>

using namespace util::hex_literals;

int main() {
    std::cout << "=== DRIP Genesis Block Miner ===" << std::endl;
    std::cout << std::endl;
    
    // Get DRIP chain params
    const auto params = CChainParams::Drip();
    const auto& consensus = params->GetConsensus();
    
    // Genesis block parameters
    const char* drip_genesis_msg = "WSJ 25/Dec/2025 The Economic Divide Between Big and Small Companies Is Growing";
    const CScript drip_genesis_script = CScript() << "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"_hex << OP_CHECKSIG;
    
    uint32_t nTime = 1766620800; // Dec 25, 2025 00:00:00 UTC (correct timestamp)
    uint32_t nBits = 0x1d00ffff;
    int32_t nVersion = 1;
    CAmount genesisReward = 50 * COIN;
    
    std::cout << "Genesis message: " << drip_genesis_msg << std::endl;
    std::cout << "Target time: " << nTime << " (Dec 25, 2025 00:00:00 UTC)" << std::endl;
    std::cout << "Target difficulty: 0x" << std::hex << nBits << std::dec << std::endl;
    std::cout << std::endl;
    std::cout << "Mining genesis block..." << std::endl;
    std::cout << "(This may take a while depending on your CPU)" << std::endl;
    std::cout << std::endl;
    
    auto start_time = std::chrono::steady_clock::now();
    
    // Create genesis block
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)drip_genesis_msg, (const unsigned char*)drip_genesis_msg + strlen(drip_genesis_msg));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = drip_genesis_script;
    
    CBlock genesis;
    genesis.nTime = nTime;
    genesis.nBits = nBits;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    
    // Mine the block
    uint32_t nonce = 0;
    bool found = false;
    uint256 hash;
    
    std::cout << "Merkle root: " << genesis.hashMerkleRoot.ToString() << std::endl;
    std::cout << "Starting nonce search..." << std::endl;
    std::cout << std::endl;
    
    while (nonce < 0xFFFFFFFF && !found) {
        genesis.nNonce = nonce;
        hash = genesis.GetHash();
        
        if (CheckProofOfWork(hash, nBits, consensus)) {
            found = true;
            break;
        }
        
        nonce++;
        
        // Progress update every 10 million nonces
        if (nonce % 10000000 == 0) {
            auto elapsed = std::chrono::steady_clock::now() - start_time;
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
            if (seconds > 0) {
                double hashes_per_sec = nonce / (double)seconds;
                std::cout << "Tried " << std::setw(10) << nonce << " nonces (" 
                          << std::fixed << std::setprecision(0) << hashes_per_sec 
                          << " hashes/sec)" << std::endl;
            }
        }
    }
    
    auto elapsed = std::chrono::steady_clock::now() - start_time;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    
    if (found) {
        std::cout << std::endl;
        std::cout << "=== GENESIS BLOCK MINED SUCCESSFULLY ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Block Hash: " << hash.ToString() << std::endl;
        std::cout << "Merkle Root: " << genesis.hashMerkleRoot.ToString() << std::endl;
        std::cout << "Nonce: " << genesis.nNonce << " (0x" << std::hex << genesis.nNonce << std::dec << ")" << std::endl;
        std::cout << "Time: " << genesis.nTime << std::endl;
        std::cout << "Bits: 0x" << std::hex << genesis.nBits << std::dec << std::endl;
        std::cout << "Version: " << genesis.nVersion << std::endl;
        std::cout << "Time taken: " << seconds << " seconds" << std::endl;
        std::cout << std::endl;
        std::cout << "=== UPDATE CODE WITH THESE VALUES ===" << std::endl;
        std::cout << std::endl;
        std::cout << "In src/kernel/chainparams.cpp, update CDripParams constructor:" << std::endl;
        std::cout << std::endl;
        std::cout << "        genesis = CreateGenesisBlock(drip_genesis_msg," << std::endl;
        std::cout << "                drip_genesis_script," << std::endl;
        std::cout << "                " << genesis.nTime << ", " << genesis.nNonce 
                  << ", 0x" << std::hex << genesis.nBits << std::dec << ", 1, 50 * COIN);" << std::endl;
        std::cout << "        consensus.hashGenesisBlock = genesis.GetHash();" << std::endl;
        std::cout << "        assert(consensus.hashGenesisBlock == uint256{\"" 
                  << hash.ToString() << "\"});" << std::endl;
        std::cout << "        assert(genesis.hashMerkleRoot == uint256{\"" 
                  << genesis.hashMerkleRoot.ToString() << "\"});" << std::endl;
        std::cout << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "Failed to find valid nonce in range 0-4294967295." << std::endl;
        std::cout << "Try adjusting the timestamp slightly and try again." << std::endl;
        std::cout << std::endl;
        return 1;
    }
}
