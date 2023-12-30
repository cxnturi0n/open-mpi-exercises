#include "../include/broadcast_rolling_multiply.h"
#include "../include/messages.h"
#include "../include/utils.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  // first diagonal
  int row_root = coordinate[0];

  double *buffer = (double *)calloc(total_size, sizeof(double));
  if (!buffer) {
    fprintf(stderr, MALLOC_FAILURE);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  for (int step = 0; step < proc_in_group; step++) {
    // if I'm on the i-th step diagonal
    int row_root = (coordinate[0] + step) % proc_in_group;
    if ((coordinate[0] + step) % proc_in_group == coordinate[1]) {
      for (int i = 0; i < total_size; i++) {
        buffer[i] = sub_matrix_1[i];
      }
    }

    MPI_Bcast(buffer, total_size, MPI_DOUBLE, row_root, *rows_comm);

    if (step > 0) {
      int receive_from = (coordinate[0] - 1) % proc_in_group;
      int send_to = (coordinate[0] + 1) % proc_in_group;

      // request will be ignored for simplicity
      MPI_Request req;
      MPI_Isend(sub_matrix_2, total_size, MPI_DOUBLE, send_to,
                TAG_SHARE_MATRIX_2, *cols_comm, &req);

      MPI_Recv(sub_matrix_2, total_size, MPI_DOUBLE, receive_from,
               TAG_SHARE_MATRIX_2, *cols_comm, MPI_STATUS_IGNORE);
    }

    matrix_product(buffer, sub_matrix_2, sub_result, size);
  }

  free(buffer);
  return;
}
