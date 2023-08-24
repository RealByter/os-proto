#include "frame_buffer.h"
#include "serial_port.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "user_interface.h"
#include "isr.h"
#include "ide.h"

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
  ata_init();
  // ide_init(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
  char* write = "something";
  ide_write_sectors(1, 1, 0, (uint32)write);
  char read[10];
  ide_read_sectors(1, 1, 0, (uint32)read);
  
  
  // uint8 data = 'a';
  // uint8 reg = 0x00;
  // uint8 channel = 0x00;

  // ide_write_register(channel, reg, data);
  // uint8 res = ide_read_register(channel, reg);

  // fb_write_char(res);

  //char buf[BUFFER_LEN];
  //user_input(buf, BUFFER_LEN);
  //fb_write(buf, BUFFER_LEN);
  return 0;
}