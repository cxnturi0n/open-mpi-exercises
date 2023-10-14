#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define WRONG_ARG_MESSAGE                                                      \
  "Usage: ./sum_n_strat_1 n\n where b is the number of elements to"

#define NOT_POW_OF_2_MESSAGE                                                   \
  "[ERR]: input size can't be decomposed because is not a pow of 2"

typedef enum { TAG_MERGE, TAG_DISTRIBUTE } tag_t;

void distribute_data(int nprocs, int rest, double *data, int send_size);
void receive_data(double *data, int size);
void merge_data(int nprocs, int rank, int merge_steps, double *data);
int fast_log_2(int n);
int population_count(int n);

int main(int argc, char *argv[]) {
  int rank, nprocs;
  if (argc != 2) {
    printf(WRONG_ARG_MESSAGE);
    exit(1);
  }

  int data_size = atoi(argv[1]);

  //////////////////////////////////////////
  /// Can be done only inside process 1/////

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  // local sum
  double sum = 0;
  // sum received by other process
  double received_sum = 0;

  int communication_size = data_size / nprocs;
  int rest = data_size % nprocs;

  int merge_steps = fast_log_2(nprocs);

  double *local_data = (double *)malloc(communication_size * sizeof(double));

  if (rank < rest) {
    communication_size++;
  }

  if (merge_steps == -1) {
    if (rank == 0) {
      // print error message only from master process
      printf(NOT_POW_OF_2_MESSAGE);
    }
    exit(1);
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

  // all the process do the local sum
  for (i = 0; i < communication_size; i++) {
    sum += local_data[i];
  }

  // not used anymore
  free(local_data);

  merge_data(nprocs, rank, merge_steps, &sum);

  printf("In process %d the total sum: %lf\n", rank, sum);

  MPI_Finalize();

  return 0;
}

// maybe use size_t
// works for all strategy
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

void merge_data(int nprocs, int rank, int merge_steps, double *sum) {
  int pow = 2;
  int offset_id = 1;
  int step;
  double received_sum = 0;

  for (step = 0; step < merge_steps; step++) {
    if (rank % pow < offset_id) {
      // receive then send
      MPI_Recv(&received_sum, 1, MPI_DOUBLE, rank + offset_id, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(sum, 1, MPI_DOUBLE, rank + offset_id, TAG_MERGE, MPI_COMM_WORLD);
    } else {
      // send then receive
      MPI_Send(sum, 1, MPI_DOUBLE, rank - offset_id, TAG_MERGE, MPI_COMM_WORLD);
      MPI_Recv(&received_sum, 1, MPI_DOUBLE, rank - offset_id, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    *sum += received_sum;
    offset_id = pow;
    pow <<= 1;
  }

  return;
}

// log_2 in O(log_2(n))
int fast_log_2(int n) {
  // can be done faster
  if (!n || (n & (n - 1))) {
    return -1;
  }
  return population_count(n - 1);
}

// O(log_2(n)), where s is the number of bits set to 1 in n
int population_count(int n) {
  int count = 0;
  while (n != 0) {
    n = n & (n - 1);
    count++;
  }
  return count;
}
