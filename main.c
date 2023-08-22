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
#define MAXIMUM_IDE_DEVICES     5


#define ATA_REG_CONTROL     0x0C

// ATA register ports for read/write
#define ATA_REG_DATA         0x00
#define ATA_REG_ERROR        0x01
#define ATA_REG_FEATURES     0x01
#define ATA_REG_SECCOUNT0    0x02
#define ATA_REG_LBA0         0x03
#define ATA_REG_LBA1         0x04
#define ATA_REG_LBA2         0x05
#define ATA_REG_HDDEVSEL     0x06
#define ATA_REG_COMMAND      0x07
#define ATA_REG_STATUS       0x07
#define ATA_REG_SECCOUNT1    0x08
#define ATA_REG_LBA3         0x09
#define ATA_REG_LBA4         0x0A
#define ATA_REG_LBA5         0x0B
#define ATA_REG_CONTROL      0x0C
#define ATA_REG_ALTSTATUS    0x0C
#define ATA_REG_DEVADDRESS   0x0D

// ATA drive status
#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

// ATA drive error status
#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark


// Channels
#define ATA_PRIMARY      0x00
#define ATA_SECONDARY    0x01

// IDE types
#define IDE_ATA      0x00
#define IDE_ATAPI    0x01

// Command types
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

// Identify types
#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200


#define ATA_SECTOR_SIZE    512

// Directions
#define ATA_READ     0x00
#define ATA_WRITE    0x01

// LBA(Linear Block Address) modes
#define LBA_MODE_48   0x02
#define LBA_MODE_28   0x01

static uint8 ide_read_register(uint8 channel, uint8 reg);
static void ide_write_register(uint8 channel, uint8 reg, uint8 data);

typedef struct {
    uint16 base;  // i/o base port
    uint16 control;  // control port
    uint16 bm_ide; // bus-master ide port
    uint16 no_intr; // no interrupt port
} IDE_CHANNELS;

typedef struct {
    uint8 reserved; // 0 or 1 if drive exists or not
    uint8 channel; // primary(0) or secondary(1)
    uint8 drive; // master(0) or slave(1)
    uint16 type; // drive type- ATA(0), ATAPI(1),
    uint16 signature; // drive signature
    uint16 features; // drive features
    uint32 command_sets; // supported command sets
    uint32 size; // drive size in sectors
    unsigned char model[41]; // drive name
} IDE_DEVICE;

IDE_CHANNELS g_ide_channels[MAXIMUM_CHANNELS];
IDE_DEVICE g_ide_devices[MAXIMUM_IDE_DEVICES];

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

void ide_init(uint32 prim_channel_base_addr, uint32 prim_channel_control_base_addr,
              uint32 sec_channel_base_addr, uint32 sec_channel_control_addr,
              uint32 bus_master_addr) {
    int i, j, k, count = 0;
    unsigned char ide_buf[2048] = {0};

    // 1- Detect I/O Ports which interface IDE Controller:
    // (checking the addr is removed for simplicity, just assigning all ports)
    g_ide_channels[ATA_PRIMARY].base = prim_channel_base_addr;
    g_ide_channels[ATA_PRIMARY].control = prim_channel_control_base_addr;
    g_ide_channels[ATA_SECONDARY].base = sec_channel_base_addr;
    g_ide_channels[ATA_SECONDARY].control = sec_channel_control_addr;
    g_ide_channels[ATA_PRIMARY].bm_ide = bus_master_addr;
    g_ide_channels[ATA_SECONDARY].bm_ide = bus_master_addr;

    // 2- Disable IRQs:
    ide_write_register(ATA_PRIMARY, ATA_REG_CONTROL, 2);
    ide_write_register(ATA_SECONDARY, ATA_REG_CONTROL, 2);

    // 3- Detect ATA-ATAPI Devices:
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            uint8 err = 0, type = IDE_ATA, status;
            g_ide_devices[count].reserved = 0;  // Assuming that no drive here.

            // (I) Select Drive:
            ide_write_register(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4));  // Select Drive.
            //sleep(1); // Wait 1ms for drive select to work.

            // (II) Send ATA Identify Command:
            ide_write_register(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            //sleep(1); // This function should be implemented in your OS. which waits for 1 ms.
            // it is based on System Timer Device Driver.

            // (III) Polling:
            if (ide_read_register(i, ATA_REG_STATUS) == 0) continue;  // If Status = 0, No Device.

            while (1) {
                status = ide_read_register(i, ATA_REG_STATUS);
                if ((status & ATA_SR_ERR)) {
                    err = 1;
                    break;
                }                                                            // If Err, Device is not ATA.
                if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;  // Everything is right.
            }

            // (IV) Probe for ATAPI Devices:

            if (err != 0) {
                unsigned char cl = ide_read_register(i, ATA_REG_LBA1);
                unsigned char ch = ide_read_register(i, ATA_REG_LBA2);

                if (cl == 0x14 && ch == 0xEB)
                    type = IDE_ATAPI;
                else if (cl == 0x69 && ch == 0x96)
                    type = IDE_ATAPI;
                else
                    continue;  // Unknown Type (may not be a device).

                ide_write_register(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
                //sleep(1);
            }

            // (V) Read Identification Space of the Device:
            // ide_read_buffer(i, ATA_REG_DATA, (unsigned int *)ide_buf, 128);

            // (VI) Read Device Parameters:
            g_ide_devices[count].reserved = 1;
            g_ide_devices[count].type = type;
            g_ide_devices[count].channel = i;
            g_ide_devices[count].drive = j;
            g_ide_devices[count].signature = *((unsigned short *)(ide_buf + ATA_IDENT_DEVICETYPE));
            g_ide_devices[count].features = *((unsigned short *)(ide_buf + ATA_IDENT_CAPABILITIES));
            g_ide_devices[count].command_sets = *((unsigned int *)(ide_buf + ATA_IDENT_COMMANDSETS));

            // (VII) Get Size:
            if (g_ide_devices[count].command_sets & (1 << 26))
                // Device uses 48-Bit Addressing:
                g_ide_devices[count].size = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
            else
                // Device uses CHS or 28-bit Addressing:
                g_ide_devices[count].size = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA));

            // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
            for (k = 0; k < 40; k += 2) {
                g_ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
                g_ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
            }
            g_ide_devices[count].model[40] = '\0';  // Terminate String.
            // remove trailing spaces in model string
            for(k = 39; k >= 0; k--) {
                char ch = g_ide_devices[count].model[k];
                if(ch == ' ')
                    g_ide_devices[count].model[k] = '\0';
                else
                    break;
            }

            count++;
        }
    }
}


int main()
{
  init_gdt();
  init_idt();
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
  serial_write(SERIAL_COM1_BASE, "something", 9);
  clear_screen();
  ide_init(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
  
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