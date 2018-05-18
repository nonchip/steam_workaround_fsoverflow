#!/bin/bash -x
d="$(dirname "$(readlink -f "$0")")"
env LD_PRELOAD=${d}/build/libswfsoverflow64.so:${d}/build/libswfsoverflow32.so DEBUGGER="${DEBUGGER} env LD_PRELOAD=${d}/build/libswfsoverflow64.so:${d}/build/libswfsoverflow32.so" steam "$@"
