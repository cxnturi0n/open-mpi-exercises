#include "../include/broadcast_rolling_multiply.h"
#include "../include/utils.h"
#include "mpi.h"
#include <messages.h>
#include <stdlib.h>

void broadcast_rolling_multiply(double *local_matrix_1, double *local_matrix_2,
                                double *local_result, MPI_Comm *torus,
                                MPI_Comm *rows_comm, MPI_Comm *cols_comm,
                                int sub_size, int size, int step) {

  int sub_matrix_size = sub_size * sub_size;

  // get process coordinates
  int coordinate[] = {0, 0};
  int dims[] = {size, size};
  int periods[] = {1, 1};
  MPI_Cart_get(*torus, 2, dims, periods, coordinate);

  // check that it is on the diagonal
  if (coordinate[0] + step == coordinate[1]) {
    int sender_rank;
    MPI_Cart_rank(*torus, coordinate, &sender_rank);
    // send local matrix 1 (broadcast on the same rows)
    MPI_BCast(local_matrix_1, sub_matrix_size, MPI_DOUBLE, sender_rank,
              rows_comm);
    // send local matrix 2 (broadcast on the same cols)
    MPI_BCast(local_matrix_2, sub_matrix_size, MPI_DOUBLE, sender_rank,
              cols_comm);

    matrix_product(local_matrix_1, local_matrix_2, local_result,
                   sub_matrix_size);

  } else {

    double receive_buffer_1[sub_matrix_size] = {0.0};
    double receive_buffer_2[sub_matrix_size] = {0.0};

    // get emitter coordinate
    // the emitter is on the same row but
    // the column is shifted by `step` to the right wrapping toward 0
    int coord_emitter[] = {coordinate[0], (coordinate[1] + step) % size};
    int emitter_id;
    // obtain the id of the emitter
    MPI_Cart_rank(*torus, coord_emitter, &emitter_id);

    MPI_Recv(receive_buffer_1, sub_matrix_size, MPI_DOUBLE, emitter_id,
             TAG_BROADCAST_MATRIX_1, rows_comm, MPI_STATUS_IGNORE);
    MPI_Recv(receive_buffer_2, sub_matrix_size, MPI_DOUBLE, emitter_id,
             TAG_BROADCAST_MATRIX_2, cols_comm, MPI_STATUS_IGNORE);

    matrix_product(local_matrix_1, receive_buffer_2, local_result,
                   sub_matrix_size);

    matrix_product(local_matrix_2, receive_buffer_1, local_result,
                   sub_matrix_size);
  }
  return;
}
