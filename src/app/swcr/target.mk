SWCR_DIR = $(REP_DIR)/include/swcr/services

TARGET = swcr
SRC_CC = main.cc $(SWCR_DIR)/cpu_thread.cc $(SWCR_DIR)/cpu.cc
LIBS   = base