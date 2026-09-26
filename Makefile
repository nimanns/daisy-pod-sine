TARGET = DaisyPodSine
CPP_SOURCES = DaisyPodSine.cpp

LIBDAISY_DIR = libDaisy
DAISYSP_DIR = DaisySP

OCD_DIR = /opt/homebrew/share/openocd/scripts
GCC_PATH = $(HOME)/.local/xpack-arm-none-eabi-gcc-15.2.1-1.1/bin

SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
