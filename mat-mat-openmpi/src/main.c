#include "mpi.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/broadcast_rolling_multiply.h"
#include "../include/communicator.h"
#include "../include/messages.h"
#include "../include/utils.h"

int main(int argc, char *argv[])
{
  srand(time(NULL));

  int rank, nprocs;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  // parametri costruzione toro
  // assumo toro bidimensionale (matrice wrappata)
  // toro di comunicazione, comunicatore per righe, comunicatore per colonne
  MPI_Comm torus, rows, cols;
  int number_row_procs = (int)sqrt(nprocs);
  int number_col_procs = nprocs / number_row_procs;
  int coordinates[] = {0, 0};
  int size = atoi(argv[1]);

  if (!is_number(argv[1]))
  {
    if (rank == 0)
    {
      fprintf(stderr, NOT_A_VALID_NUMBER_INPUT);
    }
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  if (size < nprocs)
  {
    if (rank == 0)
    {
      fprintf(stderr, SIZE_LESS_THAN_PROCS);
    }
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  if (nprocs % number_row_procs != 0)
  {
    if (rank == 0)
    {
      fprintf(stderr, PROC_NOT_MUL_OF_ROW_PROCS);
    }
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  if (number_row_procs != number_col_procs)
  {
    if (rank == 0)
    {
      fprintf(stderr, PROC_ROW_NEQ_PROC_COL);
    }
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // build communication torus
  build_2D_torus(&torus, &rows, &cols, number_row_procs, number_col_procs);
  MPI_Barrier(MPI_COMM_WORLD);

  double **matrix_1, **matrix_2;

  double *local_matrix_1;
  double *local_matrix_2;
  double *local_result;
  // root process
  if (rank == 0)
  {
    matrix_1 = (double **)malloc(size * sizeof(double *));
    matrix_2 = (double **)malloc(size * sizeof(double *));

    if (!matrix_1 || !matrix_2)
    {
      if (rank == 0)
      {
        fprintf(stderr, MALLOC_FAILURE);
      }
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++)
    {
      matrix_1[i] = (double *)malloc(size * sizeof(double));
      matrix_2[i] = (double *)malloc(size * sizeof(double));

      if (!matrix_1[i] || !matrix_2[i])
      {
        if (rank == 0)
        {
          fprintf(stderr, MALLOC_FAILURE);
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
    }

    // fill matrices
    for (int i = 0; i < size; i++)
    {
      for (int j = 0; j < size; j++)
      {
        matrix_1[i][j] = random_number();
        matrix_2[i][j] = random_number();
      }
    }
  }

  int sub_size = size / number_row_procs;

  local_matrix_1 = (double *)calloc(sub_size * sub_size, sizeof(double));
  local_matrix_2 = (double *)calloc(sub_size * sub_size, sizeof(double));
  local_result = (double *)calloc(sub_size * sub_size, sizeof(double));
  if (!local_matrix_1 || !local_matrix_2 || !local_result)
  {
    if (rank == 0)
    {
      fprintf(stderr, MALLOC_FAILURE);
    }
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }
  // distribute data
  distribute(nprocs, rank, matrix_1, matrix_2, local_matrix_1, local_matrix_2,
             size, sub_size);

  if (rank == 0)
  {
    free(matrix_1);
    free(matrix_2);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  int **emitters_row = (int **)malloc(number_col_procs * sizeof(int *));
  int **emitters_col = (int **)malloc(number_col_procs * sizeof(int *));

  if (!emitters_row || !emitters_col)
  {
    fprintf(stderr, MALLOC_FAILURE);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  for (int i = 0; i < number_row_procs; i++)
  {
    emitters_row[i] = (int *)calloc(number_col_procs, sizeof(int));
    emitters_col[i] = (int *)calloc(number_col_procs, sizeof(int));
    if (!emitters_row[i] || !emitters_col[i])
    {
      fprintf(stderr, MALLOC_FAILURE);
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
  }

  double start = MPI_Wtime();

  broadcast_rolling_multiply(local_matrix_1, local_matrix_2, local_result,
                             &torus, &rows, &cols, sub_size, number_row_procs);

  double end = MPI_Wtime();

  double delta_time = end - start;

  double max_time = 0;
  MPI_Reduce(&max_time, &delta_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    // number of procs, matrix size, time elapsed
    printf("%d, %d, %lf\n", nprocs, size, max_time);
  }

  double **result = recompose_result(local_result, sub_size, size, rank, nprocs,
                                     number_row_procs, &torus);

  // print result
  /* if (rank == 0) { */
  /*   printf("RESULT:\n"); */
  /*   for (int i = 0; i < size; i++) { */
  /*     for (int j = 0; j < size; j++) { */
  /*       printf("%lf, ", result[i][j]); */
  /*     } */
  /*     printf("\n"); */
  /*   } */
  /*   printf("\n\n"); */
  /* } */

  free(local_matrix_1);
  free(local_matrix_2);
  free(local_result);
  if (rank == 0)
  {
    for (int i = 0; i < size; i++)
    {
      free(result[i]);
    }
    free(result);
  }
  MPI_Finalize();
  return 0;
}
