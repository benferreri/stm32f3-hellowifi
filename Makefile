TARGET = main

# define linker script location and chip arch
LD_SCRIPT = ./linker/stm32f334r8t6.ld
MCU_SPEC = cortex-m4

# toolchain definitions (ARM bare metal defaults)
TOOLCHAIN = /usr
CC = $(TOOLCHAIN)/bin/arm-none-eabi-gcc
AS = $(TOOLCHAIN)/bin/arm-none-eabi-as
LD = $(TOOLCHAIN)/bin/arm-none-eabi-ld
OC = $(TOOLCHAIN)/bin/arm-none-eabi-objcopy
OD = $(TOOLCHAIN)/bin/arm-none-eabi-objdump
OS = $(TOOLCHAIN)/bin/arm-none-eabi-size

# assembly directives
ASFLAGS += -c
ASFLAGS += -O0
ASFLAGS += -mcpu=$(MCU_SPEC)
ASFLAGS += -mthumb
ASFLAGS += -Wall
# (set error messages to appear on a single line)
ASFLAGS += -fmessage-length=0

# C compilation directives
CFLAGS += -mcpu=$(MCU_SPEC)
CFLAGS += -mthumb
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -g
# (set error messages to appear on a single line)
CFLAGS += -fmessage-length=0
# (set system to ignore semihosted junk)
CFLAGS += --specs=nosys.specs

# linker directives
LSCRIPT = $(LD_SCRIPT)
LFLAGS += -mcpu=$(MCU_SPEC)
LFLAGS += -mthumb
LFLAGS += -Wall
LFLAGS += --specs=nosys.specs
LFLAGS += -nostdlib
LFLAGS += -lgcc
LFLAGS += -T$(LSCRIPT)

STARTUP  =  ./boot/startup_stm32f334x8.s
C_SRC    =  ./device_headers/system_stm32f3xx.c
C_SRC 	 += ./src/main.c
C_SRC    += ./src/uart.c
C_SRC    += ./src/clock.c

INCLUDE =  -I./
INCLUDE += -I./device_headers

OBJS =  $(STARTUP:.s=.o)
OBJS += $(C_SRC:.c=.o)

.PHONY: all
all: $(TARGET).bin
%.o: %.s
	$(CC) -x assembler-with-cpp $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@
    
$(TARGET).elf: $(OBJS)
	$(CC) $^ $(LFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OC) -S -O binary $< $@
	$(OS) $<

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(TARGET).elf

.PHONY: flash
flash:
	st-flash write main.bin 0x08000000
