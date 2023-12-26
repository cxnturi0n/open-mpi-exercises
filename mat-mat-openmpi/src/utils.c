#include "../include/utils.h"

double random_number() {
  return 1;
  /* return rand() % MAX_RANGE; */
  /* return ((double)rand()) / RAND_MAX; */
}

// work on square matrix
// MPI require contiguos memory, hence the matrix must be treated as a vector
// instead of a vector of vectors
void matrix_product(double *matrix_1, double *matrix_2, double *result,
                    int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        result[i * size + k] += matrix_1[i * size + j] * matrix_2[j * size + k];
      }
    }
  }
}

int is_number(char *s) {
  int i;
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] < '0' || s[i] > '9') {
      return 0;
    }
  }
  return 1;
}
