
#include "perf_test.h"

#define _TIME_MEASURE_ENABLED_
#ifdef _TIME_MEASURE_ENABLED_
/* using perf_get_relative() --> clock_gettime() */
#include <time.h>
#include <sys/resource.h>
/* Relative time, since startup */
static struct timeval relative_time;

static int perf_get_relative(struct timeval *tv)
{
  int ret;
  struct timespec tp;
  if (!(ret = clock_gettime (CLOCK_MONOTONIC, &tp)))
  {
    relative_time.tv_sec = tp.tv_sec;
    relative_time.tv_usec = tp.tv_nsec / 1000;
  }

  if (tv)
    *tv = relative_time;
  return ret;
}

time_t perf_clock(void)  // time_t : long int
{
  struct timeval tv;
  perf_get_relative(&tv);
  return tv.tv_sec;
}

/* using getrusage() */

struct rusage_t
{
  struct rusage cpu;
  struct timeval real;
};
#define RUSAGE_T        struct rusage_t
#define GETRUSAGE(X) thread_getrusage(X)
unsigned long realtime, cputime;
RUSAGE_T before, after;

void
thread_getrusage (RUSAGE_T *r)
{
  perf_get_relative (NULL);

  /* Return resource usage information on process indicated by RUSAGE_SELF(The calling process)
     and put it in r->cpu.  Returns 0 for success, -1 for failure.  */
  getrusage(RUSAGE_SELF, &(r->cpu));

  r->real = relative_time;
}

#define TIMER_SECOND_MICRO 1000000L
static unsigned long
timeval_elapsed (struct timeval a, struct timeval b)
{
  return (((a.tv_sec - b.tv_sec) * TIMER_SECOND_MICRO)
	  + (a.tv_usec - b.tv_usec));
}

unsigned long
thread_consumed_time (RUSAGE_T *now, RUSAGE_T *start, unsigned long *cputime)
{
  /* This is 'user + sys' time.  */
  *cputime = timeval_elapsed (now->cpu.ru_utime, start->cpu.ru_utime) +
	     timeval_elapsed (now->cpu.ru_stime, start->cpu.ru_stime);
  return timeval_elapsed (now->real, start->real);
}

void print_timestamp(char* str)
{
  zlog_info ("%s TASK\t\t: %10lu us (cpu time %10lu us)", str, realtime, cputime);
}
#endif /* _TIME_MEASURE_ENABLED_ */



/* global variable */
SRxCryptoAPI *g_capi;
static int stCount=1;

u_int8_t hashbuff[BGPSEC_MAX_SIG_LENGTH + 10 + BGPSEC_AFI_LENGTH];
u_int8_t sigbuff[BGPSEC_MAX_SIG_LENGTH];
u_int8_t bski[BGPSEC_SKI_LENGTH];
u_int8_t sigLen = 0;
u_int8_t *ski = (u_int8_t*)"C30433FA1975FF193181458FB902B501EA9789DC";
static u_int8_t keyID_priv=0;

static unsigned char  bgpsec_ski[BGPSEC_SKI_LENGTH] = {
    0xC3, 0x04, 0x33, 0xFA, 0x19, 0x75, 0xFF, 0x19, 0x31, 0x81,
    0x45, 0x8F, 0xB9, 0x02, 0xB5, 0x01, 0xEA, 0x97, 0x89, 0xDC
  };
/* attribute setting */
u_int32_t       targetAS = 60003;
u_int32_t       signerAS = 60002;
u_int8_t        pCount   = 1;
u_int8_t        spFlags  = 0x00;

/* prefix: 10.0.0.0/8 */
u_int8_t        prefixLen= 0x08;
u_int32_t       nlri     = 0x0a;




unsigned char hex2bin_byte(unsigned char* in)
{
  unsigned char result=0;
  int i=0;
  for(i=0; i < LEN_BYTE_NIBBLE; i++)
  {
    if(in[i] > 0x40)
      result |= ((in[i] - CHAR_CONV_CONST) & 0x0f) << (4-(i*4));
    else if(in[i] > 0x30 && in[i] < 0x40)
      result |= (in[i] - DIGIT_CONV_CONST) << (4-(i*4));
  }
  return result;
}


/**
 *  Data initialization
 */
void init_handle(void)
{
  bzero(bski, BGPSEC_SKI_LENGTH);

  unsigned int i;
  for(i=0; i< BGPSEC_SKI_LENGTH; i++)
    bski[i] = hex2bin_byte(ski+(i*2));


  g_capi = malloc(sizeof(SRxCryptoAPI));
  g_capi->libHandle = NULL;
  g_capi->configFile = NULL;
  g_capi->registerPrivateKey = NULL;
  g_capi->unregisterPrivateKey = NULL;
  g_capi->sign_with_id = NULL;
  g_capi->sign_with_key = NULL;
  g_capi->validate = NULL;
  g_capi->isExtended = NULL;

  if(srxCryptoInit(g_capi) == 0)
    zlog_err("[BGPSEC] SRxCryptoAPI not initialized!\n");

  if (g_capi->libHandle == NULL)
    zlog_debug("[%s:%d] api handle error", __FUNCTION__, __LINE__);

  zlog_info("[%s:%d] api handle done", __FUNCTION__, __LINE__);
}


