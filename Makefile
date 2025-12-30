PROJECT = firmware

SOURCES = Core/Src/main.c \
		  Core/Src/usart.c \
		  Core/Src/gpio.c \
		  Core/Src/pins.c \
          Core/Src/syscalls.c \
		  Core/Src/utils.c \
		  Core/Src/tim.c \
		  Core/Src/hcsr04.c \
          Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c \
          Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s

INCLUDES = \
  -ICore/Inc \
  -IDrivers/CMSIS/Core/CMSIS/Core/Include \
  -IDrivers/CMSIS/Device/ST/STM32F4xx/Include

# Compiler flags
CFLAGS  = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
          -g3 -Os -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
          -ffunction-sections -fdata-sections $(INCLUDES) \
          -DSTM32F407xx

CFLAGS += -u _printf_float
CFLAGS += -Wno-double-promotion
CFLAGS += --sysroot=/usr/local/arm-gnu-toolchain-14.3.rel1-darwin-arm64-arm-none-eabi
CFLAGS += -isystem /usr/local/arm-gnu-toolchain-14.3.rel1-darwin-arm64-arm-none-eabi/include

# Linker flags
LDFLAGS = -TSTM32F407VGTX_FLASH.ld \
          -nostartfiles \
          --specs=rdimon.specs -lc -lrdimon -lgcc \
          -Wl,--gc-sections \
          -Wl,-Map=$(PROJECT).map \
          -Wl,-eReset_Handler

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Build ELF using the linker script (handles both FLASH and RAM)
$(PROJECT).elf: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $@

# Convert ELF to binary for flashing
$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

# Flash to STM32
flash: $(PROJECT).bin
	st-flash --reset write $(PROJECT).bin 0x08000000

# Clean build artifacts
clean:
	rm -f $(PROJECT).elf $(PROJECT).bin $(PROJECT).map
