#include "isr.h"
#include "io.h"

isr_t interrupt_handlers[256];


void register_interrupt_handler(u8int n, isr_t handler) {
  interrupt_handlers[n] = handler;
}


void interrupt_handler(registers_t regs) {
  
  if (regs.stack_contents.int_no >= 40) {
    outb(0xA0, 0x20);
  }
 
  if (regs.stack_contents.int_no >= 32) {
    outb(0x20, 0x20);
  }
 
  if (interrupt_handlers[regs.stack_contents.int_no] != 0) {
    isr_t handler = interrupt_handlers[regs.stack_contents.int_no];
    handler(regs);
  }
}