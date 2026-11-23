#!/bin/bash

PROGRAM="./experimento"

NUMACTL="numactl -C 4"

PERF_EVENTS="{cpu_core/cycles/,cpu_core/slots/,"
PERF_EVENTS+="cpu_core/fp_arith_inst_retired.scalar_single/,"
PERF_EVENTS+="cpu_core/fp_arith_inst_retired.128b_packed_single/,"
PERF_EVENTS+="cpu_core/fp_arith_inst_retired.256b_packed_single/,"
PERF_EVENTS+="cpu_core/exe_activity.1_ports_util/,"
PERF_EVENTS+="cpu_core/exe_activity.2_ports_util/,"
PERF_EVENTS+="cpu_core/exe_activity.3_ports_util/,"
PERF_EVENTS+="cpu_core/topdown-bad-spec/,"
PERF_EVENTS+="cpu_core/topdown-be-bound/,"
PERF_EVENTS+="cpu_core/topdown-fe-bound/,"
PERF_EVENTS+="cpu_core/topdown-retiring/,"
PERF_EVENTS+="cpu_core/topdown-mem-bound/,"
PERF_EVENTS+="cpu_core/exe_activity.bound_on_loads/,"
PERF_EVENTS+="cpu_core/event=0xa2,umask=0x10/, cpu_core/event=0xa2,umask=0x01/,"
PERF_EVENTS+="cpu_core/memory_activity.stalls_l1d_miss/,"
PERF_EVENTS+="cpu_core/memory_activity.stalls_l2_miss/,"
PERF_EVENTS+="cpu_core/memory_activity.stalls_l3_miss/}:W"



perf stat \
    -e "$PERF_EVENTS" \
    $NUMACTL $PROGRAM
