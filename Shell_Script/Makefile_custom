CC=gcc
CPP=g++
LD=ld
KERNELDIR=/root/linux
KERNEL_SOURCE:=$(KERNELDIR)

CFLAGS2 = -Wall -g -D__DEBUG__ 
CFLAGS = -D__DEBUG__ 
KFLAGS = -D__KERNEL__ -DMODULE -O -Wall -I$(KERNELDIR)/include
SFLAGS = -D_REENTRANT
LIBS = -lpthread -lm -lnetlink -lutil 
INCS = -I/usr/src/redhat/SOURCES/iproute-2.6.20-070313/include/linux
#INCS = -I/usr/src/kernels/2.6.25.3-18.fc9.i686/include/linux
LDAPLIBS = -lldap 
LDAPLIBDIR = -L/usr/local/src/openldap-2.4.11/libraries/libldap/.libs/ 
LBERLIBS = -llber
LBERLIBSDIR = -L/usr/local/src/openldap-2.4.11/libraries/liblber/.libs/

# COMMAND OBJECT FILE
CMD_OBJS = mmi.o 
# DRIVER OBJECT FILE
DRV_OBJS = utils.o read_table.o
# CORE OBJECT FILE
COR_OBJS = ID_LOCd.o  
# THREAD(TASK) OBJECT FILE
TAS_OBJS = 
# LDAP OBJECT FILE
LDAP_OBJS = ldap_client.o
# KERNEL MODULE Object
KERN_OBJS = #ID_LOC_init_module.o

ALL_OBJS = $(CMD_OBJS) $(DRV_OBJS) $(COR_OBJS) $(TAS_OBJS) $(LDAP_OBJS)

# SOURCE and OBJECT FILES RELATED COMMUNICATION
#AGENT_SRCS = ini.c util_cal.c crc.c
#AGENT_OBJS = agent.o agent_addon.o MulticastOperation.o ini.o util_cal.o crc.o
#GC1115_SRCS = gc1115.c cal_coefs.c

#KERNEL MODULE
KER_MODS = 

BUILD = ID_LOCd

######################
# SUFFIXES RULE      #
######################

.SUFFIXES : .o .c
.c.o :
#	$(CC) $(INCS) $(CFLAGS) -c $<
	$(CC) $(INCS) $(CFLAGS) $(SFLAGS) -c $< 

######################
# MAKING RULE        #
######################

.PHONY: all clean
#all : $(KER_MODS) ID_LOCd 
all : $(BUILD)


######################
# RELATION RULE      #
######################

$(COR_OBJS): $($@:.o=.c) ../../common.h Makefile
$(CMD_OBJS): $($@:.o=.c) $($@:.o=.h) mmi.h




$(BUILD): $(ALL_OBJS)
#	$(CC) $(INCS) --static $(CFLAGS) -o $@ $^ $(LIBS) -L. 
#	$(CC) $(INCS) --static $(CFLAGS) -o $@ $^ $(LIBS) -L. $(LDAPLIBS) $(LDAPLIBDIR) $(LBERLIBS) $(LBERLIBSDIR)
#	$(CC) $(INCS) $(CFLAGS) -o $@ $^ $(LIBS) -L. $(LDAPLIBS) $(LBERLIBS) -lssl -lcrypto  -L/usr/lib
	$(CC) $(INCS) $(CFLAGS) -o $@ $^ $(LIBS) -L. $(LDAPLIBS) $(LBERLIBS) 

utils: utils.c
	$(CC) $(INCS) $(CFLAGS) -o $@ utils.c $(LIBS) -L.
#	$(CC) $(INCS) --static $(CFLAGS) -o $@ utils.c $(LIBS) -L.

init: ID_LOC_init_module.c
#	$(CC) -c -g -D__KERNEL__ ID_LOC_init_module.c 
	$(CC) $(KFLAGS) -c -D__KERNEL__ ID_LOC_init_module.c 
	
ldap: ldap_test.c
	$(CC) -o $@ ldap_test.c $(LDAPLIBS) 

ldap2: ldap_test2.c
	$(CC) -Wall -g -I/usr/local/include ldap_test2.c -o ldap_test2 -L/usr/local/lib $(LDAPLIBS) 

mmi: mmi.c
	$(CC) --static $(CFLAGS) -o $@ mmi.c -lpthread -lm -L.

ldap_client: ldap_client.c
	$(CC) $(CFLAGS) -o $@ ldap_client.c -lldap $(LDAPLIBDIR)  
	ls  -al

my_utils: my_utils.o
	$(CC) $(CFLAGS) -o $@ $< -lpthread -lm -L.


evalTrie: evalTrie.o trie.o
	${CPP} ${CFLAGS} -o $@ $^

evalMultiTrie: evalMultiTrie.o multiTrie.o
	${CPP} ${CFLAGS} -o $@ $^

prefixGen: prefixGen.o trie.o
	${CPP} ${CFLAGS} -o $@ $^

trie.o : stdinc.h trie.h
multiTrie.o : stdinc.h multiTrie.h
	${CPP} ${CFLAGS} -c $*.c 
evalTrie.o : stdinc.h trie.h
evalMultiTrie.o : stdinc.h multiTrie.h
	${CPP} ${CFLAGS} -c $*.c 

prefixGen.o : stdinc.h trie.h


	
agent:	
	$(CC) $(INCS) --static $(CFLAGS) $(SFLAGS) -c  $(AGENT_SRCS) 
	ar rs libagent.a $(AGENT_OBJS)
	
#gc1115:	gc1115.c cal_coefs.c
#	$(CC) $(INCS) --static $(CFLAGS) $(SFLAGS) -c  $(GC1115_SRCS) 
	
#ramdisk : tcfr gfm.o 
#	rm -rf ~/rootfsb/root/tcr.final/*
#	cp tcfr gfm.o ~/rootfsb/root/tcr.final
#	su -l -c /home/sangchul/script/makeram.sh
	
clean: 
	rm -rf *.o *~
	rm -rf $(BUILD)

tag:
	rm -rf tags
	find ./ -name '*.[chCH]' |  ctags -R -L -
#	../../make_cstags.sh
