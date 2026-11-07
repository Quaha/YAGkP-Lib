#!/usr/bin/env python3
"""Ускорение YAGkP по числу потоков в сравнении с линейным.

    python3 benchmark/analysis/speedup.py benchmark/results/<прогон> [--min-time 100]

Ускорение S(t) = T(1) / T(t), где T — медиана времени по повторам;
эффективность E(t) = S(t) / t (1 — идеальное линейное ускорение).
Разрез — средний по повторам, относительно разреза на 1 потоке.

В папку прогона сохраняются:
    speedup.csv  граф x k x потоки: время, ускорение, эффективность, разрез
    speedup.pdf  первая страница — итог по всем графам (среднее
                 геометрическое) для каждого k: ускорение и линейное,
                 эффективность, разрез; дальше — по странице на граф

В итог для каждого k входят только графы, которые при этом k на 1 потоке
считаются не меньше --min-time мс: на мелких графах время слишком мало, и
ускорение тонет в накладных расходах и шуме. На страницах отдельных графов
показаны все.
"""

import argparse
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.backends.backend_pdf import PdfPages  # noqa: E402

from common import geomean, load  # noqa: E402


def speedup_table(df):
    base = df[df["threads"] == 1].set_index(["graph", "k"])[["time_ms", "edge_cut"]]
    df = df.join(base, on=["graph", "k"], rsuffix="_1")
    df["speedup"] = df["time_ms_1"] / df["time_ms"]
    df["efficiency"] = df["speedup"] / df["threads"]
    df["cut_ratio"] = df["edge_cut"] / df["edge_cut_1"]
    return df


def draw(axes, data, ks, threads, colors):
    """Три графика: ускорение (с линейным), эффективность, разрез.
    data(k) возвращает таблицу потоки -> speedup, efficiency, cut_ratio."""
    ax_s, ax_e, ax_c = axes
    ax_s.plot(threads, threads, color="gray", linestyle="--", linewidth=1, label="линейное")
    for k in ks:
        d = data(k)
        if d is None or d.empty:
            continue
        ax_s.plot(d.index, d["speedup"], marker="o", color=colors[k], label=f"k = {k}")
        ax_e.plot(d.index, d["efficiency"], marker="o", color=colors[k], label=f"k = {k}")
        ax_c.plot(d.index, d["cut_ratio"], marker="o", color=colors[k], label=f"k = {k}")

    ax_s.set_ylabel("ускорение  T(1) / T(t)")
    ax_e.set_ylabel("эффективность  ускорение / t")
    ax_e.axhline(1, color="gray", linestyle="--", linewidth=1)
    ax_e.set_ylim(0, 1.1)
    ax_c.set_ylabel("разрез / разрез на 1 потоке")
    ax_c.axhline(1, color="gray", linestyle="--", linewidth=1)
    for ax in axes:
        ax.set_xlabel("потоки t")
        ax.set_xticks(threads)
        ax.grid(alpha=0.3)
    ax_s.legend(fontsize=8)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("run_dir")
    parser.add_argument("--solver", default="yagkp")
    parser.add_argument("--min-time", type=float, default=100,
                        help="минимальное время на 1 потоке (мс), чтобы граф попал в итог")
    args = parser.parse_args()
    run_dir = Path(args.run_dir)

    df = load(run_dir)
    df = df[df["solver"] == args.solver]
    if 1 not in set(df["threads"]):
        raise SystemExit("В прогоне нет запусков на 1 потоке: ускорение не от чего считать")
    df = speedup_table(df)

    cols = ["graph", "n", "m", "k", "threads", "repeats", "time_ms", "speedup",
            "efficiency", "edge_cut", "cut_ratio"]
    df[cols].sort_values(["m", "k", "threads"]).to_csv(run_dir / "speedup.csv", index=False)

    ks = sorted(df["k"].unique())
    threads = sorted(df["threads"].unique())
    palette = plt.get_cmap("tab10")
    colors = {k: palette(i % 10) for i, k in enumerate(ks)}

    # Итог: среднее геометрическое по графам, которые считаются достаточно долго
    big = df[df["time_ms_1"] >= args.min_time]
    n_graphs = {k: big[big["k"] == k]["graph"].nunique() for k in ks}

    def summary(k):
        d = big[big["k"] == k]
        if d.empty:
            return None
        return d.groupby("threads").agg(
            speedup=("speedup", geomean),
            efficiency=("efficiency", geomean),
            cut_ratio=("cut_ratio", geomean))

    counts = ", ".join(f"k={k}: {n_graphs[k]}" for k in ks)
    print(f"Итог, среднее геометрическое по графам от {args.min_time:g} мс на 1 потоке "
          f"(графов {counts}):")
    print(f"{'потоки':8s}" + "".join(f"k={k:<20d}" for k in ks))
    for t in threads:
        row = ""
        for k in ks:
            s = summary(k)
            if s is None or t not in s.index:
                row += f"{'-':22s}"
            else:
                row += f"{s.loc[t, 'speedup']:5.2f}x ({s.loc[t, 'efficiency'] * 100:3.0f}%) " \
                       f"{s.loc[t, 'cut_ratio']:5.3f}  "
        print(f"{t:<8d}{row}")
    print("(ускорение, в скобках эффективность, затем разрез относительно 1 потока)")

    out = run_dir / "speedup.pdf"
    with PdfPages(out) as pdf:
        fig, axes = plt.subplots(1, 3, figsize=(15, 4.8))
        draw(axes, summary, ks, threads, colors)
        fig.suptitle(f"{args.solver}: среднее геометрическое по графам от {args.min_time:g} мс "
                     f"на 1 потоке (графов {counts})")
        fig.tight_layout()
        pdf.savefig(fig)
        plt.close(fig)

        for graph, g in sorted(df.groupby("graph"), key=lambda item: item[1]["m"].iloc[0]):
            def per_graph(k, g=g):
                return g[g["k"] == k].set_index("threads").sort_index()

            fig, axes = plt.subplots(1, 3, figsize=(15, 4.8))
            draw(axes, per_graph, ks, threads, colors)
            first = g.iloc[0]
            t1 = ", ".join(f"k={k}: {g[(g['k'] == k) & (g['threads'] == 1)]['time_ms'].iloc[0]:.0f}"
                           for k in ks if not g[(g["k"] == k) & (g["threads"] == 1)].empty)
            fig.suptitle(f"{graph}   (n = {first['n']:,}, m = {first['m']:,});   "
                         f"время на 1 потоке, мс: {t1}")
            fig.tight_layout()
            pdf.savefig(fig)
            plt.close(fig)

    print(f"\nСохранено: {run_dir}/speedup.csv, {out.name} ({1 + df['graph'].nunique()} стр.)")


if __name__ == "__main__":
    main()