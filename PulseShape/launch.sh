#!/bin/bash

export EXE="TkPulseShape"

for i in `seq 1 3`; do
    for j in `seq 1 4`; do
       echo ./${EXE} $i $j
       ./${EXE} $i $j
    done
done
