#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#pragma once
#include "io.h"
#include "serial_port.h"


#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FB_GREEN 2
#define FB_BLACK 0

#define FB_BASE_ADDRESS 0xB8000


#define NUM_TO_ASCII 48
#define MAX_INT_SIZE 10


void outb(unsigned short port, unsigned char data);
void fb_move_cursor(unsigned short pos);
void clear_screen();
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
void fb_write(char *buf, unsigned int len);
void fb_write_char(char character);
void uint_to_str(char* buf, unsigned int number);

#endif 