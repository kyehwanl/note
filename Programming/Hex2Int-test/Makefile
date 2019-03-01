CC=gcc
CPP=g++
LD=ld
KERNELDIR=/root/linux
KERNEL_SOURCE:=$(KERNELDIR)

CFLAGS2 = -Wall -g -D__DEBUG__ 
CFLAGS = -D__DEBUG__ -g -O0
KFLAGS = -D__KERNEL__ -DMODULE -O -Wall -I$(KERNELDIR)/include
SFLAGS = -D_REENTRANT
#LIBS = -lpthread -lm -lnetlink -lutil 
LIBS =  -lcrypto
#INCS = -I/usr/src/kernels/2.6.25.3-18.fc9.i686/include/linux
INCS = #-I$(PWD)/header -I~/srx_test1/020-03/srx-0.2.0/src/shared/crc32.h
#LDAPLIBS = -lldap 
#LDAPLIBDIR = -L/usr/local/src/openldap-2.4.11/libraries/libldap/.libs/ 
#LBERLIBS = -llber
#LBERLIBSDIR = -L/usr/local/src/openldap-2.4.11/libraries/liblber/.libs/

# COMMAND OBJECT FILE
CMD_OBJS = 
# DRIVER OBJECT FILE
DRV_OBJS = 
# CORE OBJECT FILE
COR_OBJS =  aaa.o
# THREAD(TASK) OBJECT FILE
TAS_OBJS = 
# LDAP OBJECT FILE
LDAP_OBJS = 
# KERNEL MODULE Object
KERN_OBJS = #ID_LOC_init_module.o

ALL_OBJS = $(CMD_OBJS) $(DRV_OBJS) $(COR_OBJS) $(TAS_OBJS) $(LDAP_OBJS)

# SOURCE and OBJECT FILES RELATED COMMUNICATION
#AGENT_SRCS = ini.c util_cal.c crc.c
#AGENT_OBJS = agent.o agent_addon.o MulticastOperation.o ini.o util_cal.o crc.o
#GC1115_SRCS = gc1115.c cal_coefs.c

#KERNEL MODULE
KER_MODS = 

BUILD = test

######################
# SUFFIXES RULE      #
######################

.SUFFIXES : .o .c
.c.o :
	$(CC) $(INCS) $(CFLAGS) -c $<
#	$(CC) $(INCS) $(CFLAGS) $(SFLAGS) -c $< 

######################
# MAKING RULE        #
######################

.PHONY: all clean
#all : $(KER_MODS) ID_LOCd 
all : $(BUILD)


######################
# RELATION RULE      #
######################

$(COR_OBJS): $($@:.o=.c) Makefile
$(CMD_OBJS): $($@:.o=.c) $($@:.o=.h) 




$(BUILD): $(ALL_OBJS)
#	$(CC) $(INCS) --static $(CFLAGS) -o $@ $^ $(LIBS) -L. 
#	$(CC) $(INCS) --static $(CFLAGS) -o $@ $^ $(LIBS) -L. $(LDAPLIBS) $(LDAPLIBDIR) $(LBERLIBS) $(LBERLIBSDIR)
#	$(CC) $(INCS) $(CFLAGS) -o $@ $^ $(LIBS) -L. $(LDAPLIBS) $(LBERLIBS) -lssl -lcrypto  -L/usr/lib
	$(CC) $(INCS) $(CFLAGS) -o $@ $^ $(LIBS) -L 

utils: utils.c
	$(CC) $(INCS) $(CFLAGS) -o $@ utils.c $(LIBS) -L.
#	$(CC) $(INCS) --static $(CFLAGS) -o $@ utils.c $(LIBS) -L.

agent:	
	$(CC) $(INCS) --static $(CFLAGS) $(SFLAGS) -c  $(AGENT_SRCS) 
	ar rs libagent.a $(AGENT_OBJS)
	
	
clean: 
	rm -rf *.o *~
	rm -rf $(BUILD)

tag:
	rm -rf tags
	find ./ -name '*.[chCH]' |  ctags -R -L -
	find ./ -name '*.[ch]' > cscope.files && cscope -b
#	../../make_cstags.sh
