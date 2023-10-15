#ifndef CONSTANTS_H_
#define CONSTANTS_H_

typedef enum { TAG_MERGE, TAG_DISTRIBUTE } tag_t;
typedef enum { STRATEGY_1, STRATEGY_2, STRATEGY_3, STRATEGY_WRONG } strategy_t;

#define WRONG_ARG_MESSAGE "Usage: ./sum_n_numbers n strategy"
#define NOT_A_NUMBER_ERROR_MESSAGE "[ERROR]: given input is not a number\n"
#define STRATEGY_WRONG_MESSAGE "[ERROR]: wrong strategy, choose between 1, 2, 3"

#define RAND_MAX_RANGE 50

#endif // CONSTANTS_H_
