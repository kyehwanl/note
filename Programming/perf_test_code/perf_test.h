
/* perf_test.h */
#ifndef _PERF_TESTL_H
#define _PERF_TESTL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "srxcryptoapi.h"

#define BGPSEC_SKI_LENGTH           20
#define BGPSEC_ALGO_ID              1
#define BGPSEC_MAX_SIG_LENGTH       128
#define BGPSEC_AFI_LENGTH           1
#define AFI_IP                    1
#define SAFI_UNICAST              1

unsigned long term_bgp_debug_bgpsec = 0x07;
#define BGP_DEBUG_BGPSEC_DETAIL       0x01
#define BGP_DEBUG_BGPSEC              0x01 // alias of the bgpsec detail
#define BGP_DEBUG_BGPSEC_IN           0x02
#define BGP_DEBUG_BGPSEC_OUT          0x04
#define BGP_DEBUG(a, b)		(term_bgp_debug_ ## a & BGP_DEBUG_ ## b)


#define CHAR_CONV_CONST     0x37
#define DIGIT_CONV_CONST    0x30
#define LEN_BYTE_NIBBLE     0x02

#define BGPSEC_SKI_LENGTH           20
#define OCTET_SIG_BLOCK_LEN         2
#define OCTET_ALGORITHM_ID          1
#define OCTET_SIG_BLOCK_LEN         2
#define OCTET_SIGNATURE_LEN         2



/* function declarations */
unsigned char hex2bin_byte(unsigned char* in);


/* global variables */
int master;





#define PSIZE(a) (((a) + 7) / (8))



#include <netinet/in.h>
struct prefix
{
  u_char family;
  u_char prefixlen;
  union
  {
    u_char prefix;
    struct in_addr prefix4;
#ifdef HAVE_IPV6
    struct in6_addr prefix6;
#endif /* HAVE_IPV6 */
    struct
    {
      struct in_addr id;
      struct in_addr adv_router;
    } lp;
    u_char val[8];
  } u __attribute__ ((aligned (8)));
};





#endif /* _PERF_TESTL_H*/
