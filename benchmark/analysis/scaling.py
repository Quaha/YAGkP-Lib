#!/usr/bin/env python3
"""Ускорение YAGkP по числу потоков и изменение разреза (прогон по плану scaling).

    python3 benchmark/analysis/scaling.py benchmark/results/<прогон>

Ускорение — время на 1 потоке / время на t потоках (медианы по повторам),
разрез — средний по повторам, относительно 1 потока. В папку прогона
сохраняются:
    scaling.csv  граф x k x потоки: время, ускорение, разрез
    scaling.pdf  ускорение и разрез по потокам, по графику на каждое k
"""

import argparse
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402

from common import geomean, load  # noqa: E402


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("run_dir")
    parser.add_argument("--solver", default="yagkp")
    args = parser.parse_args()
    run_dir = Path(args.run_dir)

    df = load(run_dir)
    df = df[df["solver"] == args.solver].copy()
    if 1 not in set(df["threads"]):
        raise SystemExit("В прогоне нет запусков на 1 потоке: ускорение не от чего считать")

    base = df[df["threads"] == 1].set_index(["graph", "k"])[["time_ms", "edge_cut"]]
    df = df.join(base, on=["graph", "k"], rsuffix="_1")
    df["speedup"] = df["time_ms_1"] / df["time_ms"]
    df["cut_ratio"] = df["edge_cut"] / df["edge_cut_1"]
    out = df[["graph", "k", "threads", "repeats", "time_ms", "speedup", "edge_cut", "cut_ratio"]]
    out.sort_values(["graph", "k", "threads"]).to_csv(run_dir / "scaling.csv", index=False)

    ks = sorted(df["k"].unique())
    threads = sorted(df["threads"].unique())
    print("Ускорение (среднее геометрическое по графам) / разрез относительно 1 потока")
    print(f"{'потоки':8s}" + "".join(f"k={k:<16d}" for k in ks))
    for t in threads:
        row = ""
        for k in ks:
            sel = df[(df["k"] == k) & (df["threads"] == t)]
            row += f"{geomean(sel['speedup']):5.2f}x / {geomean(sel['cut_ratio']):5.3f}   "
        print(f"{t:<8d}{row}")

    fig, axes = plt.subplots(2, len(ks), figsize=(4.5 * len(ks), 8), squeeze=False)
    for j, k in enumerate(ks):
        for graph, g in df[df["k"] == k].groupby("graph"):
            g = g.sort_values("threads")
            axes[0][j].plot(g["threads"], g["speedup"], marker="o", label=graph)
            axes[1][j].plot(g["threads"], g["cut_ratio"], marker="o", label=graph)
        axes[0][j].plot(threads, threads, color="gray", linestyle="--", linewidth=0.8)
        axes[0][j].set_title(f"k = {k}")
        axes[0][j].set_ylabel("ускорение")
        axes[1][j].axhline(1, color="gray", linewidth=0.8)
        axes[1][j].set_ylabel("разрез / разрез на 1 потоке")
        for ax in (axes[0][j], axes[1][j]):
            ax.set_xlabel("потоки")
            ax.set_xticks(threads)
            ax.grid(alpha=0.3)
    axes[0][0].legend(fontsize=8)
    fig.tight_layout()
    fig.savefig(run_dir / "scaling.pdf")
    print(f"\nСохранено: {run_dir}/scaling.csv, scaling.pdf")


if __name__ == "__main__":
    main()
