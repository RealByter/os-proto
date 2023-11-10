#include "scheduler.h"
#include "isr.h"
#include "frame_buffer.h"

cpu_state_t schedule(registers_t state) {
  fb_write("scheduling", 11);
  return state.cpu_registers;
}


void init_scheduler() 
{
  register_interrupt_handler(32, schedule);
}