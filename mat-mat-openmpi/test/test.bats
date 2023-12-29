#!/usr/bin/env /project/bats-core/bin/bats

setup(){
  WORKDIR="$(pwd)"
  hostname > $WORKDIR/hostfile
  mpicc $WORKDIR/src/main.c $WORKDIR/src/utils.c $WORKDIR/src/communicator.c $WORKDIR/src/broadcast_rolling_multiply.c -o $WORKDIR/matxmat
}
@test "processors_not_multiple_of_processors_per_row_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 5 $WORKDIR/matxmat 19 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 1 ]

  grep -q 'number of processors must be multiple of number of processors per row' "$WORKDIR/MatxMat.err"
  [ "$?" -eq 0 ]

}

@test "processors_per_row_not_equal_to_processors_per_column_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/matxmat 3 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 1 ]

  grep -q 'number of processes for row must be equal to the number of processes on the columns' "$WORKDIR/MatxMat.err"
  [ "$?" -eq 0 ]

}

@test "negative_size_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/matxmat -1 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 1 ]

  grep -q 'given input is not a valid number' "$WORKDIR/MatxMat.err"
  [ "$?" -eq 0 ]

}

@test "alphanumeric_size_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/matxmat 'not-a-number' > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 1 ]

  grep -q 'given input is not a valid number' "$WORKDIR/MatxMat.err"
  [ "$?" -eq 0 ]

}

@test "size_less_than_processors_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 8 $WORKDIR/matxmat 7 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 1 ]

  grep -q 'given size should be greater or equal than processors' "$WORKDIR/MatxMat.err"
  [ "$?" -eq 0 ]

}

@test "size_equal_to_processors_should_succeed" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/matxmat 4 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ ! -s "$WORKDIR/MatxMat.err" ]

}

@test "size_greater_than_processors_should_succeed" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/matxmat 16 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ ! -s "$WORKDIR/MatxMat.err" ]

}

@test "check_matrix_product_144_size_1_proc" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 1 $WORKDIR/matxmat 144 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 144.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((144 * 144)) ]

}

@test "check_matrix_product_1008_size_1_proc" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 1 $WORKDIR/matxmat 1008 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 1008.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((1008 * 1008)) ]

}

@test "check_matrix_product_1728_size_1_proc" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 1 $WORKDIR/matxmat 1728 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 1728.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((1728 * 1728)) ]

}

@test "check_matrix_product_2160_size_1_proc" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 1 $WORKDIR/matxmat 2160 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 2160.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((2160 * 2160)) ]

}

@test "check_matrix_product_144_size_4_procs" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/matxmat 144 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 144.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((144 * 144)) ]

}

@test "check_matrix_product_1008_size_4_procs" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/matxmat 1008 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 1008.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((1008 * 1008)) ]

}

@test "check_matrix_product_1728_size_4_procs" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/matxmat 1728 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 1728.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((1728 * 1728)) ]

}

@test "check_matrix_product_2160_size_4_procs" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/matxmat 2160 > $WORKDIR/MatxMat.out 2> $WORKDIR/MatxMat.err"

  [ "$status" -eq 0 ]

  [ "$(grep -o 2160.00000 $WORKDIR/MatxMat.out | wc -l)" -eq $((2160 * 2160)) ]

}

