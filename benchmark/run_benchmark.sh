#!/bin/bash

BINARY="$(pwd)/build/YAGkP_app"
DATA_DIR="$(pwd)/data"
OUTPUT_DIR="$(pwd)/benchmark/results"
ALGOS=("yagkp" "kahip" "metis" "scotch")
KS=(2 4 8 16 32 64)

mkdir -p "$OUTPUT_DIR"

graphs=("$DATA_DIR"/*.mtx)
total=${#graphs[@]}
current=0

for graph in "${graphs[@]}"; do
    graph_name=$(basename "$graph")
    current=$((current + 1))
    echo "[$current/$total] Graph: $graph_name"

    for k in "${KS[@]}"; do
        echo "  k = $k"

        stem="${graph_name}_k${k}"
        rm -f "$OUTPUT_DIR/csv/${stem}.csv"
        rm -f "$OUTPUT_DIR/txt/${stem}.txt"

        # Запускаем все алгоритмы параллельно для одного (граф, k)
        for algo in "${ALGOS[@]}"; do
            "$BINARY" --graph "$graph" --k "$k" --algo "$algo" --output "$OUTPUT_DIR" &
        done

        # Ждём завершения всех алгоритмов перед следующим k
        wait
    done
done

echo ""
echo "Done! Results saved to $OUTPUT_DIR"