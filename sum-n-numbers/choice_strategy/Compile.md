# Compilation instruction

First run:
```bash
mpicc main.c bitwise_algo.c distribute.c strategies.c utils.c -o sum_n_numbers
```
Then run with `mpirun` or `mpiexec`, here an example
```bash
mpirun -n 4 --hostfile hostfile sum_n_numbers 1000000 3
```
In this case *mpi* run with the following configuration:
- Use the address inside `hostfile` to resolve the machine to use
- Run with `4` processs
- `sum_n_numbers` runs with:
  - `1000000` as input number
  - Use strategy `3`

# Local testing

Build docker image:
```bash
docker build . -t openmpi-batscore
```

Launch and interact with container:
```bash
docker run -it  openmpi-batscore
```

Change directory to test and execute tests:
```bash
cd test && ./test.bats
```