/**
 *  key manipulation
 */
void key_loading(u_int32_t keyAS, u_int8_t *keySki, BGPSecKey* outKeyInfo, bool fPrivate)
{
  if(!outKeyInfo)
  {
    printf("Error - NULL key\n");
    exit(1);
  }

  outKeyInfo->algoID        = BGPSEC_ALGO_ID;
  outKeyInfo->asn           = keyAS;
  memcpy(outKeyInfo->ski, keySki, BGPSEC_SKI_LENGTH);


  /* load key info */
  if(fPrivate)
  {
    if(!keyID_priv)
    {
      if(sca_loadKey(outKeyInfo, 1) == 0) // private key
      {
        outKeyInfo->keyData = NULL;
        outKeyInfo->keyLength = 0;
        printf ("private key loading Error");
        exit(1);
      }
      else /* if found, register first time */
       ;// keyID_priv = g_capi->registerPrivateKey(outKeyInfo);
    }
  }
  else
  {
    if(sca_loadKey(outKeyInfo, 0) == 0) // 0 : public key
    {
      //sca_debugLog(LOG_ERR, "+ [libcrypto] (%s:%d) failed to load key \n", __FUNCTION__, __LINE__);
      outKeyInfo->keyData = NULL;
      outKeyInfo->keyLength = 0;
      printf ("public key loading Error");
      exit(1);
    }
    else
    {
      //sca_debugLog(LOG_INFO,"[%s:%d] load key success\n", __FUNCTION__, __LINE__);
    }
  }

}


/**
 *  signing
 */
void signing_test(void)
{

  bool fPrivate = true;
  unsigned int i;
  int psize;

  bzero(hashbuff, (BGPSEC_MAX_SIG_LENGTH + 10));
  bzero(sigbuff, BGPSEC_MAX_SIG_LENGTH);

  /* prefix manipulation */
  struct prefix p;
  p.family = AF_INET;
  p.prefixlen = prefixLen;
  psize = PSIZE (p.prefixlen);
  memcpy (&p.u.prefix, &nlri, psize);


  BGPSecSignData bgpsecSignData = {
    .data             = hashbuff,
    .dataLength       = sizeof(hashbuff),
    .sigLen           = BGPSEC_MAX_SIG_LENGTH,
    .signature        = sigbuff,
  };


  /*
   * key manipulation
   */
  BGPSecKey *outKeyInfo=NULL;
  outKeyInfo = (BGPSecKey*) malloc(sizeof(BGPSecKey));
  memset(outKeyInfo, 0x0, sizeof(BGPSecKey));

#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&before);
#endif /* _TIME_MEASURE_ENABLED_ */
  key_loading(signerAS, bgpsec_ski, outKeyInfo, fPrivate);
#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  zlog_info ("key_loading TASK\t\t: %10lu ms (cpu time %10lu ms)", realtime/1000, cputime/1000);
#endif /* _TIME_MEASURE_ENABLED_ */


#ifdef REGISTER_KEY
  unsigned char keyID_priv=0;
  printf("-- [REGISTER]  out key: %p, pkey pub key:%p(len:%d) \n", \
          outKeyInfo, outKeyInfo->keyData, outKeyInfo->keyLength);
  int j;
  for(j=0; j<outKeyInfo->keyLength; j++)
  {
    printf("%02x ", outKeyInfo->keyData[j]);
    if((j+1)%16==0) printf("\n");
  }
  printf("\n");
  keyID_priv = g_capi->registerPrivateKey(outKeyInfo);
#endif

  /* call srx-crypto-api function */
  sca_generateMSG1(bgpsecSignData.data,(u_int8_t*) &bgpsecSignData.dataLength, targetAS, signerAS,
      pCount, spFlags, BGPSEC_ALGO_ID, AFI_IP, SAFI_UNICAST, p.prefixlen, &p.u.prefix);

  /*
  int ret = g_capi->libHandle == NULL ? 0
    : keyID_priv != 0 ? g_capi->sign_with_id(&bgpsecSignData, keyID_priv)
    : g_capi->sign_with_key(&bgpsecSignData, outKeyInfo);
  */

#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&before);
#endif /* _TIME_MEASURE_ENABLED_ */
  /* signing */
  for(i=0; i<stCount; i++)
    g_capi->sign_with_key(&bgpsecSignData, outKeyInfo);
#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  zlog_info ("SIGNING (%8d run) \t: %10lu ms (cpu time %10lu ms)", stCount, realtime/1000, cputime/1000);
  print_timestamp("---------");
#endif /* _TIME_MEASURE_ENABLED_ */
  sigLen = bgpsecSignData.sigLen;
}


/**
 *  validation
 */
