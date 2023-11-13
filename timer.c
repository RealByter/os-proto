#include "timer.h"
#include "io.h"
#include "isr.h"
#include "scheduler.h"

void init_timer(uint32 frequency)
{
  register_interrupt_handler(32, schedule);

  uint32 divisor = 1193180 / frequency;

  outb(0x43, 0x36);

  // uint8 ls_bits = (uint8)(divisor & 0xFF);
  // uint8 ms_bits = (uint8)((divisor >> 8) & 0xFF);
  uint8 ls_bits = (uint8)(23864 & 0xFF);
  uint8 ms_bits = (uint8)((23864 >> 8) & 0xFF);

  outb(0x40, ls_bits);
  outb(0x40, ms_bits);

  asm volatile("sti");
}