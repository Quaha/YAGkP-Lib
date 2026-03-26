import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

csv_files = glob.glob("experiments/*.csv")
df = pd.concat([pd.read_csv(f) for f in csv_files], ignore_index=True)

graphs = df["graph"].unique()
runs   = df["run"].unique()
colors = plt.cm.tab10.colors

output_dir = "plots"
os.makedirs(output_dir, exist_ok=True)

for graph in graphs:
    graph_data = df[df["graph"] == graph]
    graph_name = os.path.splitext(graph)[0]

    n = graph_data["n"].iloc[0]
    m = graph_data["m"].iloc[0]

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    fig.suptitle(f"Graph: {graph_name}", fontsize=14)

    fig.suptitle(f"Graph: {graph_name}  |  n = {n}, m = {m}", fontsize=14)

    # --- Edge Cut vs k ---
    ax = axes[0]
    for i, run in enumerate(runs):
        run_data = graph_data[graph_data["run"] == run].sort_values("k")
        ax.plot(run_data["k"], run_data["edge_cut"],
                marker="o", label=run, color=colors[i])
    ax.set_title("Edge Cut - k")
    ax.set_xlabel("k")
    ax.set_ylabel("Edge Cut")
    ax.set_xscale("log", base=2)
    ax.legend()
    ax.grid(True)

    # --- Time vs k ---
    ax = axes[1]
    for i, run in enumerate(runs):
        run_data = graph_data[graph_data["run"] == run].sort_values("k")
        ax.plot(run_data["k"], run_data["time_ms"],
                marker="o", label=run, color=colors[i])
    ax.set_title("Time - k")
    ax.set_xlabel("k")
    ax.set_ylabel("Time (ms)")
    ax.set_xscale("log", base=2)
    ax.legend()
    ax.grid(True)

    plt.tight_layout()
    plt.savefig(f"{output_dir}/{graph_name}.png", dpi=150)
    plt.close()
    print(f"Saved: {output_dir}/{graph_name}.png")

print("Done!")