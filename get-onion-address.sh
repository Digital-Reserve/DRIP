#!/bin/bash
# Quick script to get the DRIP node onion address

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DRIP_CLI="$SCRIPT_DIR/build/bin/drip-cli"

if [ ! -f "$DRIP_CLI" ]; then
    echo "Error: drip-cli not found at $DRIP_CLI"
    echo "Please build the project first: cmake --build build"
    exit 1
fi

ONION=$("$DRIP_CLI" -drip getnetworkinfo 2>/dev/null | \
  grep -oP '[a-z0-9]{56}\.onion' | head -1)

if [ -n "$ONION" ]; then
  echo "Your DRIP node onion address:"
  echo "$ONION:58333"
  echo ""
  echo "Share this with others to add your node:"
  echo "addnode=$ONION:58333"
else
  echo "Onion address not yet generated. The node may still be starting up."
  echo "Wait a few minutes and try again, or check:"
  echo "  $DRIP_CLI -drip getnetworkinfo | jq '.localaddresses'"
fi

