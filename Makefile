TARGET    := myos

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
ISO_DIR   := $(BUILD_DIR)/iso

KERNEL_BIN := $(BUILD_DIR)/$(TARGET).bin
KERNEL_ISO := $(BUILD_DIR)/$(TARGET).iso

CC   := gcc
LD   := ld
NASM := nasm

CFLAGS  := -m32 -ffreestanding -O2 -Wall -Wextra \
           -fno-pic -fno-stack-protector -nostdlib -fno-builtin -g
LDFLAGS := -m elf_i386 -T linker.ld

# ============================================================
# Source files (여기에 새 파일 추가하면 자동 빌드에 포함됨)
# ============================================================
C_SRCS := \
  kernel/lib/itoa.c \
  kernel/kernel.c \
  kernel/memory/multiboot.c \
  kernel/memory/heap.c \
  kernel/panic/panic.c \
  kernel/console/kprintf.c \
  drivers/serial/serial.c \
  drivers/keyboard/keyboard.c \
  arch/x86/cpu/gdt.c \
  arch/x86/interrupt/idt.c \
  arch/x86/interrupt/isr.c \
  arch/x86/interrupt/pic.c \
  arch/x86/interrupt/irq.c \
  arch/x86/interrupt/pit.c

ASM_SRCS := \
  boot/entry.asm \
  arch/x86/cpu/gdt_flush.asm \
  arch/x86/interrupt/isr_stub.asm

# ============================================================
# Objects (obj/ 아래에 원본 경로를 그대로 미러링)
# 예) kernel/kernel.c -> build/obj/kernel/kernel.o
# ============================================================
C_OBJS   := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRCS))
ASM_OBJS := $(patsubst %.asm,$(OBJ_DIR)/%.o,$(ASM_SRCS))
OBJS     := $(C_OBJS) $(ASM_OBJS)

# ============================================================
# Directories
# ============================================================
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)/boot/grub

# obj 디렉토리는 파일별로 자동 생성 (rule에서 mkdir -p $(dir $@))

# ============================================================
# Top-level
# ============================================================
all: $(KERNEL_ISO)

# ============================================================
# Compile rules
# ============================================================
$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(NASM) -f elf32 $< -o $@

# ============================================================
# Link
# ============================================================
$(KERNEL_BIN): $(OBJS) linker.ld | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# ============================================================
# ISO
# ============================================================
$(KERNEL_ISO): $(KERNEL_BIN) | $(ISO_DIR)
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin

	echo 'set timeout=0'                 >  $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0'                 >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "My OS" {'           >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin'  >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  boot'                        >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}'                             >> $(ISO_DIR)/boot/grub/grub.cfg

	grub-mkrescue -o $@ $(ISO_DIR)

# ============================================================
# Run / Debug
# ============================================================
run: $(KERNEL_ISO)
	qemu-system-i386 -cdrom $< -no-reboot -serial stdio -d int,guest_errors -D $(BUILD_DIR)/qemu.log

debug: $(KERNEL_ISO)
	qemu-system-i386 -cdrom $< -serial stdio -no-reboot -s -S -d int,guest_errors -D $(BUILD_DIR)/qemu.log

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run debug clean
