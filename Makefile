TARGET  := myos
ISO_DIR := iso
ISO     := $(TARGET).iso

CC   := gcc
LD   := ld
NASM := nasm

CFLAGS  := -m32 -ffreestanding -O2 -Wall -Wextra \
           -fno-pic -fno-stack-protector -nostdlib -fno-builtin
LDFLAGS := -m elf_i386 -T linker.ld

OBJS := boot/entry.o kernel/kernel.o kernel/vga.o

all: $(ISO)

boot/entry.o: boot/entry.asm
	$(NASM) -f elf32 $< -o $@

kernel/kernel.o: kernel/kernel.c kernel/vga.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/vga.o: kernel/vga.c kernel/vga.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(ISO): $(TARGET).bin
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(TARGET).bin $(ISO_DIR)/boot/kernel.bin

	echo 'set timeout=0'                >  $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0'                >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "My OS" {'          >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin' >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  boot'                       >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}'                            >> $(ISO_DIR)/boot/grub/grub.cfg

	grub-mkrescue -o $(ISO) $(ISO_DIR)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(OBJS) $(TARGET).bin $(ISO_DIR) $(ISO)

.PHONY: all run clean