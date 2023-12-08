#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  srand(time(NULL));
  if (argc != 2) {
    fprintf(stderr, "error, pass me the size of N");
    exit(EXIT_FAILURE);
  }
  int size = atoi(argv[1]);
  double sum = 0, expected = 0;
  double *data = (double *)malloc(sizeof(double) * size);
  if (!data) {
    fprintf(stderr, "not enough memory to allocate data");
    exit(EXIT_FAILURE);
  }
  int i;
  for (i = 0; i < size; i++) {
    data[i] = (double)rand() / RAND_MAX;
    expected += data[i];
  }

  clock_t start = clock();
#pragma omp parallel for reduction(+ : sum)
  for (i = 0; i < size; i++) {
    sum += data[i];
  }
  clock_t end = clock();
  double time = (double)(end - start) / ((clock_t)1000000);

  printf("%s, %d, %lf, %lf, %lf\n", argv[0], size, expected, sum, time);
  return 0;
}
