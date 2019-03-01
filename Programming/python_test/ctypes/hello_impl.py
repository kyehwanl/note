
from ctypes import cdll
from ctypes import c_char_p

hello_lib = cdll.LoadLibrary("hello.so")
hello = hello_lib.hello
hello.restype = c_char_p



print "from python implementation: " + hello("world")
