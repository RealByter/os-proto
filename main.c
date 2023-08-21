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

#define MAXIMUM_CHANNELS    2
#define ATA_REG_CONTROL     0x0C

static uint8 ide_read_register(uint8 channel, uint8 reg);
static void ide_write_register(uint8 channel, uint8 reg, uint8 data);

typedef struct {
    uint16 base;  // i/o base port
    uint16 control;  // control port
    uint16 bm_ide; // bus-master ide port
    uint16 no_intr; // no interrupt port
} IDE_CHANNELS;

IDE_CHANNELS g_ide_channels[MAXIMUM_CHANNELS];

/**
 * read a byte from given port number
 */
uint8 inportb(uint16 port) {
    uint8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/**
 * write a given byte to given port number
 */
void outportb(uint16 port, uint8 val) {
    asm volatile("outb %1, %0" :: "dN"(port), "a"(val));
}

// read register value from the given channel
static uint8 ide_read_register(uint8 channel, uint8 reg) {
    uint8 ret;

    // write value ata-control to tell irq is ready
    if (reg > 0x07 && reg < 0x0C)
        ide_write_register(channel, ATA_REG_CONTROL, 0x80 | g_ide_channels[channel].no_intr);

    // read register from base channel port
    if (reg < 0x08)
        ret = inportb(g_ide_channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        ret = inportb(g_ide_channels[channel].base + reg - 0x06);
    else if (reg < 0x0E)
        ret = inportb(g_ide_channels[channel].control + reg - 0x0A);
    else if (reg < 0x16)
        ret = inportb(g_ide_channels[channel].bm_ide + reg - 0x0E);

    // write value to tell reading is done
    if (reg > 0x07 && reg < 0x0C)
        ide_write_register(channel, ATA_REG_CONTROL, g_ide_channels[channel].no_intr);

    return ret;
}

// write data to register to the given channel
static void ide_write_register(uint8 channel, uint8 reg, uint8 data) {
    // write value ata-control to tell irq is ready
    if (reg > 0x07 && reg < 0x0C)
        ide_write_register(channel, ATA_REG_CONTROL, 0x80 | g_ide_channels[channel].no_intr);

    // write data to register ports
    if (reg < 0x08)
        outportb(g_ide_channels[channel].base + reg - 0x00, data);
    else if (reg < 0x0C)
        outportb(g_ide_channels[channel].base + reg - 0x06, data);
    else if (reg < 0x0E)
        outportb(g_ide_channels[channel].control + reg - 0x0A, data);
    else if (reg < 0x16)
        outportb(g_ide_channels[channel].bm_ide + reg - 0x0E, data);

    // write value to tell reading is done
    if (reg > 0x07 && reg < 0x0C)
        ide_write_register(channel, ATA_REG_CONTROL, g_ide_channels[channel].no_intr);
}


int main()
{
  init_gdt();
  init_idt();
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  serial_write(SERIAL_COM1_BASE, "something", 9);
  clear_screen();
  
  uint8 data = 'a';
  uint8 reg = 0x00;
  uint8 channel = 0x00;

  ide_write_register(channel, reg, data);
  uint8 res = ide_read_register(channel, reg);

  fb_write_char(res);

  //char buf[BUFFER_LEN];
  //user_input(buf, BUFFER_LEN);
  //fb_write(buf, BUFFER_LEN);
  return 0;
}