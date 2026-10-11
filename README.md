# [RU] YAGkP-Lib — Yet Another Graph k-Partition Library

Библиотека для решения задачи разбиения графа на k частей.

---

## Требования

- Linux или WSL, bash 4.2+
- CMake 3.21+
- Компилятор C/C++ с поддержкой C++20 (проверено на GCC 13)
- git: сторонние библиотеки и googletest подключены подмодулями и скачиваются скриптами сборки при первой необходимости

```bash
git clone https://github.com/Quaha/YAGkP-Lib
cd YAGkP-Lib
```

---

## Сборка

Общий скрипт `build.sh` принимает режимы и варианты флагами и по очереди вызывает малые скрипты из `scripts/build/`. Справка: `bash build.sh --help`.

| Команда | Что собирается |
|---|---|
| `bash build.sh` или `-d` | **debug**: ядро, приложения и тесты; тесты запускаются после сборки |
| `bash build.sh -r` | **release**: ядро и `yagkp_partition` |
| `bash build.sh -p` | **profile**: как release, плюс `-pg` для gprof |
| `bash build.sh -b` | **benchmark**: ядро и `yagkp_bench` вместе с METIS, KaHIP, SCOTCH, все с одинаковыми флагами |
| `bash build.sh -a` | все режимы |

Варианты: `--seq` (по умолчанию) и `--par` (параллельная версия, пока заготовка); можно указать оба. Опции: `-c` (собрать с нуля), `-j N`, `--no-tests`.

Результат: `build/<режим>-<вариант>/`, например `build/release-seq/apps/partition/yagkp_partition`.

Каждый режим можно собрать и напрямую, например `bash scripts/build/release.sh --help`.

### Флаги

Все флаги компиляции заданы в одном месте, `scripts/lib/flags.sh`; в `CMakeLists.txt` их нет.

| Режим | Флаги |
|---|---|
| debug | `-O0 -g3` |
| release, benchmark | `-O3 -DNDEBUG -funroll-loops -march=native` |
| profile | как release, плюс `-g -pg` |

`-march` задаётся переменной `YAGKP_MARCH`: например, `YAGKP_MARCH=x86-64 bash build.sh -r` даёт бинарь, переносимый между узлами кластера с разными процессорами.

### Сторонние библиотеки

METIS (с GKlib), KaHIP и SCOTCH нужны только режиму benchmark. Каждая собирается своим скриптом из `scripts/deps/` с флагами режима benchmark в `deps/<вариант>/<библиотека>/`. `build.sh -b` вызывает их сам; отдельно: `bash scripts/deps/all.sh`.

Библиотека пересобирается автоматически, если изменились флаги, компилятор, версия исходников или скрипт сборки; иначе сборка пропускается.

### Проверка сборок

```bash
bash scripts/check_builds.sh            # все режимы с нуля, seq
bash scripts/check_builds.sh --seq --par
bash scripts/check_builds.sh --full     # и сторонние библиотеки с нуля
```

Собирает каждый режим во временном каталоге (рабочий `build/` не затрагивается), запускает собранные программы на тестовом графе и выводит таблицу результатов.

### Структура скриптов

```
build.sh                   общий скрипт: флаги -> scripts/build/<режим>.sh
scripts/
  build/                   debug.sh, release.sh, profile.sh, benchmark.sh
  deps/                    gklib.sh, metis.sh, scotch.sh, kahip.sh, all.sh
  lib/                     общие функции (не запускаются сами)
    common.sh              вывод, проверки, разбор аргументов
    flags.sh               флаги компиляции всех режимов
    build.sh, deps.sh      общая логика сборки YAGkP и сторонних библиотек
  check_builds.sh          проверка всех сборок с нуля
```

---

## Запуск

### Разбиение графа (только YAGkP)

```bash
./build/release-seq/apps/partition/yagkp_partition --graph data/add20.mtx --k 4
./build/release-seq/apps/partition/yagkp_partition --graph data/add20.mtx --k 4 \
    --imbalance 0.01 --output add20.part.4
```

`--output` сохраняет номер части для каждой вершины, по одному на строку (формат METIS).

### Одиночный замер

```bash
./build/benchmark-seq/apps/bench/yagkp_bench --graph data/add20.mtx --k 4 \
    --algo kahip_strong --output benchmark/results/
```

Алгоритмы: `yagkp`, `metis_kway`, `metis_recursive`, `kahip_fast`, `kahip_eco`, `kahip_strong`, `scotch`.

### Полный бенчмарк

```bash
bash build.sh -b
bash benchmark/run_benchmark.sh
```

Графы из `benchmark/graphs.txt` скачиваются в `data/`, результаты сохраняются в `benchmark/results/`.

### Профилирование

```bash
bash build.sh -p
./build/profile-seq/apps/partition/yagkp_partition --graph data/auto.mtx --k 32
gprof ./build/profile-seq/apps/partition/yagkp_partition gmon.out > profile.txt
```

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
- Добавить начальное сжатие соседних (по рёбрам) вершин с одинаковыми соседями, проверка хэшами
- Избавиться от indexed heap
---