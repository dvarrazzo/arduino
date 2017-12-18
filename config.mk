# Configuration file for arduino makefiles.
#
# Arduino adaptation by mellis, eighthave, oli.keller

TARGET = $(notdir $(CURDIR))

# Installation path of the arduino environment
INSTALL_DIR = /usr/share/arduino

PORT = /dev/ttyACM0
UPLOAD_RATE = 57600
AVRDUDE_PROGRAMMER = arduino
ARDUINO_VARIANT = standard
MCU = atmega168
F_CPU = 16000000

# Optional functionalities
#
# In order to include optional pieces of the arduino library,
# define any of the following symbols in your makefile.
HAVE_SERVO =
HAVE_WIRE =
HAVE_LIQUID_CRYSTAL =

