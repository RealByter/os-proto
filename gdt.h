#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

#pragma once
#include "types.h"

/* This structure contains the value of one GDT entry.
 * We use the attribute 'packed' to tell GCC not to change
 * any of the alignment in the structure.
 */
struct gdt_entry_struct {
  uint16 limit_low;  // The lower 16 bits of the limit.
  uint16 base_low;   // The lower 16 bits of the base.
  uint8 base_middle; // The next 8 bits of the base.
  uint8
      access; // Access flags, determine what ring this segment can be used in.
  uint8 granularity;
  uint8 base_high; // The last 8 bits of the base.
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

/* This struct describes a GDT pointer. It points to the start of
 * our array of GDT entries, and is in the format required by the
 * lgdt instruction.
 */
struct gdt_ptr_struct {
  uint16 limit; // The upper 16 bits of table with entries.
  uint32 base;  // The address of the first gdt_entry_t struct.
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

/* Function to initialize GDT */
void init_gdt();

#endif /* INCLUDE_GDT_H */