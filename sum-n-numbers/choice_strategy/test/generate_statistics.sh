#!/bin/bash

# Computing times for size in [1e^4, 1e^5, 1e^6, 1e^7, 1e^9], processes/cores in [1, 2, 4, 7 ,16, 32, 64] and strategies in [1, 2, 3]
# Keeping busy as much nodes/cpus as possible

WORKDIR="YOURWORKDIR" # ex. /homes/DMA/PDC/2024/CNCFBA98U/projects/sum-n-numbers/choice_strategy

# Create Sum.out if not exists. If it exists, empty it.
touch $WORKDIR/Sum.out
>$WORKDIR/Sum.out

# Define the values for SIZE, STRATEGY, and processes
SIZES=(1000 10000 100000 1000000 1000000000)
STRATEGIES=(1 2 3)
PROCESSES=(1 2 4 8 16 32 64)


# Define the mapping for processes to nodes and ppn
PROCESSES_MAP=( [1]="1:1" [2]="2:1" [4]="4:1" [8]="8:1" [16]="8:2" [32]="8:4" [64]="8:8" )

# Define the number of times you want the same qsub to be launched
REPEAT_TIMES=5

# Loop through different SIZE, STRATEGY, and process values
for SIZE in "${SIZES[@]}"; do
  for STRATEGY in "${STRATEGIES[@]}"; do
    for NUM_PROCESSES in "${PROCESSES[@]}"; do
      # Get the corresponding nodes and ppn values
      nodes_ppn=${PROCESSES_MAP[$NUM_PROCESSES]}

      # Extract nodes and ppn from the mapping
      nodes=$(cut -d: -f1 <<< "$nodes_ppn")
      ppn=$(cut -d: -f2 <<< "$nodes_ppn")

      # Call qsub with the specified values for SIZE, STRATEGY, nodes, and ppn
      for ((i = 0; i < REPEAT_TIMES; i++)); do
        qsub -l nodes="$nodes":ppn="$ppn" -v SIZE="$SIZE",STRATEGY="$STRATEGY" -V "$WORKDIR/Sum.pbs"
        # Decomment to print qsub commands generated: 
        # echo qsub -l nodes="$nodes":ppn="$ppn" -v SIZE="$SIZE",STRATEGY="$STRATEGY" -V "$WORKDIR/Sum.pbs"
      done
    done
  done
done

# qsub commands generated example if REPEAT_TIMES=1: 

# qsub -l nodes=1:ppn=1 -v SIZE=1000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=10000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=10000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=10000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=10000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=10000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=10000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=10000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=10000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=10000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=10000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=10000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=10000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=10000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=10000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=10000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=10000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=10000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=10000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=10000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=10000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=10000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=100000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=100000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=100000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=100000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=100000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=100000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=100000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=100000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=100000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=100000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=100000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=100000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=100000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=100000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=100000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=100000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=100000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=100000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=100000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=100000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=100000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000000000,STRATEGY=1 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000000000,STRATEGY=2 -V /Sum.pbs
# qsub -l nodes=1:ppn=1 -v SIZE=1000000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=2:ppn=1 -v SIZE=1000000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=4:ppn=1 -v SIZE=1000000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=1 -v SIZE=1000000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=2 -v SIZE=1000000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=4 -v SIZE=1000000000,STRATEGY=3 -V /Sum.pbs
# qsub -l nodes=8:ppn=8 -v SIZE=1000000000,STRATEGY=3 -V /Sum.pbs
