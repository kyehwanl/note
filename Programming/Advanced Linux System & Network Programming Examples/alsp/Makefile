#----------------------------------------------------------------------------
# Destination directory for binaries and class libraries
#----------------------------------------------------------------------------
LIB_DIR	= lib

#----------------------------------------------------------------------------
# Compiler and linker options.
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
# Suffix rules
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
# Default target.
#----------------------------------------------------------------------------

all: mk_subdirs

clean: cl_subdirs

#----------------------------------------------------------------------------
# Making sub-directory:
#----------------------------------------------------------------------------

PREFIX_SRCDIR  = "src/"
SUBDIRS = 0.preface \
			1.process   \
			2.file    \
			3.text    \
			4.memory  \
			5.ipc     \
			6.io_interface     \
			7.io_multiplexing  \
			8.pthread  \
			9.signal  \
			10.realtime \
			appendix

mk_subdirs:
	@curdir=`pwd`; \
	for i in $(SUBDIRS) ; do \
		echo "==================================================================="; \
		echo " Build binary : ($(PREFIX_SRCDIR)$$i)"; \
		echo "==================================================================="; \
		$(MAKE) -C $(PREFIX_SRCDIR)$$i ; \
		if [ $$? -ne 0 ]; then \
			echo ""; echo ">> Error on making ($(PREFIX_SRCDIR)$$i)"; echo ""; \
			exit 1; \
		fi; \
	done; \
	echo "+ Finish";

#----------------------------------------------------------------------------
# Cleaning sub-directory
#----------------------------------------------------------------------------

cl_subdirs: rm_lib
	@curdir=`pwd`; \
	for i in $(SUBDIRS) ; do \
		echo "==================================================================="; \
		echo " Clean binary : ($(PREFIX_SRCDIR)$$i)"; \
		echo "==================================================================="; \
		$(MAKE) clean -C $(PREFIX_SRCDIR)$$i; \
		if [ $$? -ne 0 ]; then \
			echo ""; echo ">> Error on cleaning ($(PREFIX_SRCDIR)$$i)"; echo ""; \
			exit 1; \
		fi; \
		done; \
	echo "+ Finish"; \
	echo "+ CURRENT DIR:$$curdir"; \
	cd $$curdir

rm_lib:
	(cd $(LIB_DIR); rm -f lib*.*)
	echo "+ Remove built library"

#----------------------------------------------------------------------------
# Making tag
#----------------------------------------------------------------------------
mk_tags:
	@echo "Tags generated with whole srcs.."
	find ./ -name "*.[ch]" | xargs ctags 
