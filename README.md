# [RU] YAGkP-Lib — Yet Another Graph k-Partition Library

Библиотека для решения задачи разбиения графа на k частей.

---

## Установка и сборка

### Клонирование
```bash
git clone --recursive https://github.com/Quaha/YAGkP-Lib
cd YAGkP-Lib
```

### Сборка (Linux)
```bash
bash build_linux.sh
```

Скрипт автоматически собирает все зависимости (GKlib, METIS, SCOTCH) в папку `deps/` и затем собирает основной проект. Папка `deps/` не хранится в git — при первом запуске сборка займёт несколько минут.

---

## Запуск бенчмарка

### Одиночный запуск
```bash
./build/YAGkP_app --graph data/add20.mtx --k 4 --algo kahip --output benchmark/results/
```

Доступные алгоритмы: `yagkp`, `kahip`, `metis`, `scotch`

### Полный бенчмарк
```bash
bash benchmark/run_benchmark.sh
```

Результаты сохраняются в:
- `benchmark/results/`

---

## Полезные ресурсы
- The Graph Partitioning Archive: https://chriswalshaw.co.uk/partition/ - таблица рекордов разделений
- The SuiteSparse Matrix Collection: https://sparse.tamu.edu - коллекция матриц

---

## TO-DO & Notes:
- GGA не корректный (Всё, что соответствует первой части - first)
- Избавиться от постобработки
---

