#!/bin/bash
set -e

# =============================================================================
# Paths
# SCRIPT_DIR is always the directory where this script lives (benchmark/),
# ROOT_DIR is the project root regardless of where the script is called from
# =============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# Собирается командой: bash build.sh --benchmark
BINARY="$ROOT_DIR/build/benchmark-seq/apps/bench/yagkp_bench"
SET="${1:-walshaw}"
SET_DIR="$ROOT_DIR/data/$SET"
GRAPHS_FILE="$ROOT_DIR/data/${SET}_list.txt"
OUTPUT_DIR="$SCRIPT_DIR/results"

#ALGOS=("yagkp")
ALGOS=("yagkp" "kahip_strong" "kahip_eco" "kahip_fast" "metis_kway" "metis_recursive" "scotch")
KS=(2 4 8 16 32 64)

if [ ! -x "$BINARY" ]; then
    echo "Error: $BINARY not found. Build it first: bash build.sh --benchmark" >&2
    exit 1
fi

if [ ! -f "$GRAPHS_FILE" ]; then
    echo "Error: $GRAPHS_FILE not found. Download the set first: bash data/download_graphs.sh $SET" >&2
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

# =============================================================================
# Graphs of the set: data/<set>_list.txt, one name per line;
# commented (#) and empty lines are skipped
# =============================================================================
graphs=()
while read -r name _; do
    [[ -z "${name:-}" || "$name" == \#* ]] && continue
    graphs+=("$SET_DIR/$name.mtx")
done < "$GRAPHS_FILE"

# =============================================================================
# Benchmark
# For each graph and each k, run all algorithms sequentially
# Results are written to OUTPUT_DIR by the binary itself
# =============================================================================
total=${#graphs[@]}
current=0

for graph in "${graphs[@]}"; do
    if [ ! -f "$graph" ]; then
        echo "Warning: $graph not found, skipped" >&2
        continue
    fi
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