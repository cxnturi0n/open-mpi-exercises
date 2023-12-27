#include "../include/communicator.h"
#include "../include/messages.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void build_2D_torus(MPI_Comm *torus_comm, MPI_Comm *rows_comm,
                    MPI_Comm *cols_comm, int num_procs_rows,
                    int num_procs_cols) {
  // don't reorder processors
  int reorder = 0;

  // TODO: free them
  int *dimensions = (int *)malloc(2 * sizeof(int));
  int *periods = (int *)malloc(2 * sizeof(int));

  // TODO: error message malloc failed
  if (!dimensions || !periods) {
    fprintf(stderr, MALLOC_FAILURE);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  dimensions[0] = num_procs_rows;
  dimensions[1] = num_procs_cols;

  periods[0] = periods[1] = 1;

  // create the torus
  MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, periods, reorder, torus_comm);

  int projections[2];
  // rows
  projections[0] = 0;
  projections[1] = 1;

  MPI_Cart_sub(*torus_comm, projections, rows_comm);

  // rows
  projections[0] = 1;
  projections[1] = 0;
  MPI_Cart_sub(*torus_comm, projections, cols_comm);

  return;
}

void distribute(int nprocs, int rank, double **matrix_1, double **matrix_2,
                double *local_matrix_1, double *local_matrix_2, int size,
                int sub_size) {
  int total_size = sub_size * sub_size;
  local_matrix_1 = (double *)malloc(total_size * sizeof(double));
  local_matrix_2 = (double *)malloc(total_size * sizeof(double));

  if (!local_matrix_1 || !local_matrix_2) {
    fprintf(stderr, MALLOC_FAILURE);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  if (rank == 0) {
    double *sub_matrix_1 = (double *)malloc(total_size * sizeof(double));
    double *sub_matrix_2 = (double *)malloc(total_size * sizeof(double));

    if (!sub_matrix_1 || !sub_matrix_2) {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int row_offset = 0;
    int col_offset = 0;
    for (int i = 0; i < nprocs; i++) {
      int index = 0;
      for (int j = 0; j < sub_size; j++) {
        printf("id := %d receive:\n", i);
        for (int k = 0; k < sub_size; k++) {
          printf("m1[%d, %d] := %lf \t m2[%d, %d] := %lf\n", row_offset + j,
                 col_offset + k, matrix_1[row_offset + j][col_offset + k],
                 row_offset + j, col_offset + k,
                 matrix_2[row_offset + j][col_offset + k]);

          sub_matrix_1[index] = matrix_1[row_offset + j][col_offset + k];
          sub_matrix_2[index] = matrix_2[row_offset + j][col_offset + k];
          index++;
        }
        printf("\n");
      }
      col_offset = (col_offset + sub_size) % size;
      if (i % sub_size == 0) {
        /* row_offset += sub_size; */
        row_offset = 0;
      }
      // if non root, send data
      if (i > 0) {
        MPI_Send(sub_matrix_1, total_size, MPI_DOUBLE, i, TAG_DISTRIBUTE + 1,
                 MPI_COMM_WORLD);
        MPI_Send(sub_matrix_2, total_size, MPI_DOUBLE, i, TAG_DISTRIBUTE + 2,
                 MPI_COMM_WORLD);
      } else {
        memcpy(local_matrix_1, sub_matrix_1, total_size);
        memcpy(local_matrix_2, sub_matrix_2, total_size);
      }
    }

    free(sub_matrix_1);
    free(sub_matrix_2);

  } else {
    MPI_Recv(local_matrix_1, total_size, MPI_DOUBLE, 0, TAG_DISTRIBUTE + 1,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(local_matrix_2, total_size, MPI_DOUBLE, 0, TAG_DISTRIBUTE + 2,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  return;
}

double **recompose_result(double *local_result, int sub_size, int size,
                          int rank, int nprocs) {
  double **result = (double **)malloc(sizeof(double *) * size);
  if (!result) {
    fprintf(stderr, MALLOC_FAILURE);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  for (int i = 0; i < size; i++) {
    result[i] = (double *)malloc(sizeof(double));
    if (!result[i]) {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
  }

  if (rank == 0) {
    // copy local result
    for (int i = 0; i < sub_size; i++) {
      for (int j = 0; j < sub_size; j++) {
        result[i][j] = local_result[i * sub_size + j];
      }
    }

    double *buffer;
    int total_sub_size = sub_size * sub_size;
    int row_offset = 0;
    int col_offset = 0;
    for (int i = 1; i < nprocs; i++) {
      MPI_Recv(buffer, total_sub_size, MPI_DOUBLE, i, TAG_RECOMPOSE + i,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (int j = 0; j < total_sub_size; j++) {
        int row = j / sub_size;
        int col = j % sub_size;
        result[row_offset + row][col_offset + col] = buffer[j];
      }
      col_offset = (col_offset + sub_size) % size;
      if (i % sub_size == 0) {
        row_offset += sub_size;
      }
    }

  } else {
    MPI_Send(local_result, sub_size * sub_size, MPI_DOUBLE, 0,
             TAG_RECOMPOSE + rank, MPI_COMM_WORLD);
  }

  return result;
}
