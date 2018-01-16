#
# TELEMETRY F.W.
# AUTHOR - Harlan <root@aboyandhisgnu.org>
# DATE   - January 15, 2018
#
# https://aboyandhisgnu.org
#
NAME = telemetry

BUILD_PATH = ./build
OPTIMIZATION = -O3

CC = avr-gcc
CP = avr-objcopy
DP = avr-objdump

ISP = avrdude
ISP_TOOL = usbtiny
ISP_TARGET = m644p

TARGET = atmega644pa
TARGET_ARCH = avr5

CDEFS_LIST = DEBUG UART_USE_2X
CDEFS = $(addprefix -D, $(CDEFS_LIST))

CFLAGS  = -std=c99 -Wall -Wno-unused -I./src
CFLAGS += -mmcu=$(TARGET)
CFLAGS += -g $(OPTIMIZATION) $(CDEFS)

#LDFLAGS

SRC  = main.c
SRC += kernel/kernel.c
SRC += serial/uart.c
SRC += sys/panic.c
SRC_PATH = ./src

OBJ = $(addsuffix .o, $(SRC))
OBJ_PATH = $(addprefix $(BUILD_PATH)/, $(OBJ))

DEPS = $(OBJ_PATH:%.o=%.d)

BLD = \e[1m
RST = \e[0m

ELF = $(BUILD_PATH)/$(NAME).elf
HEX = $(BUILD_PATH)/$(NAME).hex

default: $(HEX)

flash: $(HEX)
	sudo $(ISP) -p $(ISP_TARGET) -c $(ISP_TOOL) -P usb -B 16 -U flash:w:$(HEX) -v

$(HEX): $(ELF)
	$(CP) -j .text -j .data -O ihex $^ $@

$(ELF): $(OBJ_PATH)
	@printf "$(BLD)CC$(RST)\t%s %s\n" $@ $<
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(DEPS)
$(BUILD_PATH)/%.c.o: $(SRC_PATH)/%.c
	@printf "$(BLD)MKDIR$(RST)"
	@printf "\t%s\n" $(@D)
	@mkdir -p $(@D)

	@printf "$(BLD)CC$(RST)\t%s %s\n" $@ $<
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

clean:
	@rm -rf $(BIN) $(DEPS) $(OBJ_PATH)
	@printf "$(BLD)RM$(RST)"
	@printf "\t%s %s\n" $(BIN) $(DEPS) $(OBJ_PATH)

dump: $(ELF)
	@$(DP) -m $(TARGET_ARCH) -S $^ > $^.dump
	@echo "Dumping $^ $(TARGET):$(TARGET_ARCH)"

.PHONY: clean debug dump flash
