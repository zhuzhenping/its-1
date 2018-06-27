#######Makefile Begin#######################

SOLUTION_NAME := its

ifeq ($(MODULE_TYPE), dynamic_lib)
 TARGET_NAME := lib_$(TARGET).so
endif
ifeq ($(MODULE_TYPE), static_lib)
 TARGET_NAME := lib_$(TARGET).a
endif
ifeq ($(MODULE_TYPE), executable)
 TARGET_NAME := exe_$(TARGET)
endif

ifeq ($(BUILD_TYPE),Release)
 CFLAGS = -O2 -Wall 
else
 CFLAGS = -g -Wall -DDEBUG
endif

SRCEXTS = .c .cpp
CC = g++

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS= $(patsubst %.cpp,./obj/%.o,$(notdir $(SOURCES)))

vpath %.o ./obj
vpath %.cpp $(dir $(SOURCES))

.PHONY : all objs clean rebuild install
all : $(TARGET) 

objs : $(OBJS)
./obj/%.o : %.c
	@echo "============="
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

./obj/%.o : %.cpp
	@echo "============="
	@echo "Compiling $<"
	$(CC)  $(CFLAGS) $(CPPFLAGS) -c $< -o $@

	
$(TARGET) : BUILD_PRE $(OBJS)
ifeq ($(MODULE_TYPE), dynamic_lib)
	$(CC) -o $(BIN_PATH)/$(BUILD_TYPE)/$(TARGET_NAME) -shared -fPIC -Duse_namespace $(OBJS) $(LDFLAGS)
endif

ifeq ($(MODULE_TYPE), static_lib)
	ar rcs $(LIB_PATH)/$(BUILD_TYPE)/$(TARGET_NAME) $(OBJS)
endif

ifeq ($(MODULE_TYPE), executable)
	$(CC) -o $(BIN_PATH)/$(BUILD_TYPE)/$(TARGET_NAME) $(OBJS)  $(LDFLAGS)
endif

BUILD_PRE :
	@-mkdir -p $(BIN_PATH)/$(BUILD_TYPE)
	@-mkdir -p $(LIB_PATH)/$(BUILD_TYPE)
	@-mkdir -p ./obj

install : all
	@-mkdir -p $(INSTALL_DIR)/$(SOLUTION_NAME)/bin
	@-mkdir -p $(INSTALL_DIR)/$(SOLUTION_NAME)/lib

ifeq ($(MODULE_TYPE), dynamic_lib)
	cp -f $(BIN_PATH)/$(BUILD_TYPE)/$(TARGET_NAME) $(INSTALL_DIR)/$(SOLUTION_NAME)/bin
endif

ifeq ($(MODULE_TYPE), static_lib)
	cp -f $(LIB_PATH)/$(BUILD_TYPE)/$(TARGET_NAME) $(INSTALL_DIR)/$(SOLUTION_NAME)/lib
endif

ifeq ($(MODULE_TYPE), executable)
	cp -f $(BIN_PATH)/$(BUILD_TYPE)/$(TARGET_NAME) $(INSTALL_DIR)/$(SOLUTION_NAME)/bin
	@chmod a+x $(BIN_PATH)/$(BUILD_TYPE)/$(TARGET_NAME)
	@chmod og-w $(BIN_PATH)/$(BUILD_TYPE)/$(TARGET_NAME)
endif

rebuild: clean all

clean :
	@echo "cleaning ..."
	@echo "rm objects ..."
	@-rm -f ./obj/*.o
	-rm -f $(BIN_PATH)/$(BUILD_TYPE)/$(TARGET_NAME)
	-rm -f $(LIB_PATH)/$(BUILD_TYPE)/$(TARGET_NAME)

###############################################################################

