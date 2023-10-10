#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define WRONG_ARG_MESSAGE                                                      \
  "Usage: ./sum_n_strat_1 n\n where b is the number of elements to"

typdef enum { TAG_MERGE, TAG_DISTRIBUTE } tag_t;

int main(int argc, char *argv[]) {
  int rank, nprocs;
  if (argc != 2) {
    printf(WRONG_ARG_MESSAGE);
  }

  int data_size = atoi(argv[1]);

  double *data = (double *)calloc(data_size, sizeof(double));
  // the expected sum is `data_size`
  for (int i = 0; i < data_size; i++) {
    data[i] = 1;
  }

  double sum = 0;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  int step = data_size / nprocs;
  int rest = data_size % nprocs;
  if (nprocs < rest) {
    step++;
  }
  rest--;
  double *slice =
      (double *)calloc(step + 1, sizeof(double)); // leave room for data

  if (rank == 0) { // master
    int distributed = 1;
    for (int i = 1; i < nprocs; i++) {
      // send, the data to all the processors except 0, the receive and
      // aggregate result
      int slice_index = 0;
      for (int process_index = 1; process_index < nprocs; process_index++) {
        if (rest >= 0) {
          rest--;
        } else if (distributed) {
          step--;
          distributed = !distributed;
        }
        MPI_Send(data + slice_index, step, MPI_DOUBLE, process_index,
                 TAG_DISTRIBUTE, MPI_COMM_WORLD);
        slice_index += step;
      }
    }
    for (int i = 0; i < step + (nprocs < rest ? 1 : 0); i++) {
      sum += data[i];
    }

    double tmp;
    for (int process_index = 1; process_index < nprocs; process_index++) {
      MPI_Recv(&tmp, 1, MPI_DOUBLE, process_index, TAG_MERGE, MPI_COMM_WORLD);
      sum += tmp;
    }
  } else {
    // slave, receive data, calculate and send back to 0
    MPI_Recv(slice, step, MPI_DOUBLE, 0, TAG_DISTRIBUTE, MPI_COMM_WORLD);
    for (int i = 0; i < step; i++) {
      sum += slice[i];
    }
    MPI_Send(&sum, 1, MPI_DOUBLE, 0, TAG_MERGE, MPI_COMM_WORLD);
  }

  free(slice);

  printf("Total sum in process %d is: %lf\n", rank, sum);
  MPI_Finalize();

  free(data);

  return 0;
}
