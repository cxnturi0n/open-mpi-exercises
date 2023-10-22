#include "../include/utils.h"

int is_number(char *input) {
  int i = 0;
  char c;
  if (!strlen(input)) {
    return 0;
  }
  while (input[i] != '\0') {
    c = input[i];
    if (!isdigit(c)) {
      return 0;
    }
    i++;
  }
  return 1;
}

double random_number() {
  /* return 1; */
  /* return rand() % RAND_MAX_RANGE; */
  return (double)rand() / RAND_MAX;
}

void check_if_strategy_is_applicable(strategy_t *strategy, int nprocs) {
  // redirect to strategy 1 if
  // - the number of processors is not a power of 2
  // - and
  // - the selected strategy is 2 or 3
  if ((*strategy == STRATEGY_2 || *strategy == STRATEGY_3) &&
      !is_power_of_2(nprocs)) {
    *strategy = STRATEGY_1;
  }
}

strategy_t parse_strategy(char *input) {
  int len = strlen(input);
  if (!len || len > 1) {
    return STRATEGY_WRONG;
  }
  strategy_t selected = STRATEGY_WRONG;
  if (input[0] == '1') {
    selected = STRATEGY_1;
  } else if (input[0] == '2') {
    selected = STRATEGY_2;
  } else if (input[0] == '3') {
    selected = STRATEGY_3;
  }
  return selected;
}

char *strategy_to_str(strategy_t selected_strategy) {
  switch (selected_strategy) {
  case STRATEGY_1:
    return "Strategy 1";
  case STRATEGY_2:
    return "Strategy 2";
  case STRATEGY_3:
    return "Strategy 3";
  default:
    return "Unknown Strategy";
  }
}
