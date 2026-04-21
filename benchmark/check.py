#!/usr/bin/env python3
"""
Для каждого графа построить график edge cut в зависимости от k,
сравнивая старую и новую версию алгоритма yagkp.

Использование:
    python plot_per_graph.py old_results.txt new_results.txt output.pdf [--png]
"""

import sys
import re
import matplotlib.pyplot as plt
import matplotlib.backends.backend_pdf


def parse_file(file_path):
    """
    Возвращает словарь:
        { (graph_name, k): edge_cut }
    где graph_name - базовое имя графа (без _k и расширения),
          k - число частей (int),
          edge_cut - значение edge cut (int)
    """
    data = {}
    current_graph_line = None

    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if line.startswith('# Graph:'):
                current_graph_line = line
            elif line.startswith('yagkp') and current_graph_line is not None:
                parts = line.split()
                if len(parts) < 3:
                    continue
                try:
                    edge_cut = int(parts[2])
                except ValueError:
                    continue

                # Извлекаем имя графа и k из строки '# Graph:'
                m = re.search(r'# Graph:\s*(\S+).*?\bk=(\d+)', current_graph_line)
                if m:
                    graph_file = m.group(1)   # например "3elt.mtx" или "bcsstk29.mtx"
                    k = int(m.group(2))
                    # Убираем расширение .mtx для чистоты подписи
                    graph_name = re.sub(r'\.(mtx|graph|txt)$', '', graph_file)
                    data[(graph_name, k)] = edge_cut
                else:
                    # Fallback: попробуем извлечь из имени файла, если оно где-то есть
                    # В нашем случае current_graph_line уже содержит нужное
                    pass
                current_graph_line = None
    return data


def main():
    if len(sys.argv) < 4:
        print("Укажите: old_file new_file output.pdf [--png]")
        sys.exit(1)

    old_file = sys.argv[1]
    new_file = sys.argv[2]
    output_pdf = sys.argv[3]
    save_png = '--png' in sys.argv

    print("Чтение старого файла...")
    old_data = parse_file(old_file)
    print(f"Старых записей: {len(old_data)}")

    print("Чтение нового файла...")
    new_data = parse_file(new_file)
    print(f"Новых записей: {len(new_data)}")

    # Собираем все уникальные имена графов
    all_graphs = set()
    for (g, k) in old_data.keys():
        all_graphs.add(g)
    for (g, k) in new_data.keys():
        all_graphs.add(g)

    all_graphs = sorted(all_graphs)
    print(f"Уникальных графов: {len(all_graphs)}")

    # Для каждого графа строим график
    pdf_pages = matplotlib.backends.backend_pdf.PdfPages(output_pdf)

    for graph in all_graphs:
        # Извлекаем данные для этого графа
        old_points = [(k, ec) for (g, k), ec in old_data.items() if g == graph]
        new_points = [(k, ec) for (g, k), ec in new_data.items() if g == graph]

        if not old_points and not new_points:
            continue

        # Сортируем по k
        old_points.sort(key=lambda x: x[0])
        new_points.sort(key=lambda x: x[0])

        # Создаём фигуру
        plt.figure(figsize=(8, 6))

        if old_points:
            k_old, ec_old = zip(*old_points)
            plt.plot(k_old, ec_old, 'o-', color='skyblue', label='Old', markersize=6, linewidth=2)

        if new_points:
            k_new, ec_new = zip(*new_points)
            plt.plot(k_new, ec_new, 's-', color='lightcoral', label='New', markersize=6, linewidth=2)

        plt.xlabel('Число частей (k)')
        plt.ylabel('Edge Cut')
        plt.title(f'{graph}')
        plt.grid(True, linestyle='--', alpha=0.6)
        plt.legend()

        # Настройка оси X: целые значения k
        all_k = set()
        if old_points:
            all_k.update([p[0] for p in old_points])
        if new_points:
            all_k.update([p[0] for p in new_points])
        if all_k:
            plt.xticks(sorted(all_k))

        plt.tight_layout()

        # Сохраняем в PDF
        pdf_pages.savefig()
        if save_png:
            png_name = f"{graph}_edgecut.png"
            plt.savefig(png_name, dpi=150)
            print(f"Сохранён PNG: {png_name}")

        plt.close()

    pdf_pages.close()
    print(f"Готово. Многостраничный PDF сохранён в '{output_pdf}'.")


if __name__ == "__main__":
    main()