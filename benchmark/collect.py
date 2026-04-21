#!/usr/bin/env python3
"""
Собрать из набора файлов строки '# Graph:' и 'yagkp', отсортировать по имени графа и k,
записать в один файл подряд без лишних строк.

Использование:
    python collect_yagkp.py output.txt file1.txt file2.txt ...
    python collect_yagkp.py output.txt "results/*_k*.txt"
"""

import sys
import glob
import re
from pathlib import Path


def extract_lines(file_path):
    """Извлекает из файла строки с '# Graph:' и 'yagkp'."""
    graph_line = None
    yagkp_line = None

    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.rstrip('\n')
                if graph_line is None and line.startswith('# Graph:'):
                    graph_line = line
                if yagkp_line is None and line.startswith('yagkp'):
                    yagkp_line = line
                if graph_line is not None and yagkp_line is not None:
                    break
    except Exception as e:
        print(f"Ошибка при чтении {file_path}: {e}", file=sys.stderr)

    return graph_line, yagkp_line


def parse_key_from_filename(file_path):
    """
    Извлекает из имени файла имя графа и k.
    Ожидается формат: <graph_name>_k<number>.txt, например:
        bcsstk29.mtx_k16.txt  -> graph = 'bcsstk29.mtx', k = 16
    """
    name = Path(file_path).name
    # Ищем последнее вхождение '_k' с последующими цифрами перед '.txt'
    match = re.search(r'^(.*)_k(\d+)\.txt$', name)
    if match:
        graph_part = match.group(1)   # всё до '_k'
        k_part = int(match.group(2))  # число после 'k'
        return graph_part, k_part
    else:
        # Если формат не совпадает, используем имя файла целиком и k=0
        return name, 0


def main():
    if len(sys.argv) < 3:
        print("Укажите выходной файл и хотя бы один входной файл или шаблон.")
        print("Пример: python collect_yagkp.py result.txt data/*_k*.txt")
        sys.exit(1)

    output_file = sys.argv[1]
    input_patterns = sys.argv[2:]

    input_files = []
    for pattern in input_patterns:
        matched = glob.glob(pattern)
        if not matched:
            print(f"Предупреждение: шаблон '{pattern}' не дал файлов.", file=sys.stderr)
        input_files.extend(matched)

    if not input_files:
        print("Нет входных файлов для обработки.", file=sys.stderr)
        sys.exit(1)

    records = []  # список кортежей (sort_key, graph_line, yagkp_line)

    for fpath in input_files:
        graph_line, yagkp_line = extract_lines(fpath)
        if graph_line is None or yagkp_line is None:
            print(f"В файле {fpath} не найдены нужные строки.", file=sys.stderr)
            continue

        graph_name, k_val = parse_key_from_filename(fpath)
        # Сортируем сначала по имени графа, потом по k
        sort_key = (graph_name, k_val)
        records.append((sort_key, graph_line, yagkp_line))

    if not records:
        print("Нет данных для записи.", file=sys.stderr)
        sys.exit(1)

    # Сортировка
    records.sort(key=lambda x: x[0])

    # Запись в выходной файл без лишних строк
    with open(output_file, 'w', encoding='utf-8') as out:
        for _, graph_line, yagkp_line in records:
            out.write(f"{graph_line}\n")
            out.write(f"{yagkp_line}\n")

    print(f"Готово. Обработано {len(records)} файлов. Результат в '{output_file}'.")


if __name__ == "__main__":
    main()