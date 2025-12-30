# Project name
PROJECT := firmware

# Directories
FW_DIR      := Firmware
CORE_DIR    := $(FW_DIR)/Core
DRIVERS_DIR := $(FW_DIR)/Drivers
BUILD_DIR   := $(FW_DIR)/build

# Toolchain
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

# MCU flags
CPU     := -mcpu=cortex-m4
FPU     := -mfpu=fpv4-sp-d16
FLOAT   := -mfloat-abi=hard
MCU     := $(CPU) -mthumb $(FPU) $(FLOAT)

# Sources
C_SOURCES := \
$(CORE_DIR)/Src/main.c \
$(CORE_DIR)/Src/gpio.c \
$(CORE_DIR)/Src/usart.c \
$(CORE_DIR)/Src/pins.c \
$(CORE_DIR)/Src/syscalls.c \
$(CORE_DIR)/Src/utils.c \
$(CORE_DIR)/Src/tim.c \
$(CORE_DIR)/Src/hcsr04.c \
$(DRIVERS_DIR)/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c

ASM_SOURCES := \
$(DRIVERS_DIR)/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s

# Includes
INCLUDES := \
-I$(CORE_DIR)/Inc \
-I$(DRIVERS_DIR)/CMSIS/Core/CMSIS/Core/Include \
-I$(DRIVERS_DIR)/CMSIS/Device/ST/STM32F4xx/Include

# Output objects
OBJECTS := \
$(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
$(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))

# Compiler flags
CFLAGS := $(MCU) \
-g3 -Os \
-Wall -Wextra -Wundef -Wshadow \
-ffunction-sections -fdata-sections \
$(INCLUDES) \
-DSTM32F407xx

# Linker script
LDSCRIPT := $(FW_DIR)/STM32F407VGTX_FLASH.ld

# Linker flags
LDFLAGS := $(MCU) \
-T$(LDSCRIPT) \
-Wl,--gc-sections \
-Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
-nostartfiles \
-Wl,-eReset_Handler

# Default target
all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile ASM files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(MCU) -c $< -o $@

# Link ELF
$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

# Create binary
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

# Flash
flash: $(BUILD_DIR)/$(PROJECT).bin
	st-flash --reset write $< 0x08000000

# Clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean flash
