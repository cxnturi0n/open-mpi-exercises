#ifndef BROADCAST_ROLLING_MULTIPLY_H_
#define BROADCAST_ROLLING_MULTIPLY_H_

#include "mpi.h"

#define TAG_SHARE_MATRIX_2 42

void broadcast_rolling_multiply(double *sub_matrix_1, double *sub_matrix_2,
                                double *sub_result, MPI_Comm *torus,
                                MPI_Comm *rows_comm, MPI_Comm *cols_comm,
                                int size, int proc_in_group);

#endif // BROADCAST_ROLLING_MULTIPLY_H_
