#!/usr/bin/env /project/test/bats-core/bin/bats

WORKDIR="$(pwd)/.."

hostname > $WORKDIR/hostfile
mpicc $WORKDIR/main.c $WORKDIR/strategies.c $WORKDIR/distribute.c $WORKDIR/utils.c $WORKDIR/bitwise_algo.c -o $WORKDIR/sum_n_nums

touch $WORKDIR/Sum.out
>$WORKDIR/Sum.out
touch $WORKDIR/Sum.err
>$WORKDIR/Sum.err 

teardown(){
  >$WORKDIR/Sum.out
  >$WORKDIR/Sum.err
}

@test "test_size_is_not_number_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 'definitely not a number' 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'given input is not a valid number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
  
}

@test "test_size_is_negative_integer_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums '-10000' 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'given input is not a valid number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_size_is_not_a_integer_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 3.14159 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'given input is not a valid number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]

}

@test "test_strategy_is_greater_[1,2,3]_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 1000 4 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'wrong strategy, choose between 1, 2, 3' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_strategy_is_smaller_[1,2,3]_should_fail" {
  run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 1000 0 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 1 ]

  # Check that "Sum.err" contains expected error log
  grep -q 'wrong strategy, choose between 1, 2, 3' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_numproc_not_power_of_two_with_strategy_2_should_redirect_to_strategy_1" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 3 $WORKDIR/sum_n_nums 1000 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that program redirects to strategy 1
  strategy="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 3)"
  [ "$strategy" == "Strategy 1" ]
}

@test "test_numproc_not_power_of_two_with_strategy_3_should_redirect_to_strategy_1" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 3 $WORKDIR/sum_n_nums 1000 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that program redirects to strategy 1
  strategy="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 3)"
  [ "$strategy" == "Strategy 1" ]
}


@test "test_expected_sum_is_equal_to_sum_strategy_1_100_with_1_proc" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 1 $WORKDIR/sum_n_nums 100 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_1_1000_with_1_proc" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 1 $WORKDIR/sum_n_nums 1000 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_1_10000_with_1_proc" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 1 $WORKDIR/sum_n_nums 10000 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_1_100_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 100 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_1_1000_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 1000 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_1_10000_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 10000 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_1_100_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 100 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_1_1000_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 1000 1 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]

}

@test "test_expected_sum_is_equal_to_sum_strategy_2_100_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 100 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_2_1000_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 1000 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}


@test "test_expected_sum_is_equal_to_sum_strategy_2_10000_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 10000 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_2_100_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 100 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_2_1000_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 1000 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}


@test "test_expected_sum_is_equal_to_sum_strategy_2_10000_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 10000 2 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_3_100_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 100 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_3_1000_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 1000 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_3_10000_with_2_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 2 $WORKDIR/sum_n_nums 10000 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_3_100_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 100 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_3_1000_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 1000 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}

@test "test_expected_sum_is_equal_to_sum_strategy_3_10000_with_4_procs" {
 run bash -c "mpirun --hostfile $WORKDIR/hostfile -n 4 $WORKDIR/sum_n_nums 10000 3 > $WORKDIR/Sum.out 2> $WORKDIR/Sum.err"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Check that result is equal to expected
  expected="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 4)"
  result="$(tail -n 1 $WORKDIR/Sum.out | cut -d , -f 5)"
  [ "$result" == "$expected" ]
}
