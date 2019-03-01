
#include "cpu_perf_measurement.h"

int main(void)
{

  //unsigned int measuredCpuSpeed = 0;
  tUint32 measuredCpuSpeed = 0;
  measuredCpuSpeed =  taraGetCPUSpeed();
  TARA_MEASURE_FTN_PERF (sleep(1));

  return 0;
}


// # gcc -DTARA_DO_PERF -I. -o cpu_main cpu_main.c -L./.libs/ -lcpu_perf_measurement -Wl,-rpath -Wl,/users/kyehwanl/srx_test1/tm-library/.libs
