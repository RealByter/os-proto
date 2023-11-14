#ifndef INCLUDE_ISR_H
#define INCLUDE_ISR_H

#pragma once
#include "types.h"

#define PIC1_PORT_A 0x20
#define PIC2_PORT_A 0xA0

/* The PIC interrupts have been remapped */
#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT PIC2_START_INTERRUPT + 7

#define PIC_ACK 0x20

typedef struct cpu_state
{
  uint32 ds;
  uint32 edi;
  uint32 esi;
  uint32 ebp;
  uint32 esp;
  uint32 ebx;
  uint32 edx;
  uint32 ecx;
  uint32 eax;

  uint32 vector_number;
  uint32 error_code;

  uint32 iret_eip;
  uint32 iret_cs;
  uint32 iret_flags;
  uint32 iret_esp;
  uint32 iret_ss;
} cpu_state_t;


typedef void (*isr_t)(cpu_state_t*);

void register_interrupt_handler(uint8 n, isr_t handler);
void pic_acknowledge(unsigned int interrupt);
void interrupt_dispatch(cpu_state_t* context);

#endif /* INCLUDE_ISR_H */