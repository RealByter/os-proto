#include "io.h"

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

#define FB_GREEN 2
#define FB_BLACK 0

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
void fb_move_cursor(unsigned short pos);
void write(char* buf, unsigned int len);

int cursor = 0;

int main() 
{
  fb_write_cell(0, 'A', FB_GREEN, FB_BLACK);
  fb_move_cursor(80);
  fb_write_cell(2, 'B', FB_GREEN, FB_BLACK);
  char* buf = "hello";
  write(buf, 5);
  buf = " world";
  write(buf, 6);
  return 0;
}

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
  char* fb = (char*)0x000B8000;
  fb[i] = c;
  fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

void write(char* buf, unsigned int len)
{
  for(unsigned int i = 0; i < len; i++) 
  {
    fb_write_cell(cursor, buf[i], FB_GREEN, FB_BLACK);
    cursor += 2;
  }
}