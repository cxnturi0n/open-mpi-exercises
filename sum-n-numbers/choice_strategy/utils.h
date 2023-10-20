#ifndef UTILS_H_
#define UTILS_H_

#include "bitwise_algo.h"
#include "constants.h"
#include "strategies.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int is_number(char *input);
double random_number();
void check_if_strategy_is_applicable(strategy_t *strategy, int nprocs);
strategy_t parse_strategy(char *input);
char* strategy_to_str(strategy_t selected_strategy);

#endif // UTILS_H_
