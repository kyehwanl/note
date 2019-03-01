BUILD = is_free

all:
	gcc -O0 -g -o $(BUILD) $(BUILD).c 
	#gcc -Os -o util_main util_main.c -L./.libs/ -ltm_util -lrt -Wl,-rpath -Wl,${PWD}/.libs

clean:
	rm -rf *.o *~
	rm -rf $(BUILD)


