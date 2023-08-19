#include "frame_buffer.h"
#include "serial_port.h"

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

#define FB_GREEN 2
#define FB_BLACK 0

int main() 
{
  serial_configure(SERIAL_COM1_BASE, Baud_57600);
  serial_write(SERIAL_COM1_BASE, "something", 9);
  char* buf = "hello";
  fb_write(buf, 5);
  buf = " world";
  fb_write(buf, 6);
  return 0;
}