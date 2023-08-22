#include "frame_buffer.h"
#include "serial_port.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "user_interface.h"
#include "isr.h"

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FB_GREEN 2
#define FB_BLACK 0


#define BUFFER_LEN 64

int main()
{
  init_gdt();
  init_idt();
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  serial_write(SERIAL_COM1_BASE, "something", 9);
  clear_screen();
  
  char buf[BUFFER_LEN];
  user_input(buf, BUFFER_LEN);
  fb_write(buf, BUFFER_LEN);
  return 0;
}