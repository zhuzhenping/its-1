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
NAME := $(shell basename `pwd`)

TARGET := $(NAME)
MODULE_TYPE := dynamic_lib
SRCDIRS := $(SOLUTION_DIR)/src/$(NAME)
CPPFLAGS := -I$(INCLUDE_PATH)/ -I$(INCLUDE_PATH)/qt/include -I$(INCLUDE_PATH)/ctp/ctp/linux
LDFLAGS := -L$(LIB_PATH)/$(BUILD_TYPE) -L$(LIB_PATH)/boost -L$(LIB_PATH)/qt -lQtCore -lQtXml -lcommon -ldatalib

include ../build_module.mk
