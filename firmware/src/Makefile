# simple AVR Makefile
#
# written by michael cousins (http://github.com/mcous)
# released to the public domain

# Makefile
#
# targets:
#   all:    compiles the source code
#   test:   tests the isp connection to the mcu
#   flash:  writes compiled hex file to the mcu's flash memory
#   fuse:   writes the fuse bytes to the MCU
#   disasm: disassembles the code for debugging
#   clean:  removes all .hex, .elf, and .o files in the source code and library directories

# parameters (change this stuff accordingly)
# project name
PRJ = clockblock
# avr mcu
MCU = atmega328p
# mcu clock frequency
CLK = 16000000
# avr programmer (and port if necessary)
# e.g. PRG = usbtiny -or- PRG = arduino -P /dev/tty.usbmodem411
PRG = arduino -P /dev/tty.usbmodem411
# fuse values for avr: low, high, and extended
# these values are from an Arduino Uno (ATMega328P)
# see http://www.engbedded.com/fusecalc/ for other MCUs and options
LFU = 0xFF
HFU = 0xDE
EFU = 0x05
# program source files (not including external libraries)
SRC = $(PRJ).cpp
# where to look for included libraries (consisting of .c/.cpp files and .h files)
# e.g. INC = ../../EyeToSee ../../YouSART
INC = include/DS3234 include/StuPId ../../../TLC5940
# any defines to send the compiler
DEF = BREADBOARD
# directories
OBJDIR = bin/obj
EXEDIR = bin/exe

#################################################################################################
# \/ stuff nobody needs to worry about until such time that worrying about it is appropriate \/ #
#################################################################################################

# include path
INCLUDE := $(foreach dir, $(INC), -I$(dir))
# defines
DEFINES := $(foreach def, $(DEF), -D$(def))
# c flags
CFLAGS    = -Wall -Os -DF_CPU=$(CLK) $(DEFINES) -mmcu=$(MCU) $(INCLUDE)
# any aditional flags for c++
CPPFLAGS =

# executables
AVRDUDE = avrdude -c $(PRG) -p $(MCU)
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size --format=avr --mcu=$(MCU)
CC      = avr-gcc

# generate list of objects
CFILES    = $(filter %.c, $(SRC))
INCC     := $(foreach dir, $(INC), $(wildcard $(dir)/*.c))
CPPFILES  = $(filter %.cpp, $(SRC))
INCCPP   := $(foreach dir, $(INC), $(wildcard $(dir)/*.cpp))
OBJ       = $(CFILES:.c=.o) $(INCC:.c=.o) $(CPPFILES:.cpp=.o) $(INCCPP:.cpp=.o)

# user targets
# compile all files
all: $(EXEDIR)/$(PRJ).hex

# test programmer connectivity
test:
	$(AVRDUDE) -v

# flash program to mcu
flash: all
	$(AVRDUDE) -U flash:w:$(EXEDIR)/$(PRJ).hex:i

# write fuses to mcu
fuse:
	$(AVRDUDE) -U lfuse:w:$(LFU):m -U hfuse:w:$(HFU):m -U efuse:w:$(EFU):m

# generate disassembly files for debugging
disasm: $(EXEDIR)/$(PRJ).elf
	$(OBJDUMP) -d $(EXEDIR)/$(PRJ).elf

# remove compiled files
clean:
	rm -f $(EXEDIR)/*.elf $(OBJDIR)/*.o

# other targets
# objects from c files
.c.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$(notdir $@)

# objects from c++ files
.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$(notdir $@)

# elf file
$(EXEDIR)/$(PRJ).elf: $(OBJ) | $(OBJDIR)
	$(CC) $(CFLAGS) -o $(EXEDIR)/$(PRJ).elf $(addprefix $(OBJDIR)/,$(notdir $(OBJ)))

# hex file
$(EXEDIR)/$(PRJ).hex: $(EXEDIR)/$(PRJ).elf | $(EXEDIR)
	rm -f $(EXEDIR)/$(PRJ).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(EXEDIR)/$(PRJ).elf $(EXEDIR)/$(PRJ).hex
	$(SIZE) $(EXEDIR)/$(PRJ).elf

$(EXEDIR):
	mkdir -p $(EXEDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)
