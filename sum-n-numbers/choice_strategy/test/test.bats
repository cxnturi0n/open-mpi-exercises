#!/usr/bin/env /homes/DMA/PDC/2024/CNCFBA98U/bats-core/bin/bats

WORKDIR="/homes/DMA/PDC/2024/CNCFBA98U/projects/sum-n-numbers/choice_strategy"
touch $WORKDIR/Sum.out
>$WORKDIR/Sum.out
touch $WORKDIR/Sum.err
>$WORKDIR/Sum.err 

teardown(){
  >$WORKDIR/Sum.out
  >$WORKDIR/Sum.err
}

@test "test_size_is_not_number_should_fail" {
  run qsub -l nodes=1:ppn=1 -v SIZE="definitely not a number",STRATEGY=2 -V "$WORKDIR/Sum.pbs"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Wait for "Sum.err" to be not empty
  while [ ! -s "$WORKDIR/Sum.err" ]; do
    sleep 1  # Wait for 1 second before checking again
  done

  # Check that "Sum.err" contains expected error log
  grep -q 'given input is not a number' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
  
}

@test "test_strategy_not_in_[1,2,3]_should_fail" {
  run qsub -l nodes=1:ppn=1 -v SIZE=1000,STRATEGY=4 -V "$WORKDIR/Sum.pbs"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Wait for "Sum.err" to be not empty
  while [ ! -s "$WORKDIR/Sum.err" ]; do
    sleep 1  # Wait for 1 second before checking again
  done

  # Check that "Sum.err" contains expected error log
  grep -q 'wrong strategy, choose between 1, 2, 3' "$WORKDIR/Sum.err"
  [ "$?" -eq 0 ]
}

@test "test_numproc_not_power_of_two_should_redirect_to_strategy_1" {
  run qsub -l nodes=1:ppn=3 -v SIZE=1000,STRATEGY=2 -V "$WORKDIR/Sum.pbs"

  # Check that the exit status is 0
  [ "$status" -eq 0 ]

  # Wait for "Sum.out" to be not empty
  while [ ! -s "$WORKDIR/Sum.out" ]; do
    sleep 1  # Wait for 1 second before checking again
  done

  # Check that program redirects to strategy 1
  strategy="$(tail -n 1 prova | cut -d , -f 3)"
  [ "$strategy" == "Strategy 1" ]
}


