#ifndef MESSAGES_H_
#define MESSAGES_H_

#define PROC_NOT_MUL_OF_ROW_PROCS "[ERR: REQUIREMENT] number of processors must be multiple of number of processors per row"
#define PROC_ROW_NEQ_PROC_COL                                                  \
  "[ERR: REQUIREMENT] number of processes for row must be equal to the number "  \
  "of processes on the columns"

#define MALLOC_FAILURE                                                         \
  "[ERR: MALLOC FAILED] no more space available on the heap"

#define NOT_A_VALID_NUMBER_INPUT "[ERR: PARSE INPUT] given input is not a valid number"

#define SIZE_LESS_THAN_PROCS "[ERR: REQUIREMENT] given size should be greather or equal than processors"

#endif // MESSAGES_H_
