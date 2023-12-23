#include "../include/distribute.h"
#include "../include/constants.h"
#include <stdio.h>

void distribute_data(int nprocs, int extra_numbers, double *data,
                     int send_size, ) {
  int process;
  int offset = 0;
  for (process = 1; process < nprocs; process++) {
    offset += send_size;
    if (process == extra_numbers) {
      send_size--;
    }
    MPI_Send(data + offset, send_size, MPI_DOUBLE, process, TAG_DISTRIBUTE,
             MPI_COMM_WORLD);
  }
}

void receive_data(double *data, int size) {
  MPI_Recv(data, size, MPI_DOUBLE, 0, TAG_DISTRIBUTE, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
}
