#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

BINARY="$ROOT_DIR/build/YAGkP_app"
DATA_DIR="$ROOT_DIR/data"
CACHE_DIR="$ROOT_DIR/data/cache"
OUTPUT_DIR="$SCRIPT_DIR/results"
GRAPHS_FILE="$ROOT_DIR/benchmark/graphs.txt"

ALGOS=("yagkp" "kahip" "metis" "scotch")
KS=(2 4 8 16 32 64)

mkdir -p "$DATA_DIR" "$CACHE_DIR" "$OUTPUT_DIR"

# === Читаем список нужных графов из graphs.txt ===
declare -A NEEDED  # имя -> полный путь (GROUP/NAME)
while IFS= read -r line; do
    # Пропускаем комментарии и пустые строки
    [[ "$line" =~ ^#.*$ || -z "$line" ]] && continue
    name=$(basename "$line")
    NEEDED["$name"]="$line"
done < "$GRAPHS_FILE"

# === Перемещаем лишние графы из data/ в кэш ===
for mtx_file in "$DATA_DIR"/*.mtx; do
    [ -f "$mtx_file" ] || continue
    name=$(basename "$mtx_file" .mtx)
    if [ -z "${NEEDED[$name]+x}" ]; then
        echo "[cache] $name не в списке → перемещаем в кэш"
        mv "$mtx_file" "$CACHE_DIR/"
    fi
done

# === Обеспечиваем наличие нужных графов ===
for name in "${!NEEDED[@]}"; do
    matrix="${NEEDED[$name]}"
    mtx_file="$DATA_DIR/${name}.mtx"

    if [ -f "$mtx_file" ]; then
        continue  # уже есть
    fi

    # Ищем в кэше
    if [ -f "$CACHE_DIR/${name}.mtx" ]; then
        echo "[$name] Найден в кэше → перемещаем в data/"
        mv "$CACHE_DIR/${name}.mtx" "$DATA_DIR/"
        continue
    fi

    # Скачиваем
    group=$(dirname "$matrix")
    echo "[$name] Скачиваем..."
    url="https://suitesparse-collection-website.herokuapp.com/MM/${group}/${name}.tar.gz"

    if ! wget -q --show-progress -O "/tmp/${name}.tar.gz" "$url"; then
        echo "[$name] ОШИБКА: не удалось скачать $url"
        rm -f "/tmp/${name}.tar.gz"
        continue
    fi

    tar -xzf "/tmp/${name}.tar.gz" -C "/tmp/"
    mv "/tmp/${name}/${name}.mtx" "$DATA_DIR/"
    rm -rf "/tmp/${name}" "/tmp/${name}.tar.gz"
    echo "[$name] Готово"
done

# === Бенчмарк ===
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
        rm -f "$OUTPUT_DIR/txt/${graph_name}_k${k}.txt"   # чистим перед запуском

        for algo in "${ALGOS[@]}"; do
            echo "    algo = $algo"
            "$BINARY" --graph "$graph" --k "$k" --algo "$algo" --output "$OUTPUT_DIR"
        done
    done
done

echo ""
echo "Done! Results saved to $OUTPUT_DIR"