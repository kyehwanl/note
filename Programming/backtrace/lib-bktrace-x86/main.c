#include<signal.h>
void bt_sighandler(int , struct sigcontext );


int main(int argc, char** argv)
{
  struct sigaction sa;
  sa.sa_handler = (void *)bt_sighandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGSEGV, &sa, NULL);

  //for (;;)
  {
    //pause();
    sleep(10);
    raise(SIGSEGV);
  }
  return 0;
}
