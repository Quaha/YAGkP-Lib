# [RU] YAGkP-Lib — Yet Another Graph k-Partition Library

Библиотека для решения задачи разбиения графа на k частей.

---

## Установка и сборка

### Требования

- GCC 11+
- CMake 3.15+
- Ninja
- OpenMPI
- flex, bison, zlib

На Ubuntu/WSL все зависимости ставятся одной командой:
```bash
sudo apt install -y build-essential cmake ninja-build libopenmpi-dev flex bison zlib1g-dev
```

### Клонирование
```bash
git clone --recursive https://github.com/Quaha/YAGkP-Lib
cd YAGkP-Lib
```

### Сборка (WSL / Linux)
```bash
bash build_gcc_x64_debug_wsl.sh
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
- `benchmark/results/csv/` — машиночитаемый формат
- `benchmark/results/txt/` — читаемый формат с таблицами

---

## Полезные ресурсы
- The Graph Partitioning Archive: https://chriswalshaw.co.uk/partition/ - таблица рекордов разделений
- The SuiteSparse Matrix Collection: https://sparse.tamu.edu - коллекция матриц

---

