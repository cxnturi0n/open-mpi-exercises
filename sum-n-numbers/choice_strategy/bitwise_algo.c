#include "bitwise_algo.h"

int is_power_of_2(int n) { return n && !(n & (n - 1)); }

// O(log_2(n))
int fast_log_2(int n) {
  // can be done faster
  if (!is_power_of_2(n)) {
    return -1;
  }
  return population_count(n - 1);
}

// O(log_2(n)), where n is length of the word
int population_count(int n) {
  int count = 0;
  while (n != 0) {
    n = n & (n - 1);
    count++;
  }
  return count;
}
