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
  uint32 edi;
  uint32 esi;
  uint32 ebp;
  uint32 esp;
  uint32 ebx;
  uint32 edx;
  uint32 ecx;
  uint32 eax;
} cpu_state_t;

typedef struct stack_state
{
  uint32 int_no;
  uint32 err_code;
  uint32 eip;
  uint32 cs;
  uint32 eflags;
  uint32 useresp;
  uint32 ss;
} stack_state_t;

typedef struct registers
{
  /* Data segment selector */
  uint32 ds;
  /* Pushed by pusha */
  cpu_state_t cpu_registers;
  /* Interrupt number and error code (if applicable). Pushed by the processor
   * automatically.
   */
  stack_state_t stack_contents;
} registers_t;

typedef cpu_state_t (*isr_t)(registers_t);

void register_interrupt_handler(uint8 n, isr_t handler);
void pic_acknowledge(unsigned int interrupt);
void interrupt_handler(registers_t regs);

#endif /* INCLUDE_ISR_H */