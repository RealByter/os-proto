#include "scheduler.h"
#include "frame_buffer.h"
#include "heap.h";

process_t* processes_list;
process_t* current_process;

void schedule(cpu_state_t* context)
{
  // process_t* prempted_process;
  // current_process->context = *context;
  // current_process->status = READY;

  // while() 
  // {
  //   process_t* prev_process = current_process;
  //   if(current_process->next != NULL) 
  //   {
  //     current_process = current_process->next;
  //   }
  //   else 
  //   {
  //     current_process = processes_list;
  //   }

  //   if(current_process != NULL && current_process->STATUS == DEAD)
  //   {
  //     if(prev_process != NULL)
  //     {
  //       prev_process->next = current_process->next;
  //     }
  //     else 
  //     {
  //       processes_list = current_process->next;
  //     }

  //     free(current_process);
  //   }
  //   else 
  //   {
  //     current_process->status = RUNNING;
  //     break;
  //   }
  // }

  // *context = current_process->context;
}

process_t* create_process(void (*function)(void*), void* arg)
{
  process_t* process = malloc(sizeof(process_t));

  process->status = READY;
  process->context.iret_ss = current_process->context.iret_ss;
  process->context.iret_esp = current_process->context.esp;
  process->context.iret_flags = 0x202;
  process->context.iret_cs = current_process->context.iret_cs;
  // process->context.iret_eip
  // process->context
}