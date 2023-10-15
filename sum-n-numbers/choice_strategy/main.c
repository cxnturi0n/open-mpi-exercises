#include "constants.h"
#include "mpi.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitwise_algo.h"
#include "distribute.h"
#include "strategies.h"
#include "utils.h"

void (*USE_MERGE_STRATEGY[])(int nprocs, int rank, double *data) = {
    [STRATEGY_1] = merge_strategy_1,
    [STRATEGY_2] = merge_strategy_2,
    [STRATEGY_3] = merge_strategy_3,
};

int main(int argc, char *argv[]) {
  srand(time(NULL)); // randomize rng seed

  int rank, nprocs;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (argc != 3) {
    if (rank == 0) {
      printf(WRONG_ARG_MESSAGE);
    }
    exit(1);
  }

  if (!is_number(argv[1])) {
    if (rank == 0) {
      printf(NOT_A_NUMBER_ERROR_MESSAGE);
    }
    exit(1);
  }

  int size = atoi(argv[1]);
  // default strategy
  strategy_t selected_strategy = STRATEGY_1;
  selected_strategy = parse_strategy(argv[2]);
  if (selected_strategy == STRATEGY_WRONG) {
    if (rank == 0) {
      printf(STRATEGY_WRONG_MESSAGE);
    }
    exit(1);
  }

  // redirect to strategy 1 if 2 or 3 is not applicable
  check_if_strategy_is_applicable(&selected_strategy, nprocs);

  // to time the program
  double time_start, time_end, time;

  // local sum
  double sum = 0;
  // to check if the result is correct
  double expected_sum = 0;

  // per process input size
  int local_size = size / nprocs;
  // extra input numbers to distribute across the first `size % nprocs`
  // processors.
  int extra_numbers = size % nprocs;

  // distribute the remaining to the first `extra_numebers` processors
  if (rank < extra_numbers) {
    local_size++;
  }

  // per process input data
  double *local_data = (double *)malloc(local_size * sizeof(double));

  int i;
  if (rank == 0) {
    // MASTER, distribute the data to all process
    double *data = (double *)malloc(size * sizeof(double));

    for (i = 0; i < size; i++) {
      data[i] = random_number();
      expected_sum += data[i];
    }

    // save data for processor 0
    for (i = 0; i < local_size; i++) {
      local_data[i] = data[i];
    }

    distribute_data(nprocs, extra_numbers, data, local_size);
    free(data);
  } else {
    // SLAVE, receive input from master
    receive_data(local_data, local_size);
  }

  // Wait until all processors can start at the same moment
  MPI_Barrier(MPI_COMM_WORLD);

  time_start = MPI_Wtime();

  // all the process do the local sum
  for (i = 0; i < local_size; i++) {
    sum += local_data[i];
  }
  USE_MERGE_STRATEGY[selected_strategy](nprocs, rank, &sum);

  time_end = MPI_Wtime();

  free(local_data);
  if (rank == 0) {
    printf("Total sum: %lf\t expected: %lf\n", sum, expected_sum);
    time = time_end - time_start;
    printf("%lf elapsed\n", time);
  }

  /* printf("I'm here 3 [rank %d]\n", rank); */
  MPI_Finalize();

  return 0;
}
