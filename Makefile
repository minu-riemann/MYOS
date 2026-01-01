TARGET  := myos

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
ISO_DIR := $(BUILD_DIR)/iso
ISO := $(BUILD_DIR)/$(TARGET).iso

CC   := gcc
LD   := ld
NASM := nasm

CFLAGS  := -m32 -ffreestanding -O2 -Wall -Wextra \
           -fno-pic -fno-stack-protector -nostdlib -fno-builtin
LDFLAGS := -m elf_i386 -T linker.ld

OBJS := \
  $(OBJ_DIR)/entry.o \
  $(OBJ_DIR)/kernel.o \
  $(OBJ_DIR)/vga.o \
  $(OBJ_DIR)/serial.o \
  $(OBJ_DIR)/panic.o \
  $(OBJ_DIR)/gdt.o \
  $(OBJ_DIR)/gdt_flush.o \
  $(OBJ_DIR)/idt.o \
  $(OBJ_DIR)/isr.o \
  $(OBJ_DIR)/isr_stub.o \
 $(OBJ_DIR)/pic.o \
  $(OBJ_DIR)/pit.o \
  $(OBJ_DIR)/irq.o \

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)/boot/grub

all: $(BUILD_DIR)/$(TARGET).iso

$(OBJ_DIR)/entry.o: boot/entry.asm | $(OBJ_DIR)
	$(NASM) -f elf32 $< -o $@

$(OBJ_DIR)/kernel.o: kernel/kernel.c kernel/vga.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/vga.o: kernel/vga.c kernel/vga.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/serial.o: drivers/serial/serial.c drivers/serial/serial.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/panic.o: kernel/panic/panic.c kernel/panic/panic.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/gdt.o: arch/x86/cpu/gdt.c arch/x86/cpu/gdt.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/gdt_flush.o: arch/x86/cpu/gdt_flush.asm | $(OBJ_DIR)
	$(NASM) -f elf32 $< -o $@

$(OBJ_DIR)/idt.o: arch/x86/interrupt/idt.c arch/x86/interrupt/idt.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/isr.o: arch/x86/interrupt/isr.c arch/x86/interrupt/isr.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/pic.o: arch/x86/interrupt/pic.c arch/x86/interrupt/pic.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/pit.o: arch/x86/interrupt/pit.c arch/x86/interrupt/pit.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/irq.o: arch/x86/interrupt/irq.c arch/x86/interrupt/irq.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJ_DIR)/isr_stub.o: arch/x86/interrupt/isr_stub.asm | $(OBJ_DIR)
	$(NASM) -f elf32 $< -o $@

$(BUILD_DIR)/$(TARGET).bin: $(OBJS) linker.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(TARGET).iso: $(BUILD_DIR)/$(TARGET).bin | $(ISO_DIR)
	cp $(BUILD_DIR)/$(TARGET).bin $(ISO_DIR)/boot/kernel.bin

	echo 'set timeout=0'                 >  $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0'                 >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "My OS" {'           >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin'  >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  boot'                        >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}'                             >> $(ISO_DIR)/boot/grub/grub.cfg

	grub-mkrescue -o $@ $(ISO_DIR)


run: $(BUILD_DIR)/$(TARGET).iso
	qemu-system-i386 -cdrom $< -serial stdio -no-reboot -d int,guest_errors -D build/qemu.log

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean