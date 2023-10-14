#include "mpi.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WRONG_ARG_MESSAGE "Usage: ./sum_n_strat_1 n strategy"
#define NOT_A_NUMBER_ERROR_MESSAGE "[ERROR]: given input is not a number\n"
#define STRATEGY_WRONG_MESSAGE "[ERROR]: wrong strategy, choose between 1, 2, 3"

typedef enum { TAG_MERGE, TAG_DISTRIBUTE } tag_t;
typedef enum { STRATEGY_1, STRATEGY_2, STRATEGY_3, STRATEGY_WRONG } strategy_t;

int is_numer(char *input);
strategy_t parse_strategy(char *input);
void check_if_strategy_is_applicable(strategy_t *strategy, int nprocs);

double random_number();

void distribute_data(int nprocs, int extra_numbers, double *data,
                     int send_size);
void receive_data(double *data, int size);

void merge_data_strategy_1(int nprocs, int rank, double *data);
void merge_data_strategy_2(int nprocs, int rank, double *data);
void merge_data_strategy_3(int nprocs, int rank, double *data);

int fast_log_2(int n);
int population_count(int n);

void (*USE_MERGE_STRATEGY[])(int nprocs, int rank, double *data) = {
    [STRATEGY_1] = merge_data_strategy_1,
    [STRATEGY_2] = merge_data_strategy_2,
    [STRATEGY_3] = merge_data_strategy_3,
};

int main(int argc, char *argv[]) {
  srand(time(NULL));

  if (argc != 3) {
    printf(WRONG_ARG_MESSAGE);
    exit(1);
  }

  int rank, nprocs;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  // TODO: check and parse argument if it is an integer
  if (!is_numer(argv[1])) {
    if (rank == 0) {
      printf(NOT_A_NUMBER_ERROR_MESSAGE);
    }
    exit(1);
  }
  int size = atoi(argv[1]);

  strategy_t selected_strategy = STRATEGY_1;

  selected_strategy = parse_strategy(argv[2]);
  if (selected_strategy == STRATEGY_WRONG) {
    if (rank == 0) {
      printf(STRATEGY_WRONG_MESSAGE);
    }
    exit(1);
  }

  // go back to strategy 1 if 2 or 3 is not applicable
  check_if_strategy_is_applicable(&selected_strategy, nprocs);

  double time_start, time_end, time;

  // local sum
  double sum = 0;

  // per process input size
  int local_size = size / nprocs;
  // extra input numbers to distribute across the first `size % nprocs`
  // processors.
  int extra_numbers = size % nprocs;

  // number of steps required to merge all the partial sum from other processes
  int merge_steps = fast_log_2(nprocs);

  // per process input data
  double *local_data = (double *)malloc(local_size * sizeof(double));

  if (rank < extra_numbers) {
    local_size++;
  }

  // distribute the data to all the other processors
  int i;
  if (rank == 0) {
    // MASTER
    double *data = (double *)malloc(size * sizeof(double));
    // the expected sum is `size`
    for (i = 0; i < size; i++) {
      data[i] = random_number();
    }

    // save data for process 0
    for (i = 0; i < local_size; i++) {
      local_data[i] = data[i];
    }

    distribute_data(nprocs, extra_numbers, data, local_size);
    free(data);
  } else {
    // SLAVE
    // receive data from others
    receive_data(local_data, local_size);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  time_start = MPI_Wtime();

  // all the process do the local sum
  for (i = 0; i < local_size; i++) {
    sum += local_data[i];
  }
  // not used anymore
  free(local_data);

  // merge strategy
  USE_MERGE_STRATEGY[selected_strategy](nprocs, rank, &sum);
  time_end = MPI_Wtime();

  if (rank == 0) {
    printf("In process %d the total sum: %lf\n", rank, sum);
    time = time_end - time_start;
    printf("%lf elapsed\n", time);
  }

  MPI_Finalize();

  return 0;
}

int is_numer(char *input) {
  int i;
  char c;
  if (!strlen(input)) {
    return 0;
  }
  while (input[i] != '\0') {
    c = input[i];
    if (!isdigit(c)) {
      return 0;
    }
    i++;
  }
  return 1;
}

strategy_t parse_strategy(char *input) {
  if (strlen(input) > 1) {
    return STRATEGY_WRONG;
  }
  strategy_t selected = STRATEGY_WRONG;
  if (input[0] == '1') {
    selected = STRATEGY_1;
  } else if (input[0] == '2') {
    selected = STRATEGY_2;
  } else if (input[0] == '3') {
    selected = STRATEGY_3;
  }
  return selected;
}

// redirect to strategy 1 if the number of processors is not a power of 2
void check_if_strategy_is_applicable(strategy_t *strategy, int nprocs) {
  if ((*strategy == STRATEGY_2 || *strategy == STRATEGY_3) &&
      (nprocs & (nprocs - 1))) {
    *strategy = STRATEGY_1;
  }
}

double random_number() {
  /* return (double)rand() / RAND_MAX; */
  return 1;
}

// maybe use size_t
// works for all strategy
void distribute_data(int nprocs, int extra_numbers, double *data,
                     int send_size) {
  int process;
  for (process = 1; process < nprocs; process++) {
    if (process == extra_numbers) {
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

void merge_data_strategy_1(int nprocs, int rank, double *sum) {
  if (rank == 0) {
    int step;
    double partial_sum;
    for (step = 1; step < nprocs; step++) {
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, step, TAG_MERGE, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      *sum += partial_sum;
    }
  } else {
    MPI_Send(sum, 1, MPI_DOUBLE, 0, TAG_MERGE, MPI_COMM_WORLD);
  }
}

void merge_data_strategy_2(int nprocs, int rank, double *sum) {
  int pow = 2;
  int offset_id = 1;
  int step;
  double partial_sum = 0;
  int merge_steps = fast_log_2(nprocs);

  for (step = 0; step < merge_steps; step++) {
    if (rank % pow == 0) {
      // Receivea from slave
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, rank + offset_id, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      *sum += partial_sum;
    } else if (rank % pow == offset_id) {
      // Send to master process
      MPI_Send(sum, 1, MPI_DOUBLE, rank - offset_id, TAG_MERGE, MPI_COMM_WORLD);
    }
    offset_id = pow;
    pow <<= 1;
  }
}

void merge_data_strategy_3(int nprocs, int rank, double *sum) {
  int pow = 2;
  int offset_id = 1;
  int step;
  double partial_sum = 0;
  int merge_steps = fast_log_2(nprocs);

  for (step = 0; step < merge_steps; step++) {
    if (rank % pow < offset_id) {
      // receive then send
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, rank + offset_id, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(sum, 1, MPI_DOUBLE, rank + offset_id, TAG_MERGE, MPI_COMM_WORLD);
    } else {
      // send then receive
      MPI_Send(sum, 1, MPI_DOUBLE, rank - offset_id, TAG_MERGE, MPI_COMM_WORLD);
      MPI_Recv(&partial_sum, 1, MPI_DOUBLE, rank - offset_id, TAG_MERGE,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    *sum += partial_sum;
    offset_id = pow;
    pow <<= 1;
  }
}

// O(log_2(n))
int fast_log_2(int n) {
  // can be done faster
  if (!n || (n & (n - 1))) {
    return -1;
  }
  return population_count(n - 1);
}

// O(log_2(n)), where n is length of the word
int population_count(int n) {
  int count = 0;
  while (n != 0) {
    n = n & (n - 1);
    count++;
  }
  return count;
}
