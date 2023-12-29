#include "../include/broadcast_rolling_multiply.h"
#include "../include/messages.h"
#include "../include/utils.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setup_emitters(int **emitters_row, int **emitters_col, int size,
                    int step) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      emitters_row[i][j] = (i + step) % size;
      emitters_col[i][j] = (j + step) % size;
    }
  }
}

void broadcast_rolling_multiply(double *sub_matrix_1, double *sub_matrix_2,
                                double *sub_result, MPI_Comm *torus,
                                MPI_Comm *rows_comm, MPI_Comm *cols_comm,
                                int size, int proc_in_group) {
  int total_size = size * size;

  // get process coordinates
  int coordinate[] = {0, 0};
  int dims[] = {size, size};
  int periods[] = {1, 1};

  MPI_Cart_get(*torus, 2, dims, periods, coordinate);

  int **emitters_row = (int **)malloc(proc_in_group * sizeof(int *));
  int **emitters_col = (int **)malloc(proc_in_group * sizeof(int *));

  if (!emitters_row || !emitters_col) {
    fprintf(stderr, MALLOC_FAILURE);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  for (int i = 0; i < proc_in_group; i++) {
    emitters_row[i] = (int *)calloc(proc_in_group, sizeof(int));
    emitters_col[i] = (int *)calloc(proc_in_group, sizeof(int));
    if (!emitters_row[i] || !emitters_col[i]) {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
  }

  setup_emitters(emitters_row, emitters_col, proc_in_group, 0);

  // first diagonal
  int row_root = emitters_row[coordinate[0]][coordinate[1]];
  int col_root = emitters_col[coordinate[0]][coordinate[1]];

  double *buffer = (double *)calloc(total_size, sizeof(double));
  if (!buffer) {
    fprintf(stderr, MALLOC_FAILURE);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // if it is on coordinate I copy the data on the buffer to send;
  if (coordinate[0] == coordinate[1]) {
    for (int i = 0; i < total_size; i++) {
      buffer[i] = sub_matrix_1[i];
    }
  }

  MPI_Bcast(buffer, total_size, MPI_DOUBLE, row_root, *rows_comm);

  matrix_product(buffer, sub_matrix_2, sub_result, size);

  // remaining steps
  for (int i = 1; i < proc_in_group; i++) {
    // if I'm on the i-th step diagonal
    setup_emitters(emitters_row, emitters_col, proc_in_group, 0);
    row_root = emitters_row[coordinate[0]][coordinate[1]];
    col_root = emitters_col[coordinate[0]][coordinate[1]];
    if ((coordinate[0] + i) % proc_in_group == coordinate[1]) {
      for (int i = 0; i < total_size; i++) {
        buffer[i] = sub_matrix_1[i];
      }
    }

    MPI_Bcast(buffer, total_size, MPI_DOUBLE, row_root, *rows_comm);

    int receive_from = (coordinate[0] - i) % proc_in_group;
    int send_to = (coordinate[0] + i) % proc_in_group;

    // request will be ignored for simplicity
    MPI_Request req;
    MPI_Isend(sub_matrix_2, total_size, MPI_DOUBLE, send_to, TAG_SHARE_MATRIX_2,
              *cols_comm, &req);

    MPI_Recv(sub_matrix_2, total_size, MPI_DOUBLE, receive_from,
             TAG_SHARE_MATRIX_2, *cols_comm, MPI_STATUS_IGNORE);

    matrix_product(buffer, sub_matrix_2, sub_result, size);
  }

  for (int i = 0; i < proc_in_group; i++) {
    free(emitters_row[i]);
    free(emitters_col[i]);
  }

  free(emitters_row);
  free(emitters_col);
  free(buffer);
  return;
}
