// Copyright (c) 2009-2010 Drip Nakamoto
// Copyright (c) 2009-present The DRIP developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>
#include <util/check.h>

/** DRIP: Check if a height is a difficulty adjustment boundary */
static bool IsDifficultyAdjustmentBoundary(int height, const Consensus::Params& params)
{
    // Before fork: use original 4032-block interval
    if (params.nDifficultyForkHeight == 0 || height < params.nDifficultyForkHeight) {
        return height % params.DifficultyAdjustmentInterval() == 0;
    }
    // At fork height: do NOT adjust (avoids measuring wrong period)
    // First post-fork adjustment will happen at nDifficultyForkHeight + 1008
    if (height == params.nDifficultyForkHeight) {
        return false;
    }
    // After fork: use new 1008-block interval starting from fork height
    return (height - params.nDifficultyForkHeight) % params.DifficultyAdjustmentIntervalAtHeight(height) == 0;
}

/** DRIP: Get the height of the first block in the current difficulty period */
static int GetDifficultyPeriodStart(int height, const Consensus::Params& params)
{
    int64_t interval = params.DifficultyAdjustmentIntervalAtHeight(height);
    
    // Before fork: use original interval
    if (params.nDifficultyForkHeight == 0 || height < params.nDifficultyForkHeight) {
        return height - (height % interval);
    }
    // At or after fork: periods start from fork height
    int heightSinceFork = height - params.nDifficultyForkHeight;
    int periodsSinceFork = heightSinceFork / interval;
    return params.nDifficultyForkHeight + (periodsSinceFork * interval);
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();
    int nextHeight = pindexLast->nHeight + 1;

    // Only change once per difficulty adjustment interval
    if (!IsDifficultyAdjustmentBoundary(nextHeight, params))
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then it MUST be a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && !IsDifficultyAdjustmentBoundary(pindex->nHeight, params) && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Get the first block of this difficulty period
    int nHeightFirst = GetDifficultyPeriodStart(pindexLast->nHeight, params);
    // For the first block of a period, we need the previous period's start
    if (nHeightFirst == pindexLast->nHeight) {
        // We're calculating for the first block after an adjustment
        // Go back one full period
        int64_t interval = params.DifficultyAdjustmentIntervalAtHeight(nextHeight);
        nHeightFirst = pindexLast->nHeight - (interval - 1);
        // Handle fork boundary: don't go before fork for post-fork calculations
        if (params.nDifficultyForkHeight > 0 && nextHeight > params.nDifficultyForkHeight && nHeightFirst < params.nDifficultyForkHeight) {
            nHeightFirst = params.nDifficultyForkHeight - 1; // Use block just before fork as start
        }
    }
    if (nHeightFirst < 0) nHeightFirst = 0;
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params, nextHeight);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params, int nextHeight)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Use height-aware timespan for DRIP fork
    int64_t nPowTargetTimespan = params.PowTargetTimespanAtHeight(nextHeight);

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < nPowTargetTimespan/4)
        nActualTimespan = nPowTargetTimespan/4;
    if (nActualTimespan > nPowTargetTimespan*4)
        nActualTimespan = nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;

    // Special difficulty rule for Testnet4
    if (params.enforce_BIP94) {
        // Here we use the first block of the difficulty period. This way
        // the real difficulty is always preserved in the first block as
        // it is not allowed to use the min-difficulty exception.
        int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentIntervalAtHeight(nextHeight)-1);
        const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
        bnNew.SetCompact(pindexFirst->nBits);
    } else {
        bnNew.SetCompact(pindexLast->nBits);
    }

    bnNew *= nActualTimespan;
    bnNew /= nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

// Check that on difficulty adjustments, the new difficulty does not increase
// or decrease beyond the permitted limits.
bool PermittedDifficultyTransition(const Consensus::Params& params, int64_t height, uint32_t old_nbits, uint32_t new_nbits)
{
    if (params.fPowAllowMinDifficultyBlocks) return true;

    if (IsDifficultyAdjustmentBoundary(height, params)) {
        int64_t nPowTargetTimespan = params.PowTargetTimespanAtHeight(height);
        int64_t smallest_timespan = nPowTargetTimespan/4;
        int64_t largest_timespan = nPowTargetTimespan*4;

        const arith_uint256 pow_limit = UintToArith256(params.powLimit);
        arith_uint256 observed_new_target;
        observed_new_target.SetCompact(new_nbits);

        // Calculate the largest difficulty value possible:
        arith_uint256 largest_difficulty_target;
        largest_difficulty_target.SetCompact(old_nbits);
        largest_difficulty_target *= largest_timespan;
        largest_difficulty_target /= nPowTargetTimespan;

        if (largest_difficulty_target > pow_limit) {
            largest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 maximum_new_target;
        maximum_new_target.SetCompact(largest_difficulty_target.GetCompact());
        if (maximum_new_target < observed_new_target) return false;

        // Calculate the smallest difficulty value possible:
        arith_uint256 smallest_difficulty_target;
        smallest_difficulty_target.SetCompact(old_nbits);
        smallest_difficulty_target *= smallest_timespan;
        smallest_difficulty_target /= nPowTargetTimespan;

        if (smallest_difficulty_target > pow_limit) {
            smallest_difficulty_target = pow_limit;
        }

        // Round and then compare this new calculated value to what is
        // observed.
        arith_uint256 minimum_new_target;
        minimum_new_target.SetCompact(smallest_difficulty_target.GetCompact());
        if (minimum_new_target > observed_new_target) return false;
    } else if (old_nbits != new_nbits) {
        return false;
    }
    return true;
}

// Bypasses the actual proof of work check during fuzz testing with a simplified validation checking whether
// the most significant bit of the last byte of the hash is set.
bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    if (EnableFuzzDeterminism()) return (hash.data()[31] & 0x80) == 0;
    return CheckProofOfWorkImpl(hash, nBits, params);
}

std::optional<arith_uint256> DeriveTarget(unsigned int nBits, const uint256 pow_limit)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(pow_limit))
        return {};

    return bnTarget;
}

bool CheckProofOfWorkImpl(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    auto bnTarget{DeriveTarget(nBits, params.powLimit)};
    if (!bnTarget) return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
