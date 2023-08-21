#include "user_interface.h"

void user_input(char *buf, unsigned int max_length)
{
  char current_ascii_char = 0;
  unsigned int current_length = 0;
  while (current_ascii_char != '\n' && current_length < max_length - 1)
  {
    if (inb(0x64) & 0x01) // checks if there's new input
    {
      // outb(0x64, 0xc0); makes the keyboard instant but with warnings
      unsigned char scan_code = keyboard_read_scan_code();
      current_ascii_char = keyboard_scan_code_to_ascii(scan_code);
      if (current_ascii_char)
      {
        fb_write_char(current_ascii_char);
        if (current_ascii_char != '\n')
        {
          buf[current_length] = current_ascii_char;
          current_length++;
        }
      }
    }
  }
}