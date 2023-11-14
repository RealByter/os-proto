OBJECTS = loader.o main.o io.o timer.o scheduler.o scheduler.o timer.o heap.o frame_buffer.o serial_port.o gdt_asm.o gdt.o isr_asm.o idt.o isr.o idt_asm.o keyboard.o user_interface.o string.o ide.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf
all: kernel.elf
kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf
os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	grub-mkrescue -o os.iso iso

run: os.iso
	# bochs -f bochsrc.txt -q
	qemu-system-x86_64 -boot d -d int -cdrom os.iso -m 512 -serial file:com1.out -drive format=raw,file=disk.img
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@
clean:
	rm -rf *.o kernel.elf os.iso