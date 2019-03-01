
#include <stdio.h>
#include <execinfo.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#define DEBUG_BACK_TRACE


#if defined (DEBUG_BACK_TRACE)
void print_trace() {
    char pid_buf[30];
    sprintf(pid_buf, "%d", getpid());
    char name_buf[512];
    name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
    int child_pid = fork();
    if (!child_pid) {
        dup2(2,1); // redirect output to stderr
        fprintf(stdout,"stack trace for %s pid=%s\n",name_buf,pid_buf);
        execlp("gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt full", name_buf, pid_buf, NULL);
        abort(); /* If gdb failed to start */
    } else {
        waitpid(child_pid,NULL,0);
    }
}


void bt_sighandler(int sig, struct sigcontext ctx) {

  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;

  if (sig == SIGSEGV)
    printf("Got signal %d, faulty address is %p, "
           "from %p\n", sig, ctx.cr2, ctx.eip);
  else
    printf("Got signal %d\n", sig);

  print_trace();

  trace_size = backtrace(trace, 16);
  /* overwrite sigaction with caller's address */
  trace[1] = (void *)ctx.eip;
  messages = backtrace_symbols(trace, trace_size);
  /* skip first stack frame (points here) */
  printf("[bt] Execution path:\n");

  char name_buf[512];
  name_buf[readlink("/proc/self/exe", name_buf, 511)]=0;
  for (i=1; i<trace_size; ++i)
  {
    printf("[bt] #%d %s\n", i, messages[i]);

    char syscom[256];
    sprintf(syscom,"addr2line %p -e %s ", trace[i], name_buf); //last parameter is the name of this app
    system(syscom);
  }

  exit(0);
}
#endif // (DEBUG_BACK_TRACE)

int main(int argc, char** argv)
{
#if defined (DEBUG_BACK_TRACE)
  struct sigaction sa;
  sa.sa_handler = (void *)bt_sighandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGSEGV, &sa, NULL);
#endif // (DEBUG_BACK_TRACE)

  for (;;)
  {
    pause();
  }
  return 0;
}


