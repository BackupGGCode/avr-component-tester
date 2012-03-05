###############################################################################
# AVR Component Tester
#
# Makefile for Atmega328p
###############################################################################

## General Flags
PROJECT = TransistorTestNew
UI_LANGUAGE = LANG_ENGLISH
MCU = atmega328p
TARGET = TransistorTestNew.elf
TARGET_BASE = TransistorTestNew
CC = avr-gcc

# programmer type
PROGRAMMER=usbasp
PORT=usb
LFUSE=0x62
HFUSE=0xD1

AVRDUDE = avrdude

AVRDUDE_PROGRAMMER = $(PROGRAMMER) 
AVRDUDE_PORT = $(PORT)

AVRDUDE_FLAGS = -p $(MCU) -c $(AVRDUDE_PROGRAMMER) -P $(AVRDUDE_PORT) -B 5 

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET_BASE).hex:a
AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET_BASE).eep:a
AVRDUDE_WRITE_FUSES = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=TransistorTestNew.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o lcd-routines.o swuart.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) TransistorTestNew.hex TransistorTestNew.eep TransistorTestNew.lss size

## Compile
main.o: ./main.c
	$(CC) $(INCLUDES) $(CFLAGS) -D$(UI_LANGUAGE) -c  $<

lcd-routines.o: ./lcd-routines.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
	
swuart.o: ./swuart.S
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Program
# Program the device source only.  
program:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

## Fuses (WARNING FOR ATMEGA328P If you are using a differnt AVR; look up the fuses yourself (0 startup time 1Mhz most important)
fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FUSES)

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) TransistorTestNew.elf dep/* TransistorTestNew.hex TransistorTestNew.eep TransistorTestNew.lss TransistorTestNew.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

# device programming
