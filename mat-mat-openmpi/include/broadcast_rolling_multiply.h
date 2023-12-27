#ifndef BROADCAST_ROLLING_MULTIPLY_H_
#define BROADCAST_ROLLING_MULTIPLY_H_

#define TAG_BROADCAST_MATRIX_1 100
#define TAG_BROADCAST_MATRIX_2 200

#include "mpi.h"

void broadcast_rolling_multiply(double *local_matrix_1, double *local_matrix_2,
                                double *local_result, MPI_Comm *torus,
                                MPI_Comm *rows_comm, MPI_Comm *cols_comm,
                                int sub_size, int size, int step);

#endif // BROADCAST_ROLLING_MULTIPLY_H_
