#include "isr.h"
#include "io.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(u8int n, isr_t handler)
{
  interrupt_handlers[n] = handler;
}

void pic_acknowledge(unsigned int interrupt)
{
  if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT)
  {
    return;
  }

  if (interrupt < PIC2_START_INTERRUPT)
  {
    outb(PIC1_PORT_A, PIC_ACK);
  }
  else
  {
    outb(PIC2_PORT_A, PIC_ACK);
  }
}

void interrupt_handler(registers_t regs)
{
  pic_acknowledge(regs.stack_contents.int_no);

  if (interrupt_handlers[regs.stack_contents.int_no] != 0)
  {
    isr_t handler = interrupt_handlers[regs.stack_contents.int_no];
    handler(regs);
  }
}