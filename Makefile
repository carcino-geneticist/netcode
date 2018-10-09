CC 	= gcc
CFLAGS	= -c -g -Wall
SRC	= src
BIN	= bin
LIB	= lib
BUILD	= build
INCLUDE	= include

NAME	= netcode
LIBS 	= -lpthread -ldatstruc
##########################################################

TARGET		= $(BIN)/lib$(NAME).so
SOURCES		= $(wildcard $(SRC)/$(NAME)/*.c)
SOURCES		+= $(wildcard $(SRC)/$(NAME)/*/*.c)
OBJECTS		= $(patsubst $(SRC)/$(NAME)%.c,$(BUILD)%.o, $(SOURCES))

TESTNAME	= a.$(NAME).out
TESTLIBS	= -l$(NAME)
TESTBIN		= $(BIN)/$(TESTNAME)
TESTSRC		= $(SRC)/main.$(NAME).c
TESTOBJ		= $(patsubst $(SRC)%.c,$(BUILD)%.o, $(TESTSRC))

all: $(TARGET) $(TESTBIN)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN)
	$(CC) -shared -L$(LIB) $(LIBS) $(OBJECTS) -o $@
	@mkdir -p $(LIB)
	@cp $(TARGET) $(LIB)

$(OBJECTS): $(BUILD)/%.o: $(SRC)/$(NAME)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE) -fPIC $< -o $@

$(TESTBIN): $(TARGET) $(TESTOBJ)
	$(CC) -Wall $(TESTOBJ) -I$(INCLUDE) -L$(LIB) $(TESTLIBS) -o $@

$(TESTOBJ): $(TESTSRC)
	$(CC) $(CFLAGS) $(TESTSRC) -I$(INCLUDE) -o $@

.PHONY: clean
clean: 
	rm -rf $(BIN) $(BUILD)
run: $(TESTBIN)
	LD_LIBRARY_PATH=$(LIB) ./$(TESTBIN)
