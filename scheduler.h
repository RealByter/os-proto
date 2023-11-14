#include "types.h"
#include "isr.h"
#include "isr.h"

typedef enum {
  READY, 
  RUNNING,
  DEAD
} status_t;

typedef struct process_t {
  status_t status;
  cpu_state_t context;
  struct process_t* next;
} process_t;

void schedule(cpu_state_t* context);
process_t* create_process(void (*function)(void*), void* arg);