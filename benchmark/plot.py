#!/usr/bin/env python3
"""
Построение сгруппированных столбчатых диаграмм по результатам бенчмарка
графовых разбиений. Для каждой матрицы сохраняется png из двух подграфиков:
сверху edge cut, снизу время работы (лог-шкала) по алгоритмам и k.

Формат имени входных файлов: <matrix>.mtx_k<K>.txt,
где K — число частей (обычно степень двойки: 2, 4, 8, 16, 32, 64).

Формат содержимого:
    # Graph: <name>.mtx  n=<N>  m=<M>  k=<K>  imb=<...>
    # ...
    algo  time(ms)  edge_cut  imbalance  max_part  opt_part

Пример запуска:
    python plot.py  ./results  --out ./plots
"""

import argparse
import os
import re
import sys
import warnings
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np

# tight_layout на фигуре с разными yscale у подграфиков выдаёт безобидный
# UserWarning — для нашего макета он не информативен.
warnings.filterwarnings(
    'ignore',
    message=r'.*tight_layout.*',
    category=UserWarning,
)

FILENAME_RE = re.compile(r'^(?P<matrix>.+)\.mtx_k(?P<k>\d+)\.txt$')
HEADER_RE   = re.compile(
    r'#\s*Graph:\s*(?P<name>\S+)\s+n=(?P<n>\d+)\s+m=(?P<m>\d+)'
)


def parse_file(path):
    """Считать один файл. Вернуть (meta, rows).
    meta = {'graph_name', 'n', 'm'} если удалось разобрать заголовок."""
    meta, rows = {}, []
    with open(path, 'r', encoding='utf-8', errors='replace') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if line.startswith('#'):
                if not meta:
                    mh = HEADER_RE.match(line)
                    if mh:
                        meta = {
                            'graph_name': mh.group('name'),
                            'n': int(mh.group('n')),
                            'm': int(mh.group('m')),
                        }
                continue
            parts = line.split()
            if len(parts) < 6:
                continue
            try:
                rows.append({
                    'algo':      parts[0],
                    'time_ms':   float(parts[1]),
                    'edge_cut':  float(parts[2]),
                    'imbalance': parts[3],
                    'max_part':  float(parts[4]),
                    'opt_part':  float(parts[5]),
                })
            except ValueError:
                continue
    return meta, rows


def collect(data_dir):
    """Сгруппировать файлы по матрицам: {matrix: (meta, {k: [rows, ...]})}."""
    per_matrix_rows = defaultdict(dict)
    per_matrix_meta = {}
    for name in sorted(os.listdir(data_dir)):
        m = FILENAME_RE.match(name)
        if not m:
            continue
        matrix = m.group('matrix')
        k      = int(m.group('k'))
        meta, rows = parse_file(os.path.join(data_dir, name))
        per_matrix_rows[matrix][k] = rows
        if matrix not in per_matrix_meta and meta:
            per_matrix_meta[matrix] = meta
    return {
        m: (per_matrix_meta.get(m, {}), per_matrix_rows[m])
        for m in per_matrix_rows
    }


def _fmt_int(x):
    """Разделители тысяч пробелом: 110971 -> '110 971'."""
    return f'{int(x):,}'.replace(',', ' ')


def _draw_bars(ax, ks, algos, data_by_k, colors, value_key):
    """Нарисовать сгруппированные столбики на оси ax для заданного поля."""
    n_algos = len(algos)
    group_width = 0.85
    bar_width = group_width / n_algos
    x_base = np.arange(len(ks))

    for i, algo in enumerate(algos):
        heights = []
        for k in ks:
            row = next((r for r in data_by_k[k] if r['algo'] == algo), None)
            heights.append(row[value_key] if row is not None else 0.0)
        offsets = x_base - group_width / 2 + bar_width * (i + 0.5)
        ax.bar(offsets, heights, bar_width,
               label=algo, color=colors[algo],
               edgecolor='black', linewidth=0.3)

    ax.set_xticks(x_base)
    ax.set_xticklabels([str(k) for k in ks])
    ax.grid(axis='y', linestyle='--', alpha=0.4)


def plot_matrix(matrix, meta, data_by_k, out_path):
    ks = sorted(data_by_k.keys())

    # Устойчивый порядок алгоритмов: в каком порядке впервые встретились.
    algos, seen = [], set()
    for k in ks:
        for row in data_by_k[k]:
            if row['algo'] not in seen:
                seen.add(row['algo'])
                algos.append(row['algo'])
    if not algos or not ks:
        return

    n_algos, n_k = len(algos), len(ks)
    fig_w = max(10, 1.8 * n_k + 0.35 * n_algos)
    fig, (ax_top, ax_bot) = plt.subplots(
        2, 1, figsize=(fig_w, 10), sharex=True,
        gridspec_kw={'height_ratios': [1, 1], 'hspace': 0.12},
    )

    cmap = plt.get_cmap('tab10' if n_algos <= 10 else 'tab20')
    colors = {a: cmap(i % cmap.N) for i, a in enumerate(algos)}

    # --- Верхний график: edge cut ---
    _draw_bars(ax_top, ks, algos, data_by_k, colors, 'edge_cut')
    ax_top.set_ylabel('Edge cut')
    ax_top.legend(loc='upper left', ncol=min(n_algos, 4), fontsize=9)
    ymax = ax_top.get_ylim()[1]
    ax_top.set_ylim(0, ymax * 1.18)  # запас сверху под легенду

    # --- Нижний график: время, лог-шкала ---
    _draw_bars(ax_bot, ks, algos, data_by_k, colors, 'time_ms')
    ax_bot.set_yscale('log')
    ax_bot.set_ylabel('Время, мс (лог. шкала)')
    ax_bot.set_xlabel('Число частей (k)')
    ax_bot.grid(axis='y', which='both', linestyle='--', alpha=0.3)

    # --- Общий заголовок ---
    graph_name = meta.get('graph_name', f'{matrix}.mtx')
    if 'n' in meta and 'm' in meta:
        sub = f'|V| = {_fmt_int(meta["n"])},   |E| = {_fmt_int(meta["m"])}'
        title = f'{graph_name}\n{sub}'
    else:
        title = graph_name
    fig.suptitle(title, fontsize=13, y=0.995)

    fig.tight_layout(rect=(0, 0, 1, 0.96))
    fig.savefig(out_path, dpi=150)
    plt.close(fig)


def main():
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    ap.add_argument('data_dir', help='каталог с файлами *.mtx_k<K>.txt')
    ap.add_argument('--out', default='plots',
                    help='каталог для png (по умолчанию ./plots)')
    args = ap.parse_args()

    os.makedirs(args.out, exist_ok=True)
    data = collect(args.data_dir)
    if not data:
        print(f'В каталоге {args.data_dir!r} не найдено подходящих файлов',
              file=sys.stderr)
        sys.exit(1)

    for matrix, (meta, data_by_k) in sorted(data.items()):
        out_file = os.path.join(args.out, f'{matrix}.png')
        plot_matrix(matrix, meta, data_by_k, out_file)
        ks = sorted(data_by_k.keys())
        print(f'  {matrix}: {out_file}  (k = {ks})')


if __name__ == '__main__':
    main()