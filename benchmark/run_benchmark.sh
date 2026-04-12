#!/bin/bash
set -e

# =============================================================================
# Paths
# SCRIPT_DIR is always the directory where this script lives (benchmark/),
# ROOT_DIR is the project root regardless of where the script is called from
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

BINARY="$ROOT_DIR/build/YAGkP_app"
DATA_DIR="$ROOT_DIR/data"
CACHE_DIR="$ROOT_DIR/data/cache"
OUTPUT_DIR="$SCRIPT_DIR/results"
GRAPHS_FILE="$ROOT_DIR/benchmark/graphs.txt"

ALGOS=("yagkp" "kahip_strong" "kahip_eco" "kahip_fast" "metis" "scotch")
KS=(2 4 8 16 32 64)

mkdir -p "$DATA_DIR" "$CACHE_DIR" "$OUTPUT_DIR"

# =============================================================================
# Read the list of required graphs from graphs.txt
# Lines starting with # and empty lines are ignored
# Builds an associative array: name -> GROUP/NAME
# =============================================================================
declare -A NEEDED
while IFS= read -r line; do
    [[ "$line" =~ ^#.*$ || -z "$line" ]] && continue
    name=$(basename "$line")
    NEEDED["$name"]="$line"
done < "$GRAPHS_FILE"

# =============================================================================
# Move graphs that are no longer in graphs.txt from data/ to cache
# =============================================================================
for mtx_file in "$DATA_DIR"/*.mtx; do
    [ -f "$mtx_file" ] || continue
    name=$(basename "$mtx_file" .mtx)
    if [ -z "${NEEDED[$name]+x}" ]; then
        echo "[cache] $name is not in the list → moving to cache"
        mv "$mtx_file" "$CACHE_DIR/"
    fi
done

# =============================================================================
# Ensure all required graphs are present in data/
# Order of lookup: data/ → cache/ → download
# =============================================================================
for name in "${!NEEDED[@]}"; do
    matrix="${NEEDED[$name]}"
    mtx_file="$DATA_DIR/${name}.mtx"

    # Already in data/
    if [ -f "$mtx_file" ]; then
        continue
    fi

    # Found in cache — restore to data/
    if [ -f "$CACHE_DIR/${name}.mtx" ]; then
        echo "[$name] Found in cache → moving to data/"
        mv "$CACHE_DIR/${name}.mtx" "$DATA_DIR/"
        continue
    fi

    # Not found anywhere — download from SuiteSparse Matrix Collection
    group=$(dirname "$matrix")
    echo "[$name] Downloading..."
    url="https://sparse.tamu.edu/MM/${group}/${name}.tar.gz"

    if ! wget -q --show-progress -O "/tmp/${name}.tar.gz" "$url"; then
        echo "[$name] ERROR: failed to download"
        rm -f "/tmp/${name}.tar.gz"
        continue
    fi

    tar -xzf "/tmp/${name}.tar.gz" -C "/tmp/"
    mv "/tmp/${name}/${name}.mtx" "$DATA_DIR/"
    rm -rf "/tmp/${name}" "/tmp/${name}.tar.gz"
    echo "[$name] Done"
done

# =============================================================================
# Benchmark
# For each graph and each k, run all algorithms sequentially
# Results are written to OUTPUT_DIR by the binary itself
# =============================================================================
graphs=("$DATA_DIR"/*.mtx)
total=${#graphs[@]}
current=0

for graph in "${graphs[@]}"; do
    [ -f "$graph" ] || continue
    graph_name=$(basename "$graph")
    current=$((current + 1))
    echo "[$current/$total] Graph: $graph_name"

    for k in "${KS[@]}"; do
        echo "  k = $k"
        rm -f "$OUTPUT_DIR/${graph_name}_k${k}.txt"  # clear previous results for this (graph, k)

        for algo in "${ALGOS[@]}"; do
            echo "    algo = $algo"
            "$BINARY" --graph "$graph" --k "$k" --algo "$algo" --output "$OUTPUT_DIR"
        done
    done
done

echo ""
echo "Done! Results saved to $OUTPUT_DIR"