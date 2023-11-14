#include "isr.h"
#include "io.h"
#include "frame_buffer.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8 n, isr_t handler)
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

void interrupt_handler(cpu_state_t* context)
{
  pic_acknowledge(context->vector_number); 
  fb_write("here", 4);
  char printed_vector[1];
  printed_vector[0] = context->vector_number + 33;
  fb_write(printed_vector, 1);

  if (interrupt_handlers[context->vector_number] != 0)
  {
    fb_write("here1", 5);
    isr_t handler = interrupt_handlers[context->vector_number];
    handler(context);
  }
}