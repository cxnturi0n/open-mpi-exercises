#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include "mpi.h"

#define TAG_DISTRIBUTE 4

#define TAG_RECOMPOSE 42

void build_2D_torus(MPI_Comm *torus_comm, MPI_Comm *rows_comm,
                    MPI_Comm *cols_comm, int num_procs_rows,
                    int num_procs_cols);

void distribute(int nprocs, int rank, double **matrix_1, double **matrix_2,
                double *local_matrix_1, double *local_matrix_2, int size,
                int sub_size);

double **recompose_result(double *local_result, int sub_size, int size,
                          int rank, int nprocs);

#endif // COMMUNICATOR_H_
