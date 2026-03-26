import matplotlib.pyplot as plt

# === Settings ===
input_file = "input.txt"
xlabel = "Номер итерации огрубления"
ylabel = "Удельный вес сжатой компоненты (%)"
title = "Heavy Clique Matching + GGGA + KL"

use_log_scale = False

def main():
    # === Data input ===
    level = []
    average = []
    average_max = []
    max_of_max = []
    average_median = []
    max_of_median = []

    with open(input_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) < 6:
                continue
            l, a, am, mm, amed, mmed = map(float, parts[:6])
            level.append(l)
            average.append(a)
            average_max.append(am)
            max_of_max.append(mm)
            average_median.append(amed)
            max_of_median.append(mmed)

    # === Plotting ===
    plt.figure(figsize=(10, 6))

    plt.plot(level, average, marker='o', label='Средний удельный вес')
    plt.plot(level, average_max, marker='s', label='Средний удельный максимум')
    plt.plot(level, max_of_max, marker='^', label='Максимальный удельный максимум')
    plt.plot(level, average_median, marker='x', label='Средняя удельная медиана')
    plt.plot(level, max_of_median, marker='d', label='Максимальная удельная медиана')

    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()

    if use_log_scale:
        plt.xscale("log")
        plt.yscale("log") 
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()

