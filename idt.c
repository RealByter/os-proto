#include "idt.h"
#include "io.h"

extern void idt_flush(uint32);

idt_entry_t idt_entries[256];

void init_pic()
{
  outb(PIC1, INITIALISE_COMMAND);
  outb(PIC2, INITIALISE_COMMAND);

  outb(PIC1_DATA, PIC1_VECTOR_OFFSET);
  outb(PIC2_DATA, PIC2_VECTOR_OFFSET);

  outb(PIC1_DATA, PIC2_PORT_IN_PIC1);

  outb(PIC2_DATA, PIC2_CASCADED_IDEN);

  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);

  outb(PIC1_DATA, 0x0);
  outb(PIC2_DATA, 0x0);
}

static void idt_set_gate(uint8 num, uint32 addressISR, uint16 segmentSelector,
                         uint8 accessGran)
{
  idt_entries[num].offset_low = (addressISR & 0xFFFF);
  idt_entries[num].segment_selector = segmentSelector;
  idt_entries[num].alwaysZero = 0;

  idt_entries[num].access_gran = accessGran;

  idt_entries[num].offset_high = addressISR >> 16;
}

void init_idt()
{
  idt_ptr_t idt_ptr;
  idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
  idt_ptr.base = (uint32)&idt_entries;

  init_pic();

  idt_set_gate(0, (uint32)isr0, 0x08, 0x8E);   // ISR 0
  idt_set_gate(1, (uint32)isr1, 0x08, 0x8E);   // ISR 1
  idt_set_gate(2, (uint32)isr2, 0x08, 0x8E);   // ISR 2
  idt_set_gate(3, (uint32)isr3, 0x08, 0x8E);   // ISR 3
  idt_set_gate(4, (uint32)isr4, 0x08, 0x8E);   // ISR 4
  idt_set_gate(5, (uint32)isr5, 0x08, 0x8E);   // ISR 5
  idt_set_gate(6, (uint32)isr6, 0x08, 0x8E);   // ISR 6
  idt_set_gate(7, (uint32)isr7, 0x08, 0x8E);   // ISR 7
  idt_set_gate(8, (uint32)isr8, 0x08, 0x8E);   // ISR 8
  idt_set_gate(9, (uint32)isr9, 0x08, 0x8E);   // ISR 9
  idt_set_gate(10, (uint32)isr10, 0x08, 0x8E); // ISR 10
  idt_set_gate(11, (uint32)isr11, 0x08, 0x8E); // ISR 11
  idt_set_gate(12, (uint32)isr12, 0x08, 0x8E); // ISR 12
  idt_set_gate(13, (uint32)isr13, 0x08, 0x8E); // ISR 13
  idt_set_gate(14, (uint32)isr14, 0x08, 0x8E); // ISR 14
  idt_set_gate(15, (uint32)isr15, 0x08, 0x8E); // ISR 15
  idt_set_gate(16, (uint32)isr16, 0x08, 0x8E); // ISR 16
  idt_set_gate(17, (uint32)isr17, 0x08, 0x8E); // ISR 17
  idt_set_gate(18, (uint32)isr18, 0x08, 0x8E); // ISR 18
  idt_set_gate(19, (uint32)isr19, 0x08, 0x8E); // ISR 19
  idt_set_gate(20, (uint32)isr20, 0x08, 0x8E); // ISR 20
  idt_set_gate(21, (uint32)isr21, 0x08, 0x8E); // ISR 21
  idt_set_gate(22, (uint32)isr22, 0x08, 0x8E); // ISR 22
  idt_set_gate(23, (uint32)isr23, 0x08, 0x8E); // ISR 23
  idt_set_gate(24, (uint32)isr24, 0x08, 0x8E); // ISR 24
  idt_set_gate(25, (uint32)isr25, 0x08, 0x8E); // ISR 25
  idt_set_gate(26, (uint32)isr26, 0x08, 0x8E); // ISR 26
  idt_set_gate(27, (uint32)isr27, 0x08, 0x8E); // ISR 27
  idt_set_gate(28, (uint32)isr28, 0x08, 0x8E); // ISR 28
  idt_set_gate(29, (uint32)isr29, 0x08, 0x8E); // ISR 29
  idt_set_gate(30, (uint32)isr30, 0x08, 0x8E); // ISR 30
  idt_set_gate(31, (uint32)isr31, 0x08, 0x8E); // ISR 31

  idt_set_gate(32, (uint32)irq0, 0x08, 0x8E);  // IRQ 0
  idt_set_gate(33, (uint32)irq1, 0x08, 0x8E);  // IRQ 1
  idt_set_gate(34, (uint32)irq2, 0x08, 0x8E);  // IRQ 2
  idt_set_gate(35, (uint32)irq3, 0x08, 0x8E);  // IRQ 3
  idt_set_gate(36, (uint32)irq4, 0x08, 0x8E);  // IRQ 4
  idt_set_gate(37, (uint32)irq5, 0x08, 0x8E);  // IRQ 5
  idt_set_gate(38, (uint32)irq6, 0x08, 0x8E);  // IRQ 6
  idt_set_gate(39, (uint32)irq7, 0x08, 0x8E);  // IRQ 7
  idt_set_gate(40, (uint32)irq8, 0x08, 0x8E);  // IRQ 8
  idt_set_gate(41, (uint32)irq9, 0x08, 0x8E);  // IRQ 9
  idt_set_gate(42, (uint32)irq10, 0x08, 0x8E); // IRQ 10
  idt_set_gate(43, (uint32)irq11, 0x08, 0x8E); // IRQ 11
  idt_set_gate(44, (uint32)irq12, 0x08, 0x8E); // IRQ 12
  idt_set_gate(45, (uint32)irq13, 0x08, 0x8E); // IRQ 13
  idt_set_gate(46, (uint32)irq14, 0x08, 0x8E); // IRQ 14
  idt_set_gate(47, (uint32)irq15, 0x08, 0x8E); // IRQ 15

  idt_flush((uint32)&idt_ptr);
}