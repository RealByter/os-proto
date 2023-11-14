#include "scheduler.h"
#include "isr.h"
#include "frame_buffer.h"
#include "heap.h"

process_t* head_process;
process_t* current_process;

void init_first_process(cpu_state_t* state) {
  process_t* process = malloc(sizeof(process_t));
  process->context = state;
  current_process = process;
  head_process = process;
}

cpu_state_t schedule(cpu_state_t state) {
  fb_write("scheduling", 11);
  // if(current_process == NULL)
  // {
  //   init_first_process(state);
  // }
  // else 
  // {
  //   process_t* old_process = current_process;
  //   current_process = current_process->next;
  //   if(current_process == NULL)
  //   {
  //     while(current_process == NULL)
  //     {
  //       if(head_process == DEAD)
  //       {
  //         process_t* dead_process = head_process;
  //         head_process = head_process->next;
  //         free(dead_process);
  //       }
  //       else
  //       {
  //         current_process = head_process;
  //       }
  //     }
  //   }
  // }
  // return current_process->context;
  return state;
}

void init_scheduler() 
{
  register_interrupt_handler(32, schedule);
}

void idle() {
  while(1)
    asm("hlt");
}

void process_execution_wrapper(void (*function)(void*), void* arg) {
  function(arg);
  current_process->status = DEAD;
  while(1);
}

process_t* create_process(void (*function)(void*), void* arg)
{
  process_t* process = malloc(sizeof(process_t));

  process->status = READY;
  process->context = malloc(sizeof(cpu_state_t));
  process->context->iret_ss = current_process->context->iret_ss;
  process->context->iret_esp = current_process->context->iret_esp;
  process->context->iret_flags = 0x202;
  process->context->iret_cs = current_process->context->iret_cs;
  process->context->iret_eip = (uint32)process_execution_wrapper;
  process->context->edi = (uint32)function;
  process->context->esi = (uint32)arg;
  process->context->ebp = current_process->context->ebp;

  process_t* iter_process = current_process;
  while(iter_process->next != NULL) 
  {
    if(iter_process->next->status == DEAD)
    {
      process_t* dead_process = iter_process->next;
      iter_process->next = dead_process->next;
      free(dead_process);
    }
    else
    {
      iter_process = iter_process->next;
    }
  }
  iter_process->next = process;
  
  return process;
}