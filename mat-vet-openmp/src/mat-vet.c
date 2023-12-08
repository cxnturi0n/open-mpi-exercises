#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MEMORY_ERR_MSG "Malloc Out of Memory"
#define MISSING_ARG_MSG \
  "Missing input argument, usage: ./mat-vet row-size column-size num-threads"
#define TOO_MANY_ARG_MSG \
  "To many input argument, usage: ./mat-vet row-size column-size num-threads"
#define NOT_A_NUMER_MSG "Argument is not a number"
#define NEGATIVE_ARG_MSG "Argument is Negative or not Integer"
#define NEGATIVE_NUM_THREAD_ERR "Number of threads not allowed"
#define LESS_THAN_THREADS_ERR "Input matrix size is less than thread pool size"

double random_number();
double *matxvet_strat(int col_size, int row_size, double *vector,
                      double **matrix);

int is_number(char *s);

int main(int argc, char **argv)
{
  srand(time(NULL));

  if (argc < 4)
  {
    fprintf(stderr, "%s\n", MISSING_ARG_MSG);
    exit(EXIT_FAILURE);
  }

  if (argc > 4)
  {
    fprintf(stderr, "%s\n", TOO_MANY_ARG_MSG);
    exit(EXIT_FAILURE);
  }

  if (!is_number(argv[1]))
  {
    fprintf(stderr, "%s\n", NOT_A_NUMER_MSG);
    exit(EXIT_FAILURE);
  }

  if (!is_number(argv[2]))
  {
    fprintf(stderr, "%s\n", NOT_A_NUMER_MSG);
    exit(EXIT_FAILURE);
  }

  int row_size = atoi(argv[1]);
  int col_size = atoi(argv[2]);

  if (row_size <= 0)
  {
    fprintf(stderr, "%s\n", NEGATIVE_ARG_MSG);
    exit(EXIT_FAILURE);
  }

  if (col_size <= 0)
  {
    fprintf(stderr, "%s\n", NEGATIVE_ARG_MSG);
    exit(EXIT_FAILURE);
  }

  if (!is_number(argv[3]))
  {
    fprintf(stderr, "%s\n", NOT_A_NUMER_MSG);
    exit(EXIT_FAILURE);
  }

  int pool_size = atoi(argv[3]);

  if (pool_size <= 0)
  {
    fprintf(stderr, "%s\n", NEGATIVE_NUM_THREAD_ERR);
    exit(EXIT_FAILURE);
  }

  omp_set_num_threads(4);

  if (col_size < pool_size || row_size < pool_size)
  {
    fprintf(stderr, "%s\n", LESS_THAN_THREADS_ERR);
    exit(EXIT_FAILURE);
  }

  // don't bother to deallocate previous arrays
  // Reserving space for n*m elements of the matrix
  double **matrix = (double **)malloc(row_size * sizeof(double *));
  if (matrix == NULL)
  {
    fprintf(stderr, "%s\n", MEMORY_ERR_MSG);
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < row_size; i++)
  {
    matrix[i] = (double *)malloc(col_size * sizeof(double));
    if (matrix[i] == NULL)
    {
      fprintf(stderr, "%s\n", MEMORY_ERR_MSG);
      exit(EXIT_FAILURE);
    }
  }

  // Matrix initialization
  for (int i = 0; i < row_size; i++)
  {
    for (int j = 0; j < col_size; j++)
    {
      matrix[i][j] = random_number();
    }
  }

  // Column vector initialization
  double *vector = (double *)malloc(row_size * sizeof(double));
  if (vector == NULL)
  {
    fprintf(stderr, "%s\n", MEMORY_ERR_MSG);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < row_size; i++)
  {
    vector[i] = random_number();
  }

  double time;
  clock_t time_start, time_end;
  time_start = clock();

  double *result = matxvet_strat(row_size, col_size, vector, matrix);

  time_end = clock();
  time = (double)(time_end - time_start) / CLOCKS_PER_SEC;

  // REPORT
  // row-size, col-size, num_threads, time
  fprintf(stdout, "%d, %d, %d, %lf\n", row_size, col_size, pool_size, time);

  for (int i = 0; i < row_size ; i++)
  {
      if(result[i] != col_size){
        fprintf(stderr, "Matrix computation went wrong");
        exit(EXIT_FAILURE);
      }
  }

  // Freeing up space
  free(result);
  for (int i = 0; i < row_size; i++)
  {
    free(matrix[i]);
  }
  free(matrix);

  return 0;
}

int is_number(char *s)
{
  int i;
  for (i = 0; s[i] != '\0'; i++)
  {
    if (s[i] < '0' || s[i] > '9')
    {
      return 0;
    }
  }
  return 1;
}

double random_number() { return 1; }

double *matxvet_strat(int row_size, int col_size, double *vector,
                      double **matrix)
{
  int i;
  int j;
  double *result = (double *)calloc(row_size, sizeof(double));
  if (result == NULL)
  {
    fprintf(stderr, "%s\n", MEMORY_ERR_MSG);
    exit(EXIT_FAILURE);
  }

#pragma omp parallel for default(none) shared(row_size, col_size, matrix, vector, result) private(i, j)
  for (i = 0; i < row_size; i++)
  {
    for (j = 0; j < col_size; j++)
    {
      result[i] += matrix[i][j] * vector[j];
    }
  }
  return result;
}
