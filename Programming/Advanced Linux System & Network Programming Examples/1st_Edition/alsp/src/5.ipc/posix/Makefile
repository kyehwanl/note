PREFIX_DIR	= ../../..
#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------
CC			= gcc
CPPFLAGS	= -I$(PREFIX_DIR)/include
MAKE		= make

#----------------------------------------------------------------------------
# user defined macros
#----------------------------------------------------------------------------
SRCS        = $(wildcard *.c)
DEPENDENCY  = dep.mk
DEFAULT_MAKEFILE = default.mk

#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------
.PHONY: all clean dep

all: dep
	$(MAKE) -f $(DEFAULT_MAKEFILE)

clean:
	$(MAKE) clean -f $(DEFAULT_MAKEFILE)

dep: $(DEPENDENCY)

$(DEPENDENCY): $(SRCS)
	$(CC) -M $(CPPFLAGS) $(SRCS) > $(DEPENDENCY)
