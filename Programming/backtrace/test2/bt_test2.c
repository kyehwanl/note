#include <stdio.h>
#include <signal.h>
#include <execinfo.h>

void bt_sighandler(int sig, struct sigcontext ctx) {

  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;

  if (sig == SIGSEGV)
    printf("Got signal %d, faulty address is %p, "
           "from %p\n", sig, ctx.cr2, ctx.eip);
  else
    printf("Got signal %d\n", sig);

  trace_size = backtrace(trace, 16);
  /* overwrite sigaction with caller's address */
  trace[1] = (void *)ctx.eip;
  messages = backtrace_symbols(trace, trace_size);
  /* skip first stack frame (points here) */
  printf("[bt] Execution path:\n");
  for (i=1; i<trace_size; ++i)
  {
    printf("[bt] #%d %s\n", i, messages[i]);

    char syscom[256];
    //sprintf(syscom,"addr2line %p -e sighandler", trace[i]); //last parameter is the name of this app
    sprintf(syscom,"addr2line %p -e bt_test2", trace[i]); //last parameter is the name of this app
    system(syscom);
  }

  exit(0);
}


int func_a(int a, char b) {

  char *p = (char *)0xdeadbeef;

  a = a + b;
  *p = 10;  /* CRASH here!! */

  return 2*a;
}


int func_b() {

  int res, a = 5;

  res = 5 + func_a(a, 't');

  return res;
}


int main() {

  /* Install our signal handler */
  struct sigaction sa;

  sa.sa_handler = (void *)bt_sighandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  /* ... add any other signal here */

  /* Do something */
  printf("%d\n", func_b());
}


/*

This code should be compiled as: gcc sighandler.c -o sighandler -rdynamic

The program outputs:

Got signal 11, faulty address is 0xdeadbeef, from 0x8048975
[bt] Execution path:
[bt] #1 ./sighandler(func_a+0x1d) [0x8048975]
/home/karl/workspace/stacktrace/sighandler.c:44
[bt] #2 ./sighandler(func_b+0x20) [0x804899f]
/home/karl/workspace/stacktrace/sighandler.c:54
[bt] #3 ./sighandler(main+0x6c) [0x8048a16]
/home/karl/workspace/stacktrace/sighandler.c:74
[bt] #4 /lib/tls/i686/cmov/libc.so.6(__libc_start_main+0xe6) [0x3fdbd6]
??:0
[bt] #5 ./sighandler() [0x8048781]
??:0


*/
