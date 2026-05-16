# Config

CPU_FREQUENCY=16000000
MMCU=atmega328p

UPLOAD_BAUDS=115200
UPLOAD_DEVICE=/dev/ttyUSB0

#--------

CC=avr-gcc
CFLAGS=#-Wall -Wextra -Werror
CPPFLAGS=-MMD -Os -mmcu=$(MMCU)
LDFLAGS=-Wall -mmcu=$(MMCU)
# LDLIBS=-nostdlib

COPY=avr-objcopy

BUILD_DIR=build/
INC=-I includes/

SRCS_DIR=./srcs/
SRCS=./srcs/main.c \
		./srcs/aht20.c\


NAME_BIN=main.bin

#--------


RM=rm -rf
MKDIR=mkdir -p

CPPFLAGS+=$(INC)
CPPFLAGS += -D F_CPU=$(CPU_FREQUENCY)UL

NAME_HEX=$(NAME_BIN:%.bin=%.hex)
OBJS=$(SRCS:$(SRCS_DIR)%.c=$(BUILD_DIR)%.o)

all: hex flash

$(BUILD_DIR)%.o: $(SRCS_DIR)%.c
	$(MKDIR) $(@D)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<


$(NAME_BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

%.hex: %.bin
	avr-objcopy -O ihex $< $@

hex: $(NAME_HEX)

clean:
	$(RM) $(BUILD_DIR) $(NAME_HEX) $(NAME_BIN)

re: clean all

flash: $(NAME_HEX)
	avrdude -c arduino -p $(MMCU) -b $(UPLOAD_BAUDS) -P $(UPLOAD_DEVICE) -U flash:w:$(NAME_HEX):i

screen: flash
	screen /dev/ttyUSB0 115200

.PHONY: all re clean hex flash
