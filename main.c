#include "frame_buffer.h"
#include "serial_port.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "isr.h"

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FB_GREEN 2
#define FB_BLACK 0

int main()
{
  init_gdt();
  init_idt();
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  serial_write(SERIAL_COM1_BASE, "something", 9);
  char *buf = "hello";
  fb_write(buf, 5);
  buf = " world";
  fb_write(buf, 6);

  while (1)
  {
    if (inb(0x64) & 0x01)
    {
      // outb(0x64, 0xc0); makes the keyboard instant but with warnings
      unsigned char scan_code = keyboard_read_scan_code();
      char ascii_char = keyboard_scan_code_to_ascii(scan_code);
      if (ascii_char)
      {
        char str[1];
        str[0] = ascii_char;
        fb_write(str, 1);
      }
    }
  }
  return 0;
}