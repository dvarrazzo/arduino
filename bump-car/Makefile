# Arduino program Makefile

# Root path containing the arduino libraries 
LIBS_DIR = ..

include $(LIBS_DIR)/config.mk

HAVE_SERVO = 1

# SRC = 

CXXSRC = \
	$(LIBS_DIR)/ardulib/Pot.cpp \
	$(LIBS_DIR)/ardulib/Button.cpp \
	$(LIBS_DIR)/ardulib/ServoMount.cpp \
	$(LIBS_DIR)/ardulib/TwoServosCar.cpp


include $(LIBS_DIR)/tools/rules.mk

