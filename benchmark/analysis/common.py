"""Общее для скриптов обработки: чтение results.csv из папки прогона."""

import sys
from pathlib import Path

import numpy as np
import pandas as pd


def load(run_dir):
    """results.csv прогона; повторы одного запуска сводятся в одну строку:
    время — медиана, разрез и дисбаланс — среднее (параллельная версия
    недетерминирована, так что разрез от повтора к повтору меняется)."""
    path = Path(run_dir) / "results.csv"
    if not path.exists():
        sys.exit(f"Не найден {path}")
    df = pd.read_csv(path)
    keys = ["graph", "n", "m", "k", "solver", "threads"]
    return (
        df.groupby(keys, as_index=False)
        .agg(time_ms=("time_ms", "median"),
             edge_cut=("edge_cut", "mean"),
             imbalance=("imbalance", "mean"),
             repeats=("repeat", "count"))
    )


def geomean(values):
    values = np.asarray(values, dtype=float)
    return float(np.exp(np.log(values).mean()))
