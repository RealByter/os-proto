#include "types.h"
#include "isr.h"

typedef enum {
  READY, 
  RUNNING,
  DEAD
} status_t;

typedef struct process_t {
  status_t status;
  cpu_state_t* context;
  struct process_t* next;
} process_t;

void init_scheduler();
process_t* create_process(void (*function)(void*), void* arg);
cpu_state_t schedule(cpu_state_t state);