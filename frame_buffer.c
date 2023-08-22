#include "frame_buffer.h"

#define FRAME_BUFFER_SIZE 80 * 25

static char *fb = (char *)FB_BASE_ADDRESS;
static int cursor = 0;

void fb_move_cursor(unsigned short pos)
{
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, pos & 0x00FF);
}

void clear_screen()
{
  for(int i = 0; i <= FRAME_BUFFER_SIZE; i++)
  {
    fb_write_cell(i, 0, FB_BLACK, FB_GREEN);
  }
}

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
  fb[2 * i] = c;
  fb[2 * i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void fb_write(char *buf, unsigned int len)
{
  for(unsigned int i = 0; i < len && buf[i] != 0; i++)
  {
    fb_write_char(buf[i]);
  }
}

void fb_write_char(char character)
{
  if (character != '\n')
  {
    cursor++;
    if(cursor > FRAME_BUFFER_SIZE)
    {
      clear_screen();
      cursor = 1;
    }
    fb_write_cell(cursor - 1, character, FB_BLACK, FB_GREEN);
    fb_move_cursor(cursor);
  }
  else 
  {
    cursor = ((unsigned int)(cursor / 80) + 1) * 80;
    if(cursor > FRAME_BUFFER_SIZE)
    {
      clear_screen();
      cursor = 0;
    }
    fb_move_cursor(cursor); // a length of a vga line is 80 
  }
}