###############################################################################
# Makefile for the project TransistorTestNew
###############################################################################

## General Flags
PROJECT = TransistorTestNew
UI_LANGUAGE = LANG_ENGLISH
MCU = atmega328p
TARGET = TransistorTestNew.elf
CC = avr-gcc

# programmer type
PROGRAMMER=usbtiny
PORT=usb

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
## CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
## CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d  

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2 			// org
##ASMFLAGS += -x assembler-with-cpp -Wa,-gstabs

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
#ORG# all: $(TARGET) TransistorTestNew.hex TransistorTestNew.eep TransistorTestNew.lss size  

##all: $(TARGET) TransistorTestNew.hex TransistorTestNew.eep TransistorTestNew.lss //mick, had to remove size, avrdude went nuts
##all: $(TARGET) TransistorTestNew.hex TransistorTestNew.eep
all: $(TARGET) TransistorTestNew.hex TransistorTestNew.eep TransistorTestNew.lss

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

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) TransistorTestNew.elf dep/* TransistorTestNew.hex TransistorTestNew.eep TransistorTestNew.lss TransistorTestNew.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

# device programming ATMega328

upload:
	avrdude -c usbtiny -B 10.0 -p m328p  -F -U flash:w:./TransistorTestNew.hex:a -U eeprom:w:./TransistorTestNew.eep:a
fuses:
	avrdude -c usbtiny -B 10.0 -p m328p  -F -U lfuse:w:0x62:m -U hfuse:w:0xd1:m

##for Mega8:
##avrdude -c usbtiny -B 10.0 -p m8 -F -U flash:w:./TransistorTestNew.hex:a -U eeprom:w:./TransistorTestNew.eep:a
##avrdude -c usbtiny -B 10.0 -p m8  -F -U lfuse:w:0x01:m -U hfuse:w:0xd9:m