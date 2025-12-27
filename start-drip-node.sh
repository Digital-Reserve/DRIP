#!/bin/bash
# DRIP Node Startup Script
# This script starts the DRIP node with proper Tor group permissions

DRIP_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DRIPD="$DRIP_ROOT/build/bin/dripd"

if [ ! -f "$DRIPD" ]; then
    echo "Error: DRIP daemon not found at $DRIPD"
    echo "Please build the project first: cmake --build build"
    exit 1
fi

# Check if user is in debian-tor group
if groups | grep -q debian-tor || groups | grep -q tor; then
    # User is in group, check if sg is available for easy startup
    if command -v sg &> /dev/null; then
        echo "Starting DRIP node with Tor support..."
        exec sg debian-tor -c "$DRIPD -drip -printtoconsole" 2>/dev/null || \
        exec sg tor -c "$DRIPD -drip -printtoconsole"
    else
        # Fallback: try to start directly (may work if user logged out/in)
        echo "Starting DRIP node..."
        exec "$DRIPD" -drip -printtoconsole
    fi
else
    # User not in group - try sg anyway (in case they were just added)
    if command -v sg &> /dev/null; then
        echo "Starting DRIP node with Tor support (using sg)..."
        exec sg debian-tor -c "$DRIPD -drip -printtoconsole" 2>/dev/null || \
        exec sg tor -c "$DRIPD -drip -printtoconsole" 2>/dev/null || {
            echo "Warning: Could not start with Tor group. Starting anyway..."
            exec "$DRIPD" -drip -printtoconsole
        }
    else
        echo "Warning: User not in debian-tor group and 'sg' command not available."
        echo "You may need to log out and back in for Tor authentication to work."
        echo "Starting node anyway..."
        exec "$DRIPD" -drip -printtoconsole
    fi
fi
