# Rules for the arduino makefiles.
#
# Arduino adaptation by mellis, eighthave, oli.keller

TARGET = $(notdir $(CURDIR))

ARDUINO = $(INSTALL_DIR)/hardware/arduino/cores/arduino
ARDUINO_VARIANT ?= standard
ARDUINO_LIBS_DIR = $(INSTALL_DIR)/libraries/
AVR_TOOLS_PATH = /usr/bin

CXXSRC += $(TARGET).cpp

# Arduino library required by about all the projects.

CINCS += -I$(ARDUINO) -I$(ARDUINO)/../../variants/$(ARDUINO_VARIANT)
CXXINCS += -I$(LIBS_DIR)

SRC += $(ARDUINO)/wiring_shift.c $(ARDUINO)/WInterrupts.c \
       $(ARDUINO)/wiring.c $(ARDUINO)/wiring_digital.c \
       $(ARDUINO)/wiring_analog.c

CXXSRC += $(ARDUINO)/HardwareSerial.cpp $(ARDUINO)/WMath.cpp \
          $(ARDUINO)/Print.cpp $(ARDUINO)/Stream.cpp \
          $(ARDUINO)/main.cpp $(ARDUINO)/new.cpp


#
# Optional chunks of functionalities
#
ifdef HAVE_SERVO
CXXSRC += $(ARDUINO_LIBS_DIR)/Servo/Servo.cpp
CXXINCS += -I$(ARDUINO_LIBS_DIR)/Servo
endif

ifdef HAVE_WIRE
SRC += $(ARDUINO_LIBS_DIR)/Wire/utility/twi.c
CXXSRC += $(ARDUINO_LIBS_DIR)/Wire/Wire.cpp
CXXINCS += -I$(ARDUINO_LIBS_DIR)/Wire/ \
	-I$(ARDUINO_LIBS_DIR)/Wire/utility/
endif

ifdef HAVE_LIQUID_CRYSTAL
CXXSRC += $(ARDUINO_LIBS_DIR)/LiquidCrystal/LiquidCrystal.cpp
CXXINCS += -I$(ARDUINO_LIBS_DIR)/LiquidCrystal
endif

ifdef HAVE_FASTLED
FASTLED_DIR = $(LIBS_DIR)/libs/FastLED
CXXSRC += $(FASTLED_DIR)/FastLED.cpp
CXXSRC += $(FASTLED_DIR)/hsv2rgb.cpp
CXXSRC += $(FASTLED_DIR)/colorutils.cpp
CXXINCS += -I$(FASTLED_DIR)
endif

FORMAT = ihex

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)
CXXDEFS = -DF_CPU=$(F_CPU)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEFS) $(CINCS) $(CXXINCS) -O$(OPT) -include new.h -include Arduino.h
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
LDFLAGS = -lm

# Tell the compiler to drop unused code.
CXXFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

# Programming support using avrdude. Settings and variables.
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS = -V -F -C $(INSTALL_DIR)/hardware/tools/avrdude.conf \
-p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

# Program settings
CC = $(AVR_TOOLS_PATH)/avr-gcc
CXX = $(AVR_TOOLS_PATH)/avr-g++
OBJCOPY = $(AVR_TOOLS_PATH)/avr-objcopy
OBJDUMP = $(AVR_TOOLS_PATH)/avr-objdump
AR  = $(AVR_TOOLS_PATH)/avr-ar
SIZE = $(AVR_TOOLS_PATH)/avr-size
NM = $(AVR_TOOLS_PATH)/avr-nm
AVRDUDE = $(AVR_TOOLS_PATH)/avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: build sizeafter

build: elf hex

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym

# Program the device.
upload: $(TARGET).hex
	stty -F $(PORT) hupcl
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE)  $(TARGET).elf
sizebefore:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(HEXSIZE); echo; fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(HEXSIZE); echo; fi


# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000


coff: $(TARGET).elf
	$(COFFCONVERT) -O coff-avr $(TARGET).elf $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr $(TARGET).elf $(TARGET).cof


.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@

# Link: create ELF output file from library.
$(TARGET).elf: $(TARGET).o core.a
	$(CC) $(ALL_CFLAGS) -o $@ $(TARGET).o -L. core.a $(LDFLAGS)

core.a: $(OBJ)
	@for i in $(OBJ); do echo $(AR) rcs core.a $$i; $(AR) rcs core.a $$i; done



# Compile: create object files from C++ source files.
.cpp.o:
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@ 

.cpp.s:
	$(CXX) -S $(ALL_CXXFLAGS) $< -o $@ 

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Automatic dependencies
%.d: %.c
	$(CC) -M $(ALL_CFLAGS) $< | sed "s;$(notdir $*).o:;$*.o $*.d:;" > $@

%.d: %.cpp
	$(CXX) -M $(ALL_CXXFLAGS) $< | sed "s;$(notdir $*).o:;$*.o $*.d:;" > $@


# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss core.a \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)

.PHONY:	all build elf hex eep lss sym coff extcoff clean sizebefore sizeafter

include $(SRC:.c=.d)
include $(CXXSRC:.cpp=.d)
