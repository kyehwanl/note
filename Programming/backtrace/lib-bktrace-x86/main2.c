#include<signal.h>
void bt_sighandler(int , struct sigcontext );


int main(int argc, char** argv)
{
  signal(SIGSEGV, (void *)bt_sighandler);

  {
    sleep(10);
    raise(SIGSEGV);
  }
  return 0;
}
