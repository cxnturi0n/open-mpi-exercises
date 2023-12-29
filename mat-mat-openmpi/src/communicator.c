#include "../include/communicator.h"
#include "../include/messages.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void build_2D_torus(MPI_Comm *torus_comm, MPI_Comm *rows_comm,
                    MPI_Comm *cols_comm, int num_procs_rows,
                    int num_procs_cols) {
  int reorder = 1;

  int *dimensions = (int *)malloc(2 * sizeof(int));
  int *periods = (int *)malloc(2 * sizeof(int));

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

  free(dimensions);
  free(periods);

  return;
}

void distribute(int nprocs, int rank, double **matrix_1, double **matrix_2,
                double *local_matrix_1, double *local_matrix_2, int size,
                int sub_size) {
  int total_size = sub_size * sub_size;

  if (rank == 0) {
    double *sub_matrix_1 = (double *)malloc(total_size * sizeof(double));
    double *sub_matrix_2 = (double *)malloc(total_size * sizeof(double));

    if (!sub_matrix_1 || !sub_matrix_2) {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    int row_offset = 0;
    int col_offset = 0;

    for (int id_process = 0; id_process < nprocs; id_process++) {
      int index = 0;
      for (int i = 0; i < sub_size; i++) {
        for (int j = 0; j < sub_size; j++) {
          sub_matrix_1[index] = matrix_1[row_offset + i][col_offset + j];
          sub_matrix_2[index] = matrix_2[row_offset + i][col_offset + j];
          index++;
        }
      }

      col_offset = (col_offset + sub_size) % size;
      if (!col_offset) {
        row_offset += sub_size;
      }

      // if non root, send data
      if (id_process > 0) {
        MPI_Send(sub_matrix_1, total_size, MPI_DOUBLE, id_process,
                 TAG_DISTRIBUTE + 1, MPI_COMM_WORLD);
        MPI_Send(sub_matrix_2, total_size, MPI_DOUBLE, id_process,
                 TAG_DISTRIBUTE + 2, MPI_COMM_WORLD);
      } else {
        // copy on local matrix for process 1
        for (int i = 0; i < total_size; i++) {
          local_matrix_1[i] = sub_matrix_1[i];
          local_matrix_2[i] = sub_matrix_2[i];
        }
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
                          int rank, int nprocs, int proc_group_size,
                          MPI_Comm *torus) {

  double **result;
  int total_size = sub_size * sub_size;
  if (rank == 0) {
    result = (double **)malloc(sizeof(double *) * size);
    if (!result) {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    for (int i = 0; i < size; i++) {
      result[i] = (double *)malloc(size * sizeof(double));
      if (!result[i]) {
        fprintf(stderr, MALLOC_FAILURE);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
    }

    for (int index = 0; index < total_size; index++) {
      int row = index / sub_size;
      int col = index % sub_size;
      result[row][col] = local_result[index];
    }

    int row_offset = 0, col_offset = sub_size;
    for (int id = 1; id < nprocs; id++) {
      MPI_Recv(local_result, total_size, MPI_DOUBLE, id, TAG_RECOMPOSE + id,
               *torus, MPI_STATUS_IGNORE);
      for (int index = 0; index < total_size; index++) {
        int row = index / sub_size;
        int col = index % sub_size;
        result[row_offset + row][col_offset + col] = local_result[index];
      }

      col_offset = (col_offset + sub_size) % size;
      if (!col_offset) {
        row_offset += sub_size;
      }
    }
  } else {
    MPI_Send(local_result, total_size, MPI_DOUBLE, 0, TAG_RECOMPOSE + rank,
             *torus);
  }
  return result;
}
