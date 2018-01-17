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

CC = gcc
CP = objcopy
DP = objdump

CDEFS_LIST  = MULTITHREAD 
CDEFS_LIST += _POSIX_C_SOURCE=199309L
CDEFS = $(addprefix -D, $(CDEFS_LIST))

CFLAGS  = -std=c99 -Wall -Wno-unused -I./src
CFLAGS += -g $(OPTIMIZATION) $(CDEFS)

LDFLAGS = -lftdi1 -lpthread

SRC  = main.c commands.c
SRC += threading/telemetry.c threading/mutex_queue.c
SRC += terminal/terminal.c
SRC_PATH = ./src

OBJ = $(addsuffix .o, $(SRC))
OBJ_PATH = $(addprefix $(BUILD_PATH)/, $(OBJ))

DEPS = $(OBJ_PATH:%.o=%.d)

BLD = \e[1m
RST = \e[0m

BIN = $(BUILD_PATH)/$(NAME)

default: $(BIN)

$(BIN): $(OBJ_PATH)
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

run: $(BIN)
	sudo ./$^

.PHONY: clean debug run
