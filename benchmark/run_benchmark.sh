#!/usr/bin/env bash
# =============================================================================
# Runs a benchmark plan and collects the results into one CSV file
#
#   bash benchmark/run_benchmark.sh <plan>      # e.g. compare, scaling
#
# A plan is benchmark/plans/<plan>.sh: what to run (see the plans there).
# Every combination graph x k x solver x threads x repeat is one run of
# yagkp_bench. The results go to benchmark/results/<date>_<time>_<plan>/:
#   results.csv  one line per run (header: yagkp_bench --csv-header)
#   plan.sh      the plan as it was at the start
#   info.txt     machine, commit, OpenMP settings
#   errors.log   output of the runs that failed (only if there were any)
# The lines are written as soon as a run ends, so an interrupted benchmark
# keeps what it has measured
# =============================================================================
set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

if [ $# -ne 1 ]; then
    echo "Usage: bash benchmark/run_benchmark.sh <plan>" >&2
    echo "Plans: $(cd "$SCRIPT_DIR/plans" && ls -1 ./*.sh | sed 's|^\./||; s|\.sh$||' | tr '\n' ' ')" >&2
    exit 1
fi
PLAN_NAME="$1"
PLAN_FILE="$SCRIPT_DIR/plans/$PLAN_NAME.sh"
if [ ! -f "$PLAN_FILE" ]; then
    echo "Error: plan $PLAN_FILE not found" >&2
    exit 1
fi

# -----------------------------------------------------------------------------
# The plan. Defaults for what it does not set
# -----------------------------------------------------------------------------
SET=walshaw
GRAPHS=()        # empty: all uncommented graphs of data/<SET>_list.txt
KS=(2 4 8 16 32 64)
SOLVERS=(yagkp)
THREADS=(1)      # used for yagkp only, the other solvers are single-threaded
REPEATS=1
IMBALANCE=0.05
VARIANT=par      # build of yagkp_bench: par or seq
# shellcheck source=/dev/null
source "$PLAN_FILE"

BINARY="$ROOT_DIR/build/benchmark-$VARIANT/apps/bench/yagkp_bench"
SET_DIR="$ROOT_DIR/data/$SET"
LIST_FILE="$ROOT_DIR/data/${SET}_list.txt"

if [ ! -x "$BINARY" ]; then
    echo "Error: $BINARY not found. Build it first: bash build.sh --benchmark --$VARIANT" >&2
    exit 1
fi

if [ ${#GRAPHS[@]} -eq 0 ]; then
    if [ ! -f "$LIST_FILE" ]; then
        echo "Error: $LIST_FILE not found. Download the set first: bash data/download_graphs.sh $SET" >&2
        exit 1
    fi
    while read -r name _; do
        [[ -z "${name:-}" || "$name" == \#* ]] && continue
        GRAPHS+=("$name")
    done <"$LIST_FILE"
fi

for name in "${GRAPHS[@]}"; do
    if [ ! -f "$SET_DIR/$name.mtx" ]; then
        echo "Error: $SET_DIR/$name.mtx not found" >&2
        exit 1
    fi
done

# Threads are pinned to cores, as is usual for OpenMP measurements
export OMP_PROC_BIND="${OMP_PROC_BIND:-close}"
export OMP_PLACES="${OMP_PLACES:-cores}"

# -----------------------------------------------------------------------------
# The results directory
# -----------------------------------------------------------------------------
OUT_DIR="$SCRIPT_DIR/results/$(date +%Y-%m-%d_%H-%M-%S)_$PLAN_NAME"
mkdir -p "$OUT_DIR"
CSV="$OUT_DIR/results.csv"
ERRORS="$OUT_DIR/errors.log"
STDERR="$(mktemp)"
trap 'rm -f "$STDERR"' EXIT

cp "$PLAN_FILE" "$OUT_DIR/plan.sh"
{
    echo "date:      $(date)"
    echo "host:      $(hostname)"
    echo "cpu:       $(grep -m1 'model name' /proc/cpuinfo 2>/dev/null | cut -d: -f2- | sed 's/^ *//')"
    echo "cores:     $(nproc 2>/dev/null)"
    echo "commit:    $(git -C "$ROOT_DIR" rev-parse --short HEAD 2>/dev/null)$(git -C "$ROOT_DIR" status --porcelain --untracked-files=no 2>/dev/null | grep -q . && echo ' (uncommitted changes)')"
    echo "binary:    ${BINARY#"$ROOT_DIR"/}"
    echo "omp:       OMP_PROC_BIND=$OMP_PROC_BIND OMP_PLACES=$OMP_PLACES"
} >"$OUT_DIR/info.txt"

echo "repeat,$("$BINARY" --csv-header)" >"$CSV"

# -----------------------------------------------------------------------------
# Runs
# -----------------------------------------------------------------------------
threads_of() { # threads_of <solver>: the thread counts to run the solver with
    if [ "$1" = "yagkp" ]; then
        echo "${THREADS[@]}"
    else
        echo 1
    fi
}

total=0
for solver in "${SOLVERS[@]}"; do
    count=$(threads_of "$solver" | wc -w)
    total=$((total + ${#GRAPHS[@]} * ${#KS[@]} * count * REPEATS))
done

echo "Plan $PLAN_NAME: $total runs, results in ${OUT_DIR#"$ROOT_DIR"/}"
run=0
failed=0
for name in "${GRAPHS[@]}"; do
    for k in "${KS[@]}"; do
        for solver in "${SOLVERS[@]}"; do
            for threads in $(threads_of "$solver"); do
                for ((repeat = 0; repeat < REPEATS; repeat++)); do
                    run=$((run + 1))
                    label="[$run/$total] $name k=$k $solver threads=$threads repeat=$repeat"

                    if line="$(OMP_NUM_THREADS="$threads" "$BINARY" --graph "$SET_DIR/$name.mtx" \
                        --k "$k" --algo "$solver" --imbalance "$IMBALANCE" --csv 2>"$STDERR")"; then
                        echo "$repeat,$line" >>"$CSV"
                        echo "$label: $(echo "$line" | cut -d, -f8) ms, cut $(echo "$line" | cut -d, -f9)"
                    else
                        failed=$((failed + 1))
                        echo "$label: FAILED"
                        printf '===== %s\n%s\n%s\n' "$label" "$line" "$(cat "$STDERR")" >>"$ERRORS"
                    fi
                done
            done
        done
    done
done

echo
echo "Done: $((run - failed)) of $total runs, results in ${OUT_DIR#"$ROOT_DIR"/}/results.csv"
if [ "$failed" -gt 0 ]; then
    echo "Failed runs: $failed, see ${ERRORS#"$ROOT_DIR"/}"
    exit 1
fi