void validation_test(void)
{
  int             aspathLen=1;
  int             psize;
  int             sig_length=0;

  /* prefix manipulation */
  struct prefix p;
  p.family = AF_INET;
  p.prefixlen = prefixLen;
  psize = PSIZE (p.prefixlen);
  memcpy (&p.u.prefix, &nlri, psize);


  /* bgpsec path attribute setting */
  BgpsecPathAttr    bpa;
  PathSegment       seg;
  SigBlock          sb;
  SigSegment        ss;

  memset(&bpa, 0x00, sizeof(BgpsecPathAttr));
  memset(&seg, 0x00, sizeof(PathSegment));
  memset(&sb,  0x00, sizeof(SigBlock));
  memset(&ss,  0x00, sizeof(SigSegment));

  bpa.pathSegments  = &seg;
  bpa.sigBlocks     = &sb;
  sb.sigSegments    = &ss;

  bpa.securePathLen = 0x0008;
  seg.as            = signerAS;
  seg.pCount        = pCount;
  seg.flags         = spFlags;
  seg.next          = NULL;
  sb.sigBlockLen    = sig_length + OCTET_SIG_BLOCK_LEN + OCTET_ALGORITHM_ID + \
                        aspathLen * (BGPSEC_SKI_LENGTH + OCTET_SIGNATURE_LEN);
  sb.algoSuiteId    = BGPSEC_ALGO_ID;
  sb.next           = NULL;
  ss.ski            = bgpsec_ski;
  ss.sigLen         = sigLen;
  ss.signature      = sigbuff;


  /*
   * key manipulation
   */
  bool fPrivate = false;
  BGPSecKey *outKeyInfo=NULL;
  outKeyInfo = (BGPSecKey*) malloc(sizeof(BGPSecKey));
  memset(outKeyInfo, 0x0, sizeof(BGPSecKey));

#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&before);
#endif /* _TIME_MEASURE_ENABLED_ */
  key_loading(signerAS, bgpsec_ski, outKeyInfo, fPrivate);
#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  zlog_info ("key_loading TASK\t\t: %10lu ms (cpu time %10lu ms)", realtime/1000, cputime/1000);
#endif /* _TIME_MEASURE_ENABLED_ */


#ifdef REGISTER_KEY
  unsigned char keyID_pub=0;
  printf("-- [REGISTER]  out key: %p, pkey pub key:%p(len:%d) \n", \
          outKeyInfo, outKeyInfo->keyData, outKeyInfo->keyLength);
  int j;
  for(j=0; j<outKeyInfo->keyLength; j++)
  {
    printf("%02x ", outKeyInfo->keyData[j]);
    if((j+1)%16==0) printf("\n");
  }
  printf("\n");
  keyID_pub = g_capi->registerPublicKey(outKeyInfo);
#endif


#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&before);
#endif /* _TIME_MEASURE_ENABLED_ */
  /* call api library function */
  int i;
  for(i=0; i<stCount; i++)
    g_capi->validate(&bpa, 1, &outKeyInfo, &p, targetAS);
#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  zlog_info ("VALIDATE (%8d run) \t: %10lu ms (cpu time %10lu ms)", stCount, realtime/1000, cputime/1000);
  print_timestamp("---------");
#endif /* _TIME_MEASURE_ENABLED_ */

}


void precision_test(void)
{

#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&before);
#endif /* _TIME_MEASURE_ENABLED_ */

  struct timespec tm = {2, 7400000};
  nanosleep(&tm, NULL);

#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  print_timestamp((char*)__FUNCTION__);
#endif /* _TIME_MEASURE_ENABLED_ */
}

/**
 * @ main
 *
 * @return 0
 */
int main(int argc, char** argv)
{
  // calculation test
  //precision_test();

  if (argc >= 2)
    stCount = atoi(argv[1]);


#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&before);
#endif /* _TIME_MEASURE_ENABLED_ */
  init_handle();
#ifdef _TIME_MEASURE_ENABLED_
  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  print_timestamp("init_handle");
#endif /* _TIME_MEASURE_ENABLED_ */

  signing_test();

  validation_test();


  return 0;
}


/*
 * Time measure library functions
 */
#if 0
#ifdef _TIME_MEASURE_CLOCK_TIME_
  struct timeval start, end;
  perf_get_relative(&start);
  perf_get_relative(&end);
  printf(" *** elapsed time(us):  %lu\n", timeval_elapsed (end, start));
#endif /* _TIME_MEASURE_CLOCK_TIME_*/

#ifdef _TIME_MEASURE_SEC_
  time_t start_time = perf_clock ();
  time_t end_time = perf_clock ();
  printf("+++ TEST1 [perf_test_clock] --  %lu (0x%08x)\n", start_time, start_time);
  printf("+++ TEST1 [perf_test_clock] --  %lu (0x%08x)\n", end_time, end_time);
  printf("+++ TEST1 result : %lu\n", end_time - start_time);
#endif /* _TIME_MEASURE_SEC_*/

#ifdef _TIME_MEASURE_ENABLED_
  unsigned long realtime, cputime;
  RUSAGE_T before, after;
  GETRUSAGE (&before);

  GETRUSAGE (&after);
  realtime = thread_consumed_time (&after, &before, &cputime);
  zlog_info ("SIGNING TASK: ran for %lums (cpu time %lums)", realtime/1000, cputime/1000);
#endif /* _TIME_MEASURE_ENABLED_ */
#endif



