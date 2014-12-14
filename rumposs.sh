#!/bin/sh
cmd="$1"
shift
LD_PRELOAD=./rumposs.so exec $cmd $@
