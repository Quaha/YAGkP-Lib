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

## Влияние на точность

### Coarsening

- ...

### Bipartition

- Делить граф на две части пропорционально C1/C2 лучше, чем выбрать одну вершину для первой части (с целью дальнейшего исправления в KL), но иногда второй вариант может дать лучший результат
- GGGA лучше GGA, но второй вариант иногда может выдать лучший результат

### Uncoarsening

- В KL при возможности переноса вершины как из 1-ой части во 2-ю, так и из 2-ой в 1-ю выгоднее переносить не с max gain, а из той части, где Ci - weight_i меньше

---

## TO-DO & Notes:
- ...

---

