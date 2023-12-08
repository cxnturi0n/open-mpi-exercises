#!/usr/bin/env /project/bats-core/bin/bats

setup(){
  WORKDIR="$(pwd)/.."

  gcc -fopenmp -lgomp -std=c99 -o $PBS_O_WORKDIR/mat-vet $PBS_O_WORKDIR/mat-vet.c
  touch $WORKDIR/Sum.out
  >$WORKDIR/Sum.out
  touch $WORKDIR/Sum.err
  >$WORKDIR/Sum.err 
}

teardown(){
  >$WORKDIR/Sum.out
  >$WORKDIR/Sum.err
}

@test "test_less_than_3_args" {
  run bash -c "$WORKDIR/mat-vet less-than-two"
  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  grep -q 'Missing input argument, usage: ./mat-vet row-size column-size num-threads' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_more_than_3_args" {
  run bash -c "$WORKDIR/mat-vet first second third fourth"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
 grep -q 'To many input argument, usage: ./mat-vet row-size column-size num-threads' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_first_argument_is_not_a_number" {
  run bash -c "$WORKDIR/mat-vet first 42 10"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is not a number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_second_argument_is_not_a_number" {
  run bash -c "$WORKDIR/mat-vet 42 second 10"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is not a number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_first_argument_is_not_a_valid_number" {
  run bash -c "$WORKDIR/mat-vet 0 10 10"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is Negative or not Integer' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_second_argument_is_not_a_valid_number" {
  run bash -c "$WORKDIR/mat-vet 200 3.14159 10"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is Negative or not Integer' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_columns_invalid_num_threads" {
  run bash -c "$WORKDIR/mat-vet 50 1 -3"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Number of threads not allowed' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_columns_less_than_threads" {
  run bash -c "$WORKDIR/mat-vet 50 1 100"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Input matrix size is less than thread pool size' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}
