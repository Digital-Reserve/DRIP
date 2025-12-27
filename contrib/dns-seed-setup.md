# Setting Up DNS Seeds for DRIP

## Option 1: GitHub Pages + Free DNS (Recommended)

### Step 1: Create a GitHub Pages Repository

1. Create a new repository (e.g., `drip-seeds`)
2. Enable GitHub Pages in repository settings
3. Create a simple JSON file with node addresses:

**`seeds.json`:**
```json
{
  "nodes": [
    "xc4dnfjzk7kyyda7jms5xngaldzvysi3lxkfpknhc3rb3ugyj7v2ahid.onion:58333"
  ],
  "updated": "2025-12-27"
}
```

### Step 2: Use Free DNS Service

**Option A: Freenom (Free .tk, .ml, .ga domains)**
1. Register a free domain (e.g., `drip-seeds.tk`)
2. Point it to GitHub Pages
3. Set up TXT records with node addresses

**Option B: Use a Subdomain Service**
- Services like `noip.com` offer free subdomains
- Some support TXT records

### Step 3: Add DNS Seed to DRIP

Edit `src/kernel/chainparams.cpp`:
```cpp
vSeeds.emplace_back("seed.drip-seeds.tk."); // Your DNS seed
```

## Option 2: Simple HTTP Seed Service (Easier, but requires code changes)

### Step 1: Host on GitHub Pages

Create `index.html` or use GitHub Actions to generate a simple API:

**`.github/workflows/update-seeds.yml`:**
```yaml
name: Update Seeds
on:
  schedule:
    - cron: '0 * * * *' # Every hour
  workflow_dispatch:

jobs:
  update:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Generate seeds file
        run: |
          echo '{"nodes":["xc4dnfjzk7kyyda7jms5xngaldzvysi3lxkfpknhc3rb3ugyj7v2ahid.onion:58333"]}' > seeds.json
      - name: Deploy
        uses: peaceiris/actions-gh-pages@v3
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./
```

### Step 2: Modify DRIP to Query HTTP

This would require modifying the peer discovery code to:
1. Query `https://yourusername.github.io/drip-seeds/seeds.json`
2. Parse the JSON
3. Add nodes from the list

## Option 3: Use Existing Free Services

**Cloudflare Workers (Free tier):**
- Can serve DNS-like responses
- Free for reasonable traffic

**Simple Solution for Now:**
Since DNS seeds require actual DNS infrastructure, the easiest immediate solution is:
1. Keep using manual `addnode` entries in config
2. Host a simple list on GitHub Pages that users can copy
3. Add it to documentation

## Quick GitHub Pages Setup

1. Create repo: `drip-network-seeds`
2. Add file `nodes.txt`:
```
xc4dnfjzk7kyyda7jms5xngaldzvysi3lxkfpknhc3rb3ugyj7v2ahid.onion:58333
```

3. Enable GitHub Pages
4. Users can visit: `https://yourusername.github.io/drip-network-seeds/nodes.txt`
5. Copy addresses to their config

This doesn't provide automatic discovery, but it's a free, easy way to share node addresses.

