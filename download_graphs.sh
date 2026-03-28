#!/bin/bash

DATA_DIR="$(pwd)/data"
mkdir -p "$DATA_DIR"

# Формат: "ГРУППА/ИМЯ"
MATRICES=(
    # Таблица рекордов (Walshaw)
    "Hamm/add20"
    "DIMACS10/data"
    "AG-Monien/3elt"
    "DIMACS10/uk"
    "Hamm/add32"
    "HB/bcsstk33"
    "AG-Monien/whitaker3"
    "AG-Monien/crack"
    "DIMACS10/wing_nodal"
    "DIMACS10/fe_4elt2"
    "Cote/vibrobox"
    "HB/bcsstk29"
    "DIMACS10/fe_sphere"
    "DIMACS10/cti"
    "Hamm/memplus"
    "DIMACS10/cs4"
    "HB/bcsstk30"
    "HB/bcsstk31"
    "HB/bcsstk32"
    "DIMACS10/fe_body"
    "DIMACS10/t60k"
    "AG-Monien/brack2"
    "Mulvey/finan512"
    "DIMACS10/fe_tooth"
    "DIMACS10/fe_rotor"
    "DIMACS10/598a"
    "DIMACS10/fe_ocean"
    "DIMACS10/144"
    "DIMACS10/m14b"
    "DIMACS10/auto"

    # 4elt, fe_pwt, wing, wave - weren't found

    # Крупные графы
    "LAW/indochina-2004"
    "Freescale/circuit5M"
    "DIMACS10/NLR"
    "Pajek/patents"
    "Gleich/wikipedia-20061104"
    "VLSI/ss"
    "VLSI/nv2"
    "SNAP/roadNet-PA"
)

for matrix in "${MATRICES[@]}"; do
    group=$(dirname "$matrix")
    name=$(basename "$matrix")
    mtx_file="$DATA_DIR/${name}.mtx"

    if [ -f "$mtx_file" ]; then
        echo "[$name] Already exists, skipping."
        continue
    fi

    echo "[$name] Downloading..."
    url="https://suitesparse-collection-website.herokuapp.com/MM/${group}/${name}.tar.gz"

    if ! wget -q --show-progress -O "/tmp/${name}.tar.gz" "$url"; then
        echo "[$name] ERROR: Failed to download $url"
        rm -f "/tmp/${name}.tar.gz"
        continue
    fi

    echo "[$name] Extracting..."
    tar -xzf "/tmp/${name}.tar.gz" -C "/tmp/"
    mv "/tmp/${name}/${name}.mtx" "$DATA_DIR/"
    rm -rf "/tmp/${name}" "/tmp/${name}.tar.gz"

    echo "[$name] Done -> $mtx_file"
done

echo ""
echo "All done! Files in $DATA_DIR"