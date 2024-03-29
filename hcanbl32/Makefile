
#MCU     = atmega32
MCU     = atmega644
CC      = avr-gcc
OBJCOPY = avr-objcopy

TARGET = main
OBJ     = $(TARGET).o
		 
F_CPU   = 3686400
F_CAN	= 8000000
CAN_SPEED = 125000

ifeq ($(MCU),atmega644)
	# atmega644 (64kbytes flash = 0 bis FFFF)
	# 0x7800*2=F000 size=4096 words (8kbytes Boot Size)
	MT_BOOTLOADER_ADDRESS = F000
	#JTAG-Enable: AVRDUDE_FLASH = avrdude -c avrispmkII -p m644p -P usb -e -U flash:w:"hcanBL_$(MCU)/$(TARGET).hex" -U hfuse:w:0x90:m -U lfuse:w:0xc7:m -U efuse:w:0xfc:m
	AVRDUDE_FLASH = avrdude -c avrispmkII -p m644p -P usb -e -U flash:w:"hcanBL_$(MCU)/$(TARGET).hex" -U hfuse:w:0xD0:m -U lfuse:w:0xc7:m -U efuse:w:0xfc:m
else
	# atmega32 (32kbytes flash = 0 bis 7FFF)
	# 0x3800*2=0x7000 for ATmega32  2048 words Boot Size
	# (doppelt so gross wie Atmega8 -> hier 4kbytes Boot Size)
	MT_BOOTLOADER_ADDRESS = 7000
	#JTAG-Enable: AVRDUDE_FLASH = avrdude -c avrispmkII -p m32 -P usb -e -U flash:w:"hcanBL_$(MCU)/$(TARGET).hex" -U hfuse:w:0x80:m -U lfuse:w:0x0f:m
	AVRDUDE_FLASH = avrdude -c avrispmkII -p m32 -P usb -e -U flash:w:"hcanBL_$(MCU)/$(TARGET).hex" -U hfuse:w:0xC0:m -U lfuse:w:0x0f:m
endif


CFLAGS= -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues \
		-std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct \
		-fshort-enums -DF_CPU=$(F_CPU) -DF_CAN=$(F_CAN) -DCAN_SPEED=$(CAN_SPEED)

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref

################## BOOTLOADER ######################
# MT_BOOTLOADER_ADDRESS (=Start of Boot Loader section
# in bytes - not words) is defined above.
LDFLAGS += -Wl,--section-start=.text=$(MT_BOOTLOADER_ADDRESS)

all: $(TARGET).hex
	avr-size $(TARGET).hex
	test -d ./hcanBL_$(MCU) || mkdir hcanBL_$(MCU)
	mv $(TARGET).hex hcanBL_$(MCU)

$(TARGET).hex : $(TARGET).out 
	$(OBJCOPY) -R .eeprom -O ihex $(TARGET).out $(TARGET).hex 

$(TARGET).out : $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).out -Wl,-Map,$(TARGET).map $(OBJ)

flash:
	sudo $(AVRDUDE_FLASH)
	#uisp -dlpt=/dev/parport0 --erase  -dprog=dapa
	#uisp -dlpt=/dev/parport0 --upload if="hcanBL_$(MCU)/$(TARGET).hex" -dprog=dapa  -v=3 --hash=32

erase_eeprom:
	uisp -dlpt=/dev/parport0 --erase --segment=eeprom -dprog=dapa

clean:
	rm -f *.o *.map *.out
	test -d ./hcanBL_$(MCU) || cd hcanBL_$(MCU); sudo rm -f $(TARGET).hex

ctags:
	ctags *.c *.h

