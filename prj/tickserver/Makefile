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


TARGET := $(shell basename `pwd`)
MODULE_TYPE := executable
SRCDIRS := $(SOLUTION_DIR)/src/main
CPPFLAGS := -I$(INCLUDE_PATH)/ -I$(INCLUDE_PATH)/qt/linux/include
LDFLAGS := -L$(LIB_PATH)/$(BUILD_TYPE) -L$(LIB_PATH)/boost -L$(LIB_PATH)/qt -L$(LIB_PATH)/ctp -lboost_thread -lpthread -lboost_system -lQtCore -lQtXml -lcommon -lctp  -ldatalib -lnetwork -laccount -lstrategy -lthosttraderapi -lthostmduserapi


include ../build_module.mk
