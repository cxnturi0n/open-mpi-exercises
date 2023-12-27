#include "../include/broadcast_rolling_multiply.h"
#include "../include/messages.h"
#include "../include/utils.h"
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void broadcast_rolling_multiply(double *local_matrix_1, double *local_matrix_2,
                                double *local_result, MPI_Comm *torus,
                                MPI_Comm *rows_comm, MPI_Comm *cols_comm,
                                int sub_size, int proc_per_row_col, int step) {

  int sub_matrix_size = sub_size * sub_size;

  // get process coordinates
  int coordinate[] = {0, 0};
  int dims[] = {sub_size, sub_size};
  int periods[] = {1, 1};

  MPI_Cart_get(*torus, 2, dims, periods, coordinate);
  // check that it is on the diagonal

  if ((coordinate[0] + step) % proc_per_row_col == coordinate[1]) {
    int sender_rank;
    MPI_Cart_rank(*rows_comm, coordinate, &sender_rank);
    // send local matrix 1 (broadcast on the same rows)
    MPI_Bcast(local_matrix_1, sub_matrix_size, MPI_DOUBLE, sender_rank,
              *rows_comm);

    MPI_Cart_rank(*cols_comm, coordinate, &sender_rank);
    // send local matrix 2 (broadcast on the same cols)
    MPI_Bcast(local_matrix_2, sub_matrix_size, MPI_DOUBLE, sender_rank,
              *cols_comm);

    matrix_product(local_matrix_1, local_matrix_2, local_result, sub_size);

  } else {
    double *receive_buffer_1 =
        (double *)malloc(sizeof(double) * sub_matrix_size);
    double *receive_buffer_2 =
        (double *)malloc(sizeof(double) * sub_matrix_size);

    if (!receive_buffer_1 || !receive_buffer_2) {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // get emitter coordinate
    // the emitter is on the same row but
    // the column is shifted by `step` to the right wrapping toward 0
    /* int coord_emitter_row[] = {coordinate[0], */
    /*                            (coordinate[0] + step) % proc_per_row_col}; */
    // int coord_emitter_row[] = {(coordinate[0] + step) % proc_per_row_col};
    int coord_emitter_row[] = {coordinate[0]};

    int emitter_id_row;
    // obtain the id of the emitter

    MPI_Cart_rank(*rows_comm, coord_emitter_row, &emitter_id_row);

    /* int coord_emitter_col[] = {(coordinate[1] - step) % proc_per_row_col, */
    /*                            coordinate[1]}; */
    // int coord_emitter_col[] = {(coordinate[1] - step) % proc_per_row_col};
    int coord_emitter_col[] = {coordinate[1]};
    int emitter_id_col;
    // obtain the id of the emitter
    MPI_Cart_rank(*cols_comm, coord_emitter_col, &emitter_id_col);
    int my_row, my_col;
    MPI_Comm_rank(*rows_comm, &my_row);
    MPI_Comm_rank(*cols_comm, &my_col);

    /* printf("my coord[%d,%d]\temitter row := %d\t emitter col := %d\n", */
    /*        coordinate[0], coordinate[1], emitter_id_row, emitter_id_col); */
    printf("my coord[%d,%d]\tcoord in row %d\tcoord in col %d\n", coordinate[0],
           coordinate[1], my_row, my_col);

    MPI_Recv(receive_buffer_1, sub_matrix_size, MPI_DOUBLE, emitter_id_col, 0,
             *rows_comm, MPI_STATUS_IGNORE);

    MPI_Recv(receive_buffer_2, sub_matrix_size, MPI_DOUBLE, emitter_id_row, 0,
             *cols_comm, MPI_STATUS_IGNORE);

    matrix_product(local_matrix_1, receive_buffer_2, local_result,
                   sub_matrix_size);

    matrix_product(local_matrix_2, receive_buffer_1, local_result,
                   sub_matrix_size);
    free(receive_buffer_1);
    free(receive_buffer_2);
  }

  /* printf("here coords[%d, %d], step := %d, diag? := %c\n", coordinate[0], */
  /*        coordinate[1], step, */
  /*        (coordinate[0] + step) % proc_per_row_col == coordinate[1] ? 'y' */
  /*                                                                   : 'n');
   */
  return;
}
