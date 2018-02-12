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

F_CPU = 1000000UL

CDEFS_LIST = DEBUG UART_USE_2X F_CPU=$(F_CPU)
CDEFS = $(addprefix -D, $(CDEFS_LIST))

CFLAGS  = -std=c99 -Wall -Wno-unused -I./src
CFLAGS += -mmcu=$(TARGET)
CFLAGS += -g $(OPTIMIZATION) $(CDEFS)

SRC  = main.c
SRC += kernel/kernel.c
SRC += kernel/dev/modem.c
SRC += kernel/driver/xbs2.c
SRC += kernel/sys/panic.c
SRC += kernel/serial/spi.c kernel/serial/uart.c
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

-include $(DEPS)
$(BUILD_PATH)/%.S.o: $(SRC_PATH)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

clean:
	@rm -rf $(BIN) $(DEPS) $(OBJ_PATH)
	@printf "$(BLD)RM$(RST)"
	@printf "\t%s %s\n" $(BIN) $(DEPS) $(OBJ_PATH)

docs:
	doxygen ./docs/Doxyfile

dump: $(ELF)
	$(DP) -m $(TARGET_ARCH) -S $^ > $^.dump
	@echo "Dumping $^ $(TARGET):$(TARGET_ARCH)"

sim: $(ELF)
	simavr -f $(F_CPU) -m atmega644 $(ELF)

.PHONY: clean debug docs dump flash sim
