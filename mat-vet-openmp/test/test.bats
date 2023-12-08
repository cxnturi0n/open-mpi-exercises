#!/usr/bin/env /project/bats-core/bin/bats

setup(){
  WORKDIR="$PWD/.."

  gcc -fopenmp -lgomp -std=c99 -o $WORKDIR/mat-vet $WORKDIR/src/mat-vet.c
  touch $WORKDIR/Sum.out
  >$WORKDIR/Sum.out
  touch $WORKDIR/Sum.err
  >$WORKDIR/Sum.err 
}

teardown(){
  >$WORKDIR/Sum.out
  >$WORKDIR/Sum.err
}

@test "check_product_is_valid" {
  run bash -c "$WORKDIR/mat-vet 10 10 10 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"
  # Check that the exit status is 0
  [ "$status" -eq 0 ]
}

@test "test_less_than_3_args" {
  run bash -c "$WORKDIR/mat-vet 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"
  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  grep -q 'Missing input argument, usage: ./mat-vet row-size column-size num-threads' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_more_than_3_args" {
  run bash -c "$WORKDIR/mat-vet 3 4 5 6 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0

  # Check that "Sum.err" contains expected error log
 grep -q 'To many input argument, usage: ./mat-vet row-size column-size num-threads' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_first_argument_is_not_a_number" {
  run bash -c "$WORKDIR/mat-vet ciao 2 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is not a number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_second_argument_is_not_a_number" {
  run bash -c "$WORKDIR/mat-vet 3 ciao 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is not a number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_third_argument_is_not_a_number" {
  run bash -c "$WORKDIR/mat-vet 3 3 ciao > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is not a number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_first_argument_is_not_a_valid_number" {
  run bash -c "$WORKDIR/mat-vet 0 3 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is Negative or not Integer' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_second_argument_is_not_a_valid_number" {
  run bash -c "$WORKDIR/mat-vet 3 0 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Argument is Negative or not Integer' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_third_argument_is_not_a_valid_number" {
  run bash -c "$WORKDIR/mat-vet 3 3 0 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Number of threads not allowed' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_columns_invalid_num_threads" {
  run bash -c "$WORKDIR/mat-vet 50 50 0 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Number of threads not allowed' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_columns_less_than_threads" {
  run bash -c "$WORKDIR/mat-vet 3 2 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'Input matrix size' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}