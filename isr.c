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

void interrupt_dispatch(cpu_state_t* context)
{
  fb_write('here', 4);
  pic_acknowledge(32);
  // fb_write(context->vector_number + 48, 1);
  // if (interrupt_handlers[context->vector_number] != 0)
  // {
  //   isr_t handler = interrupt_handlers[context->vector_number];
  //   cpu_state_t* new_context = handler(context);
  //   fb_write(new_context->vector_number + 48, 1);
  //   return new_context;
  // }

}