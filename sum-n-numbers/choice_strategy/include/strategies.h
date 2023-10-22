#ifndef STRATEGIES_H_
#define STRATEGIES_H_

#include "bitwise_algo.h"
#include "constants.h"
#include "mpi.h"

void merge_strategy_1(int nprocs, int rank, double *sum);
void merge_strategy_2(int nprocs, int rank, double *sum);
void merge_strategy_3(int nprocs, int rank, double *sum);

#endif // STRATEGIES_H_
