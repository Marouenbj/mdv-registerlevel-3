PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# Directories
SRC_DIR = src
OBJ_DIR = build
INC_DIR = \
	-Iinclude \
	-I/usr/arm-none-eabi/include
STARTUP_DIR = startup

ASM_DIR = asm

# Files 
SRC := $(wildcard $(SRC_DIR)/*.c)
SRC += $(wildcard $(STARTUP_DIR)/*.c)
ASM := $(wildcard $(ASM_DIR)/*.s)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
OBJ += $(patsubst $(ASM_DIR)/%.s, $(OBJ_DIR)/%.o, $(ASM))
OBJ := $(patsubst $(STARTUP_DIR)/%.c, $(OBJ_DIR)/%.o, $(OBJ))
LD := $(wildcard $(STARTUP_DIR)/*.ld)

COREFLAGS=-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS = $(COREFLAGS)
CFLAGS += -Ofast -falign-functions=1 -falign-jumps=1 -falign-loops=1 -falign-labels=1 -ffunction-sections -fdata-sections -Wall -save-temps -Wno-unused-function
CFLAGS += -I$(INC_DIR)
LFLAGS = $(COREFLAGS) -T$(LD) -Wl,-Map=$(OBJ_DIR)/main.map,--gc-sections

AFLAGS = $(COREFLAGS)
AFLAGS += -mimplicit-it=thumb

#PATHS
OPENOCD_INTERFACE = interface/stlink.cfg
OPENOCD_TARGET = target/stm32f3x.cfg

# Targets
TARGET = $(OBJ_DIR)/main.elf

all: $(OBJ) $(TARGET)

$(OBJ_DIR)/%.o : $(ASM_DIR)/%.s | mkobj
	$(AS) $(AFLAGS) -o $@ $^

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c | mkobj
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o : $(STARTUP_DIR)/%.c | mkobj
	$(CC) -c $(CFLAGS) -o $@ $^

$(TARGET) : $(OBJ) | mkobj
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^

mkobj:
	mkdir -p $(OBJ_DIR)

flash:
	openocd -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c \
	"program $(TARGET) verify reset exit"

clean:
	rm -rf $(OBJ_DIR)

force: clean all flash

.PHONY = mkobj clean force flash