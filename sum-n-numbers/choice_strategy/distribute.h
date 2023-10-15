#ifndef DISTRIBUTE_H_
#define DISTRIBUTE_H_

#include "constants.h"
#include "mpi.h"

void distribute_data(int nprocs, int extra_numbers, double *data,
                     int send_size);
void receive_data(double *data, int size);

#endif // DISTRIBUTE_H_
