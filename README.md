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

## Графы из таблицы рекордов
| Graph       | \|V\|  | \|E\|    | Copied |
|--------------|--------|----------|---------|
| add20        | 2395   | 7462     | ☑️      |
| data         | 2851   | 15093    | ☑️      |
| 3elt         | 4720   | 13722    | ⬜       |
| uk           | 4824   | 6837     | ☑️      |
| add32        | 4960   | 9462     | ☑️      |
| bcsstk33     | 8738   | 291583   | ⬜       |
| whitaker3    | 9800   | 28989    | ⬜       |
| crack        | 10240  | 30380    | ⬜       |
| wing_nodal   | 10937  | 75488    | ☑️      |
| fe_4elt2     | 11143  | 32818    | ☑️      |
| vibrobox     | 12328  | 165250   | ⬜       |
| bcsstk29     | 13992  | 302748   | ⬜       |
| 4elt         | 15606  | 45878    | ⬜       |
| fe_sphere    | 16386  | 49152    | ☑️      |
| cti          | 16840  | 48232    | ☑️      |
| memplus      | 17758  | 54196    | ⬜       |
| cs4          | 22499  | 43858    | ☑️      |
| bcsstk30     | 28924  | 1007284  | ⬜       |
| bcsstk31     | 35588  | 572914   | ⬜       |
| fe_pwt       | 36519  | 144794   | ⬜       |
| bcsstk32     | 44609  | 985046   | ⬜       |
| fe_body      | 45087  | 163734   | ☑️      |
| t60k         | 60005  | 89440    | ☑️      |
| wing         | 62032  | 121544   | ☑️      |
| brack2       | 62631  | 366559   | ⬜       |
| finan512     | 74752  | 261120   | ⬜       |
| fe_tooth     | 78136  | 452591   | ☑️      |
| fe_rotor     | 99617  | 662431   | ☑️      |
| 598a         | 110971 | 741934   | ☑️      |
| fe_ocean     | 143437 | 409593   | ☑️      |
| 144          | 144649 | 1074393  | ☑️      |
| wave         | 156317 | 1059331  | ⬜       |
| m14b         | 214765 | 1679018  | ☑️      |
| auto         | 448695 | 3314611  | ☑️      |

