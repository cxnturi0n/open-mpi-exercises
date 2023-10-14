#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define WRONG_ARG_MESSAGE                                                      \
  "Usage: ./sum_n_strat_1 n\n where b is the number of elements to"

typedef enum { TAG_MERGE, TAG_DISTRIBUTE } tag_t;

void distribute_data(int nprocs, int rest, double *data, int send_size);
void receive_data(double *data, int size);
void merge_data(int nprocs, int rank, int merge_steps, double *data);

int main(int argc, char *argv[]) {
  int rank, nprocs;
  if (argc != 2) {
    printf(WRONG_ARG_MESSAGE);
    exit(1);
  }

  int data_size = atoi(argv[1]);

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  // local sum
  double sum = 0;
  // sum received by other process
  double received_sum = 0;

  int communication_size = data_size / nprocs;
  int rest = data_size % nprocs;

  double *local_data = (double *)malloc(communication_size * sizeof(double));

  if (rank < rest) {
    communication_size++;
  }

  // distribute the data to all the other processors
  int i;
  if (rank == 0) {
    // MASTER
    double *data = (double *)malloc(data_size * sizeof(double));
    // the expected sum is `data_size`
    for (i = 0; i < data_size; i++) {
      data[i] = 1; // in real case scenario substitute with rand() / RAND_MAX;
    }

    // save data for process 0
    for (i = 0; i < communication_size; i++) {
      local_data[i] = data[i];
    }

    distribute_data(nprocs, rest, data, communication_size);
    free(data);
  } else {
    // SLAVE
    // receive data from others
    receive_data(local_data, communication_size);
  }

  for (i = 0; i < communication_size; i++) {
    sum += local_data[i];
  }
  free(local_data);

  // we can use `i` also here, but I prefer `steps` for clarity
  merge_data(nprocs, rank, nprocs, &sum);

  if (rank == 0) {
    printf("[PROCESS %d] Sum := %lf\n", rank, sum);
  }

  MPI_Finalize();

  return 0;
}

// maybe use size_t
// works for all strategy
// should run only on process 0
void distribute_data(int nprocs, int rest, double *data, int send_size) {
  int process;
  for (process = 1; process < nprocs; process++) {
    if (process == rest) {
      send_size--;
    }
    MPI_Send(data + (process * send_size), send_size, MPI_DOUBLE, process,
             TAG_DISTRIBUTE, MPI_COMM_WORLD);
  }
}

void receive_data(double *data, int size) {
  MPI_Recv(data, size, MPI_DOUBLE, 0, TAG_DISTRIBUTE, MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
}

void merge_data(int nprocs, int rank, int merge_steps, double *data) {
  if (rank == 0) {
    int step;
    double received_data;
    for (step = 1; step < merge_steps; step++) {
      MPI_Recv(&received_data, 1, MPI_DOUBLE, step, TAG_MERGE, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      *data += received_data;
    }
  } else {
    MPI_Send(data, 1, MPI_DOUBLE, 0, TAG_MERGE, MPI_COMM_WORLD);
  }
}
