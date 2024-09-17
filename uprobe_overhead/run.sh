#!/usr/bin/env bash

set -euo pipefail

# This script runs the uprobe_overhead benchmark.

mkdir build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build

./build/uprobe_overhead

# Run the benchmark
sudo perf probe -x ./build/uprobe_overhead foo
echo 1 | sudo tee /sys/kernel/tracing/events/probe_uprobe_overhead/foo/enable
./build/uprobe_overhead

echo 0 | sudo tee /sys/kernel/tracing/events/probe_uprobe_overhead/foo/enable
sudo perf probe -d probe_uprobe_overhead:foo

# echo "p:foo /app/experiments/uprobe_overhead/build/uprobe_overhead:0x1240" > uprobe_events
# echo 1 > events/uprobes/foo/enable

# sudo bpftrace -e 'usdt:/app/ddprof/build_debug/libdd_profiling.so:ddprof:allocation { printf("%d %d\n", arg0, arg1); }'

# perf probe -x /app/ddprof/build_release/libdd_profiling-embedded.so 'sdt_ddprof:allocation'
# perf record -e sdt_ddprof:allocation -aR sleep 1
# perf script
