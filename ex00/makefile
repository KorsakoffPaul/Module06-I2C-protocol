DEVICE = atmega328p
CLOCK = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy

DUDE = avrdude
PROGRAMMER = arduino
BAUD_RATE = 115200
USB_DEVICE = /dev/ttyUSB0
ACTION = flash:w:main.hex
DUDE_FLAGS = -c $(PROGRAMMER) -p $(DEVICE) -b $(BAUD_RATE) -P $(USB_DEVICE) -U $(ACTION)

CFLAGS = -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -Os

all: hex flash

hex: main.c
	$(CC) $(CFLAGS) -o main.bin main.c
	$(OBJCOPY) -O ihex main.bin main.hex

flash: hex
	$(DUDE) $(DUDE_FLAGS)

screen: flash
	screen /dev/ttyUSB0 115200

clean:
	rm -f main.bin main.hex