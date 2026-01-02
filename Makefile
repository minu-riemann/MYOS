TARGET    := myos

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
ISO_DIR   := $(BUILD_DIR)/iso

KERNEL_BIN := $(BUILD_DIR)/$(TARGET).bin
KERNEL_ISO := $(BUILD_DIR)/$(TARGET).iso

# ============================================================
# 아키텍처 선택
# ============================================================
ARCH ?= x86

ifeq ($(ARCH),x86)
    CC   := gcc
    LD   := ld
    AS   := nasm
    
    CFLAGS  := -m32 -ffreestanding -O2 -Wall -Wextra \
               -fno-pic -fno-stack-protector -nostdlib -fno-builtin -g -I include
    ASFLAGS := -f elf32
    LDFLAGS := -m elf_i386 -T linker/x86.ld
    
    ARCH_DIR := arch/x86
    BOOT_SRC := boot/x86/entry.asm
    DEFINES := -DARCH_X86
    
else ifeq ($(ARCH),arm)
    CC   := arm-none-eabi-gcc
    LD   := arm-none-eabi-ld
    AS   := arm-none-eabi-as
    
    CFLAGS  := -mcpu=cortex-a9 -ffreestanding -O2 -Wall -Wextra \
               -fno-pic -fno-stack-protector -nostdlib -fno-builtin -g -I include
    ASFLAGS := -mcpu=cortex-a9
    LDFLAGS := -T linker/arm.ld
    
    ARCH_DIR := arch/arm
    BOOT_SRC := boot/arm/entry.S
    DEFINES := -DARCH_ARM
    
else
    $(error Unknown ARCH: $(ARCH). Use ARCH=x86 or ARCH=arm)
endif

CFLAGS += $(DEFINES)

# ============================================================
# Source files
# ============================================================
# 공통 커널 소스 (아키텍처 독립적)
COMMON_SRCS := \
  kernel/lib/itoa.c \
  kernel/kernel.c \
  kernel/memory/multiboot.c \
  kernel/memory/heap.c \
  kernel/panic/panic.c \
  kernel/console/kprintf.c \
  kernel/time/time.c \
  drivers/serial/serial.c \
  drivers/keyboard/keyboard.c

# 아키텍처별 소스 (자동 수집)
ARCH_SRCS := $(wildcard $(ARCH_DIR)/**/*.c)

# 모든 C 소스
C_SRCS := $(COMMON_SRCS) $(ARCH_SRCS)

# 아키텍처별 어셈블리 소스
ifeq ($(ARCH),x86)
    ASM_SRCS := \
      boot/x86/entry.asm \
      $(wildcard $(ARCH_DIR)/**/*.asm)
else ifeq ($(ARCH),arm)
    ASM_SRCS := \
      boot/arm/entry.S \
      $(wildcard $(ARCH_DIR)/**/*.S)
endif

# ============================================================
# Objects
# ============================================================
C_OBJS   := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SRCS))
ASM_OBJS := $(patsubst %.asm,$(OBJ_DIR)/%.o,$(patsubst %.S,$(OBJ_DIR)/%.o,$(ASM_SRCS)))
OBJS     := $(C_OBJS) $(ASM_OBJS)

# ============================================================
# Directories
# ============================================================
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)/boot/grub

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
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# ============================================================
# Link
# ============================================================
$(KERNEL_BIN): $(OBJS) | $(BUILD_DIR)
ifeq ($(ARCH),x86)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
else ifeq ($(ARCH),arm)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
endif

# ============================================================
# ISO (x86 전용)
# ============================================================
$(KERNEL_ISO): $(KERNEL_BIN) | $(ISO_DIR)
ifeq ($(ARCH),x86)
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin

	echo 'set timeout=0'                 >  $(ISO_DIR)/boot/grub/grub.cfg
	echo 'set default=0'                 >> $(ISO_DIR)/boot/grub/grub.cfg
	echo 'menuentry "My OS" {'           >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin'  >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '  boot'                        >> $(ISO_DIR)/boot/grub/grub.cfg
	echo '}'                             >> $(ISO_DIR)/boot/grub/grub.cfg

	grub-mkrescue -o $@ $(ISO_DIR)
else
	@echo "ISO generation is only supported for x86"
	@false
endif

# ============================================================
# Run / Debug
# ============================================================
ifeq ($(ARCH),x86)
run: $(KERNEL_ISO)
	qemu-system-i386 -cdrom $(KERNEL_ISO) -no-reboot -serial stdio -d int,guest_errors -D $(BUILD_DIR)/qemu.log

debug: $(KERNEL_ISO)
	qemu-system-i386 -cdrom $(KERNEL_ISO) -serial stdio -no-reboot -s -S -d int,guest_errors -D $(BUILD_DIR)/qemu.log
else ifeq ($(ARCH),arm)
run: $(KERNEL_BIN)
	qemu-system-arm -M versatilepb -m 128M -kernel $(KERNEL_BIN) -serial stdio -nographic -no-reboot

debug: $(KERNEL_BIN)
	qemu-system-arm -M versatilepb -m 128M -kernel $(KERNEL_BIN) -serial stdio -nographic -no-reboot -s -S
endif

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run debug clean