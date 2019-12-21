#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
PREFIX_DIR	= ../../..

OUT		= io_epoll_pthread

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------

CC			= gcc
CFLAGS		= -Wall -g

CPPFLAGS	= -I$(PREFIX_DIR)/include
FPICFLAGS	= -fPIC 

LOADLIBES	= -L$(PREFIX_DIR)/lib
LDLIBS		= -lpthread -lsysvipc
#----------------------------------------------------------------------------
# user defined macros 
#----------------------------------------------------------------------------
SRCS		= $(wildcard *.c)
OBJS		= $(patsubst %.c,%.o,$(SRCS))
DEPENDENCY	= dep.mk

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------
.PHONY: all clean dep

all: $(OUT)

#----------------------------------------------------------------------------
# Suffix rules
#----------------------------------------------------------------------------

.SUFFIXES: 
.SUFFIXES: .c .o
.PRECIOUS: .o

#----------------------------------------------------------------------------
# Build binaries
#----------------------------------------------------------------------------

io_epoll_pthread : $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)
	ctags *.c $(PREFIX_DIR)/include/*.h

#----------------------------------------------------------------------------
# others
#----------------------------------------------------------------------------
dep: $(DEPENDENCY)

$(DEPENDENCY): $(SRCS)
	$(CC) -M $(CPPFLAGS) $(SRCS) > $(DEPENDENCY)

clean:
	@echo "Remove binary & object file"
	-rm -f *.o core core.* $(DEPENDENCY) $(OUT) tags

ifeq "$(DEPENDENCY)" "$(filter $(DEPENDENCY),$(wildcard *.mk))"
include $(DEPENDENCY)
endif
