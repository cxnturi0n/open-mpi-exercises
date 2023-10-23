#include "benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, WRONG_ARGUMENTS_NUMBER);
    exit(EXIT_FAILURE);
  }

  int size = atoi(argv[1]);
  double *data = (double *)malloc(sizeof(double) * size);
  double sum = 0;

  clock_t time_start, time_end;
  double time;

  int i;
  for (i = 0; i < size; i++) {
    data[i] = 1;
  }

  time_start = clock();
  for (i = 0; i < size; i++) {
    sum += data[i];
  }
  time_end = clock();
  time = (double)(time_end - time_start) / CLOCKS_PER_SEC;
  free(data);
  // size, sum, time
  printf("%d, %lf, %lf\n", size, sum, time);
  return 0;
}
