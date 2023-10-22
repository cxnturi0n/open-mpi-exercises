#include "../include/strategies.h"
#include "../include/bitwise_algo.h"

void merge_strategy_1(int nprocs, int rank, double *sum) {
  if (rank == 0) {
    // receive from slaves
    int step;
    double partial_sum = 0;
    for (step = 1; step < nprocs; step++) {
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, step, TAG_MERGE, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      *sum += partial_sum;
    }
  } else {
    // send to master
    MPI_Send(sum, 1, MPI_DOUBLE, 0, TAG_MERGE, MPI_COMM_WORLD);
  }
}

void merge_strategy_2(int nprocs, int rank, double *sum) {
  int pow = 2, offset = 1;
  int step;
  int merge_steps = fast_log_2(nprocs);
  double partial_sum = 0;
  int modulus;
  for (step = 0; step < merge_steps; step++) {
    modulus = rank % pow;
    if (!modulus) {
      // receive from slave
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, rank + offset, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      *sum += partial_sum;
    } else if (modulus == offset) {
      // send to master
      MPI_Send(sum, 1, MPI_DOUBLE, rank - offset, TAG_MERGE, MPI_COMM_WORLD);
    }
    offset = pow;
    pow <<= 1;
  }
}

void merge_strategy_3(int nprocs, int rank, double *sum) {
  int pow = 2, offset = 1;
  int step;
  double partial_sum = 0;
  int merge_steps = fast_log_2(nprocs);

  for (step = 0; step < merge_steps; step++) {
    if (rank % pow < offset) {
      // receive then send
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, rank + offset, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(sum, 1, MPI_DOUBLE, rank + offset, TAG_MERGE, MPI_COMM_WORLD);
    } else {
      // send then receive
      MPI_Send(sum, 1, MPI_DOUBLE, rank - offset, TAG_MERGE, MPI_COMM_WORLD);
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, rank - offset, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    *sum += partial_sum;
    offset = pow;
    pow <<= 1;
  }
}
