PORT=/dev/ttyACM1 #verander deze poort naar de geschikte voor je eigen pc
# Lars: /dev/ttyACM0 
MCU=atmega32u4
CFLAGS = -mmcu=$(MCU) -Wall
CC = avr-gcc
# voeg de c files toe die mee moeten doen met compileren.
SRC= main.c usart.c motor.c encoder.c sensoren.c #uphill.c gyro.c 
OBJ= $(SRC:.c=.o)
TARGET = ../bin/zumo

OBJCOPY = avr-objcopy
FORMAT  = ihex

all:elf hex

elf: $(TARGET).elf
hex: $(TARGET).hex


$(TARGET).elf: $(OBJ)
	$(CC)  $(CFLAGS) $^ -o $@

%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@


%.o:%.c 
	$(CC)  $(CFLAGS) -c -Os $<


clean:
	rm *.o
	rm *.elf
	rm *.hex

program: $(TARGET).hex
	avrdude -p $(MCU) -c avr109 -P $(PORT) -U flash:w:$(TARGET).hex -u
