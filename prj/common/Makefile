ifndef its
  its = /home/wd/its
endif

ifndef INSTALL_DIR
  INSTALL_DIR = /opt
endif

ifndef SOLUTION_DIR
  SOLUTION_DIR = $(its)
endif

ifeq ($(mode), release)
  export BUILD_TYPE := Release
else
  export BUILD_TYPE := Debug
endif

BIN_PATH := $(SOLUTION_DIR)/bin
INCLUDE_PATH := $(SOLUTION_DIR)/inc
LIB_PATH := $(SOLUTION_DIR)/lib

#TARGET := @basename `pwd`
TARGET := common
MODULE_TYPE := dynamic_lib
SRCDIRS := $(SOLUTION_DIR)/src/common
CPPFLAGS := -I$(INCLUDE_PATH)/ -I$(INCLUDE_PATH)/common -I$(INCLUDE_PATH)/qt
LDFLAGS := -L$(LIB_PATH)/qt

include ../build_module.mk
