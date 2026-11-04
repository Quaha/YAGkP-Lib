#!/usr/bin/env python3
"""Разрез и время YAGkP против других решателей (прогон по плану compare).

    python3 benchmark/analysis/compare.py benchmark/results/<прогон> [--ref kahip_eco]

Для каждого решателя и k печатается среднее геометрическое по графам
отношения его разреза и времени к решателю --ref. В папку прогона
сохраняются:
    compare_cut.csv, compare_time.csv  таблицы граф x k x решатель
    compare.pdf                         те же средние на графиках
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
    parser.add_argument("--ref", default="kahip_eco", help="с каким решателем сравнивать")
    args = parser.parse_args()
    run_dir = Path(args.run_dir)

    df = load(run_dir)
    df = df[df["threads"] == df.groupby("solver")["threads"].transform("min")]

    cut = df.pivot_table(index=["graph", "k"], columns="solver", values="edge_cut")
    time = df.pivot_table(index=["graph", "k"], columns="solver", values="time_ms")
    cut.to_csv(run_dir / "compare_cut.csv")
    time.to_csv(run_dir / "compare_time.csv")

    if args.ref not in cut.columns:
        raise SystemExit(f"Решателя {args.ref} нет в прогоне: {', '.join(cut.columns)}")

    solvers = [s for s in cut.columns if s != args.ref]
    ks = sorted(df["k"].unique())
    # Время меньше 1 мс не измеряется: такие строки в отношениях времени не участвуют
    ratios = {"cut": {}, "time": {}}
    for s in solvers:
        for k in ks:
            c = cut.xs(k, level="k")[[s, args.ref]].dropna()
            t = time.xs(k, level="k")[[s, args.ref]].dropna()
            t = t[(t > 0).all(axis=1)]
            ratios["cut"][s, k] = geomean(c[s] / c[args.ref]) if len(c) else float("nan")
            ratios["time"][s, k] = geomean(t[s] / t[args.ref]) if len(t) else float("nan")

    for what, title in (("cut", "разрез"), ("time", "время")):
        print(f"\n{title} относительно {args.ref} (среднее геометрическое по графам)")
        print(f"{'решатель':18s}" + "".join(f"k={k:<7d}" for k in ks))
        for s in solvers:
            print(f"{s:18s}" + "".join(f"{ratios[what][s, k]:<9.3f}" for k in ks))

    fig, axes = plt.subplots(1, 2, figsize=(12, 4.5))
    for ax, (what, title) in zip(axes, (("cut", "разрез"), ("time", "время"))):
        for s in solvers:
            ax.plot(ks, [ratios[what][s, k] for k in ks], marker="o", label=s)
        ax.axhline(1, color="gray", linewidth=0.8)
        ax.set_xscale("log", base=2)
        ax.set_xticks(ks, [str(k) for k in ks])
        ax.set_xlabel("k")
        ax.set_ylabel(f"{title} / {args.ref}")
        if what == "time":
            ax.set_yscale("log")
        ax.grid(alpha=0.3)
    axes[0].legend()
    fig.suptitle(f"Относительно {args.ref}, среднее геометрическое по графам")
    fig.tight_layout()
    fig.savefig(run_dir / "compare.pdf")
    print(f"\nСохранено: {run_dir}/compare_cut.csv, compare_time.csv, compare.pdf")


if __name__ == "__main__":
    main()
