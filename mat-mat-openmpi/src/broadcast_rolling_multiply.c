#include "../include/broadcast_rolling_multiply.h"
#include "../include/messages.h"
#include "../include/utils.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void setup_emitters(int **emitters_row, int **emitters_col, int size,
                    int step) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      emitters_row[i][j] = (i + step) % size;
      emitters_col[i][j] = (j + step) % size;
    }
  }
}

void broadcast_rolling_multiply(double *local_matrix_1, double *local_matrix_2,
                                double *local_result, MPI_Comm *torus,
                                MPI_Comm *rows_comm, MPI_Comm *cols_comm,
                                int sub_size, int proc_per_row_col, int step,
                                int **emitter_row, int **emitter_col) {
  int sub_matrix_size = sub_size * sub_size;

  // get process coordinates
  int coordinate[] = {0, 0};
  int dims[] = {sub_size, sub_size};
  int periods[] = {1, 1};
  MPI_Cart_get(*torus, 2, dims, periods, coordinate);

  int row_root = emitter_row[coordinate[0]][coordinate[1]];
  int col_root = emitter_col[coordinate[0]][coordinate[1]];

  // used to receive portion of matrix 2 from processor next processor
  double *receive_buffer_2 = (double *)malloc(sizeof(double) * sub_matrix_size);

  if ((coordinate[0] + step) % proc_per_row_col == coordinate[1]) {

    MPI_Bcast(local_matrix_1, sub_matrix_size, MPI_DOUBLE, row_root,
              *rows_comm);

    if (step == 0) {
      matrix_product(local_matrix_1, local_matrix_2, local_result, sub_size);
    }

  } else {
    double *receive_buffer_1 =
        (double *)malloc(sizeof(double) * sub_matrix_size);

    // receive matrix 1 from broadcast
    if (!receive_buffer_1) {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    MPI_Bcast(receive_buffer_1, sub_matrix_size, MPI_DOUBLE, row_root,
              *rows_comm);

    matrix_product(local_matrix_2, receive_buffer_1, local_result, sub_size);

    free(receive_buffer_1);
  }
  return;
}
