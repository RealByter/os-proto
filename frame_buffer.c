#include "frame_buffer.h"

static char *fb = (char *)FB_BASE_ADDRESS;
static int cursor = 0;

void fb_move_cursor(unsigned short pos) {
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, pos & 0x00FF);
}


void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
  fb[2 * i] = c;
  fb[2 * i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

int fb_write(char *buf, unsigned int len) {
  unsigned int indexToBuffer = 0;
  while (indexToBuffer < len) {
    fb_write_cell(cursor + indexToBuffer, buf[indexToBuffer], FB_BLACK, FB_GREEN);
    fb_move_cursor(cursor + indexToBuffer);
    indexToBuffer++;
  }
  cursor += len;
  return 0;
}