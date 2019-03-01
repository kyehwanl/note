#include <stdio.h>
#include <stdlib.h>
#include "srxcryptoapi.h"

#include "bgpsec_openssl/bgpsec_openssl.h"

/* common variables declaration */
char            skiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
unsigned char   hashbuff[BGPSEC_MAX_SIG_LENGTH + 10];
unsigned char   sigbuff[BGPSEC_MAX_SIG_LENGTH];
unsigned int    targetAS=0x0000EA62; // 60002
unsigned int    signerAS=0x0000EA61; // 60001
int             sig_length=0;
int             psize = 1; /* prefix: 10.0.0.0/8 */
int             hashLen=0;
unsigned char   pCount = 1;
unsigned char   flags =0;
unsigned char   prefixLen=0x08;

static unsigned char  bgpsec_ski[BGPSEC_SKI_LENGTH] = {
  0xC3, 0x04, 0x33, 0xFA, 0x19, 0x75, 0xFF, 0x19, 0x31, 0x81,
  0x45, 0x8F, 0xB9, 0x02, 0xB5, 0x01, 0xEA, 0x97, 0x89, 0xDC
};
#define LOG_ERR     3   /* informational */
#define LOG_INFO    6   /* informational */
#define LOG_DEBUG   7   /* debug-level messages */

inline void put_u32(void *p, u_int32_t x)
{
  x = htonl(x);
  memcpy(p, &x, 4);
}



/* signing test */
int testSigning(SRxCryptoAPI* crypto)
{
  GEN_SKI_ASCII(skiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);

  put_u32(hashbuff, targetAS);
  put_u32(hashbuff+4, signerAS);
  *(hashbuff+8) = pCount;                        // pCount
  *(hashbuff+9) = flags;                        // Flags
  hashbuff[10] = API_BGPSEC_ALGO_ID_256;    // Algorithm Suite Id
  hashbuff[11] = prefixLen;                      // NLRI length
  hashbuff[12] = 0x0a;                      // NLRI 10.0.0.0/8
  hashLen = 12+psize;

  BGPSecSignData bgpsecSignData = {
    .dataLength       = hashLen,
    .data             = hashbuff,
    //.skiLen           = strlen(skiBuffer),
    //.ski              = skiBuffer,
    //.algoID           = BGPSEC_ALGO_ID,
    .sigLen           = BGPSEC_MAX_SIG_LENGTH,
    .signature        = sigbuff,
  };

  //bgpsecSignData.ski = (u_int8_t *)malloc(BGPSEC_SKI_LENGTH);
  //memcpy(bgpsecSignData.ski, bgpsec_ski, BGPSEC_SKI_LENGTH);

  BGPSecKey   bgpsecKey = {
    //.algoID           = BGPSEC_ALGO_ID,
    .keyLength        = 0,
    .keyData          = NULL,
  };


  /* debugging */
  sca_debugLog(LOG_INFO,"[IN] -- Target AS:%d Origin AS: %d \n", targetAS, signerAS);

  if (IS_DEBUG_ENABLED)
  {
    int i;
    sca_debugLog(LOG_DEBUG,"[IN] ---- HASH recv for Origin [total length: %d] ----\n", hashLen);
    for(i=0; i < hashLen ; i++ )
    {
      if((i+1)%16 ==0) printf("\n");
      printf("%02x ", hashbuff[i]);
    }
    printf("\n");
    char skiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
    GEN_SKI_ASCII(skiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);
    sca_debugLog(LOG_DEBUG,"[IN] ---- current SKI: 0x%s ----\n", skiBuffer);
  }

  /* sign */
  sig_length = crypto->sign_with_key(&bgpsecSignData, &bgpsecKey);

  if (sig_length <= 1)
  {
    sca_debugLog(LOG_ERR,"[IN] bgpsec_sign:%d>%d:o: signing failed", signerAS,targetAS);
    sig_length = 0; /* initialization for later use with another function */
    return -1;
  }

  if (IS_DEBUG_ENABLED)
  {
    int i;
    sca_debugLog(LOG_DEBUG,"[IN] signature print");
    /* signature print out */
    for(i=0; i<sig_length; i++ )
    {
      if(i%16 ==0) printf("\n");
      printf("%02x ", (unsigned char)sigbuff[i]);
    }
    printf("\n");
  }

  return 0;
}

int testSigningWithKeyInfo(SRxCryptoAPI* crypto, BGPSecKey* keyInfo )
{
  GEN_SKI_ASCII(skiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);

  put_u32(hashbuff, targetAS);
  put_u32(hashbuff+4, signerAS);
  *(hashbuff+8) = pCount;                        // pCount
  *(hashbuff+9) = flags;                        // Flags
  hashbuff[10] = API_BGPSEC_ALGO_ID_256;    // Algorithm Suite Id
  hashbuff[11] = prefixLen;                      // NLRI length
  hashbuff[12] = 0x0a;                      // NLRI 10.0.0.0/8
  hashLen = 12+psize;

  BGPSecSignData bgpsecSignData = {
    .dataLength       = hashLen,
    .data             = hashbuff,
    //.skiLen           = strlen(skiBuffer),
    //.ski              = skiBuffer,
    //.algoID           = BGPSEC_ALGO_ID,
    .sigLen           = BGPSEC_MAX_SIG_LENGTH,
    .signature        = sigbuff,
  };

  //bgpsecSignData.ski = (u_int8_t *)malloc(BGPSEC_SKI_LENGTH);
  //memcpy(bgpsecSignData.ski, bgpsec_ski, BGPSEC_SKI_LENGTH);

  /* if null key info, restore key from either function or internal hash */
  //unsigned int id;
  void* fValideKeyInfo = (void*)keyInfo;
  BGPSecKey tmpKey;

  if(fValideKeyInfo == NULL)
  {
    memset(&tmpKey, 0x0, sizeof(BGPSecKey));

    //tmpKey.algoID = 1;
    tmpKey.asn  = signerAS;
    //memcpy(tmpKey.ski, bgpsec_ski, SKI_LENGTH);
    //tmpKey.skiLength = strlen(skiBuffer);

    keyInfo = &tmpKey;

    //printf("[%s:%d] ski:%s length:%d \n", __FUNCTION__, __LINE__, keyInfo->ski, keyInfo->skiLength);

    /* Method 1 : restore from key volt */
    if(sca_loadKey(keyInfo, 1) == 0)
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] (%s:%d) failed to load key \n", __FUNCTION__, __LINE__);
      keyInfo->keyData = NULL;
      keyInfo->keyLength = 0;
    }

    /* Method 2: restore from local key storage */
    //keyInfo = hashKey_get_BGPSecKey(tmpKey.asn, tmpKey.ski);

    if (!keyInfo || !keyInfo->keyData)
    {
      sca_debugLog(LOG_ERR,"(%s:%d) getting Keys error \n", __FUNCTION__, __LINE__);
      return -1;
    }
    printf("[%s:%d] ec key: %p\n", __FUNCTION__, __LINE__, keyInfo->keyData);
  }


  BGPSecKey   bgpsecKey = {
    //.algoID           = keyInfo->algoID,
    .keyLength        = keyInfo->keyLength,
    .keyData          = keyInfo->keyData,
  };


  /* debugging */
  sca_debugLog(LOG_INFO,"-- Target AS:%d Origin AS: %d \n", targetAS, signerAS);

  if (IS_DEBUG_ENABLED)
  {
    int i;
    sca_debugLog(LOG_DEBUG,"-- HASH recv for Origin [total length: %d] ----\n", hashLen);
    for(i=0; i < hashLen ; i++ )
    {
      if((i+1)%16 ==0) printf("\n");
      printf("%02x ", hashbuff[i]);
    }
    printf("\n");
    char skiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
    GEN_SKI_ASCII(skiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);
    sca_debugLog(LOG_DEBUG,"-- current SKI: 0x%s ----\n", skiBuffer);
  }

  /* sign */
  sig_length = crypto->sign_with_key(&bgpsecSignData, &bgpsecKey);

  /* release local tmp structure */
  if(fValideKeyInfo == NULL)
  {
    if(keyInfo->keyData)
      OPENSSL_free(keyInfo->keyData);
  }

  if (sig_length <= 1)
  {
    sca_debugLog(LOG_ERR,"--- bgpsec_sign:%d>%d:o: signing failed", signerAS,targetAS);
    sig_length = 0; /* initialization for later use with another function */
    return -1;
  }

  if (IS_DEBUG_ENABLED)
  {
    int i;
    sca_debugLog(LOG_DEBUG,"-- signature print");
    /* signature print out */
    for(i=0; i<sig_length; i++ )
    {
      if(i%16 ==0) printf("\n");
      printf("%02x ", (unsigned char)sigbuff[i]);
    }
    printf("\n");
  }

  return 0;
}


void testVerify(SRxCryptoAPI* crypto)
{
  int aspathLen=1;
  u_int32_t nlri=0x0a;

  /* prefix */
  struct prefix p;
  p.family = AF_INET;
  p.prefixlen = prefixLen;
  psize = PSIZE (p.prefixlen);
  memcpy (&p.u.prefix, &nlri, psize);

  /*
  struct transfer_data_t trData = {
    .local_as       = targetAS,
    .p              = &p,
  };
  */

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
  seg.flags         = flags;
  seg.next          = NULL;
  sb.sigBlockLen    = sig_length + OCTET_SIG_BLOCK_LEN + OCTET_ALGORITHM_ID + \
                        aspathLen * (BGPSEC_SKI_LENGTH + OCTET_SIGNATURE_LEN);
  sb.algoSuiteId    = BGPSEC_ALGO_ID;
  sb.next           = NULL;
  ss.ski            = bgpsec_ski;
  ss.sigLen         = sig_length;
  ss.signature      = sigbuff;



  /* call api library function */
  //crypto->validate(&bpa, 0, NULL, &trData);

}

void testVerify_withPublicKeyInfo(SRxCryptoAPI* crypto, u_int16_t num_key)
{
  int aspathLen=1;
  u_int32_t nlri=0x0a;

  /* prefix */
  struct prefix p;
  p.family = AF_INET;
  p.prefixlen = prefixLen;
  psize = PSIZE (p.prefixlen);
  memcpy (&p.u.prefix, &nlri, psize);

/*  struct transfer_data_t trData = {
    .local_as       = targetAS,
    .p              = &p,
    .pKeyInfoData   = NULL,
  }; */

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
  seg.flags         = flags;
  seg.next          = NULL;
  sb.sigBlockLen    = sig_length + OCTET_SIG_BLOCK_LEN + OCTET_ALGORITHM_ID + \
                        aspathLen * (BGPSEC_SKI_LENGTH + OCTET_SIGNATURE_LEN);
  sb.algoSuiteId    = BGPSEC_ALGO_ID;
  sb.next           = NULL;
  ss.ski            = bgpsec_ski;
  ss.sigLen         = sig_length;
  ss.signature      = sigbuff;


 // printf("[%s:%d] key Info:%p\n", __FUNCTION__, __LINE__, keyInfo);

  BGPSecKey* outKeyInfo= (BGPSecKey*) malloc(sizeof(BGPSecKey));
  if(!outKeyInfo)
  {
    sca_debugLog(LOG_ERR,"[%s:%d] mem allocation failed\n", __FUNCTION__, __LINE__);
    return ;// NULL;
  }

  //u_int8_t id=0;
  char   tempSkiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
  GEN_SKI_ASCII(tempSkiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);

  printf("[%s:%d] ski:%s len:%d\n", __FUNCTION__, __LINE__, tempSkiBuffer, strlen(tempSkiBuffer));

  outKeyInfo->asn = 60001;
  memcpy(outKeyInfo->ski, bgpsec_ski, BGPSEC_SKI_LENGTH);
  //outKeyInfo->skiLength= strlen(tempSkiBuffer);

  /* load key info */
  if(sca_loadKey(outKeyInfo, 0) == 0) // 0 : public key
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] (%s:%d) failed to load key \n", __FUNCTION__, __LINE__);
    outKeyInfo->keyData = NULL;
    outKeyInfo->keyLength = 0;
  }
  else
  {
    sca_debugLog(LOG_INFO,"[%s:%d] load key success\n", __FUNCTION__, __LINE__);
  }

  sca_debugLog(LOG_DEBUG,"[%s:%d] key data info: (asn:%d) (ski:%02x)\n",
      __FUNCTION__, __LINE__, outKeyInfo->asn, *outKeyInfo->ski);
  sca_debugLog (LOG_DEBUG, "key data :%p length:%d\n",
      (void*)outKeyInfo->keyData, (void*)outKeyInfo->keyLength);


  /* call api library function */
  crypto->validate(&bpa, num_key, &outKeyInfo, &p, targetAS);

  if(outKeyInfo)
  {
    if(outKeyInfo->keyData)
      free(outKeyInfo->keyData);
    free(outKeyInfo);
  }
}

void testVerify_withKeyInfo(SRxCryptoAPI* crypto, BGPSecKey** keyInfo, u_int16_t num_key)
{
  int aspathLen=1;
  u_int32_t nlri=0x0a;

  /* prefix */
  struct prefix p;
  p.family = AF_INET;
  p.prefixlen = prefixLen;
  psize = PSIZE (p.prefixlen);
  memcpy (&p.u.prefix, &nlri, psize);

/*  struct transfer_data_t trData = {
    .local_as       = targetAS,
    .p              = &p,
    .pKeyInfoData   = NULL,
  }; */

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
  seg.flags         = flags;
  seg.next          = NULL;
  sb.sigBlockLen    = sig_length + OCTET_SIG_BLOCK_LEN + OCTET_ALGORITHM_ID + \
                        aspathLen * (BGPSEC_SKI_LENGTH + OCTET_SIGNATURE_LEN);
  sb.algoSuiteId    = BGPSEC_ALGO_ID;
  sb.next           = NULL;
  ss.ski            = bgpsec_ski;
  ss.sigLen         = sig_length;
  ss.signature      = sigbuff;


  printf("[%s:%d] key Info:%p\n", __FUNCTION__, __LINE__, keyInfo);


  //struct KeyInfoData keyInfoData;
  //trData.pKeyInfoData = &keyInfoData;

  u_int32_t keyID=0;
  BGPSecKey *outKeyInfo=NULL;
  char skiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
  GEN_SKI_ASCII(skiBuffer, ss.ski, BGPSEC_SKI_LENGTH);
  //keyInfoData.keyID = keyID = crypto->registerPrivateKey(skiBuffer, strlen(skiBuffer), seg->as, &outKeyInfo);

  /* call api library function */
  crypto->validate(&bpa, num_key, keyInfo, &p, targetAS);

}

BGPSecKey* testLoadPrivateKey(SRxCryptoAPI* crypto)
{
  BGPSecKey* outKeyInfo=NULL;

  unsigned int id=0;
  char   tempSkiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
  GEN_SKI_ASCII(tempSkiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);

  printf("[%s:%d] ski:%s len:%d\n", __FUNCTION__, __LINE__, tempSkiBuffer, strlen(tempSkiBuffer));
  //id = crypto->registerPrivateKey(tempSkiBuffer,strlen(tempSkiBuffer),60002, &outKeyInfo);
  id = crypto->registerPrivateKey(outKeyInfo);

  sca_debugLog(LOG_DEBUG,"[%s] id: %x out key info:%p\n", __FUNCTION__, id,  outKeyInfo);
  sca_debugLog (LOG_DEBUG, "ec key info:%p\n", (void*)outKeyInfo->keyData);

  return outKeyInfo;

}

/**
 * @brief
 *
 * @param crypto
 */
BGPSecKey* testRegisterPrivateKey(SRxCryptoAPI* crypto, u_int8_t *retID)
{

  BGPSecKey* outKeyInfo= (BGPSecKey*) malloc(sizeof(BGPSecKey));
  if(!outKeyInfo)
  {
    sca_debugLog(LOG_ERR,"[%s:%d] mem allocation failed\n", __FUNCTION__, __LINE__);
    return NULL;
  }

  u_int8_t id=0;
  char   tempSkiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
  GEN_SKI_ASCII(tempSkiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);

  printf("[%s:%d] ski:%s len:%d\n", __FUNCTION__, __LINE__, tempSkiBuffer, strlen(tempSkiBuffer));

  outKeyInfo->asn = 60001;
  memcpy(outKeyInfo->ski, bgpsec_ski, BGPSEC_SKI_LENGTH);
  //outKeyInfo->skiLength= strlen(tempSkiBuffer);

  /* load key info */
  if(sca_loadKey(outKeyInfo, 1) == 0)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] (%s:%d) failed to load key \n", __FUNCTION__, __LINE__);
    outKeyInfo->keyData = NULL;
    outKeyInfo->keyLength = 0;
  }
  else
  {
    sca_debugLog(LOG_INFO,"[%s:%d] load key success\n", __FUNCTION__, __LINE__);
  }

  sca_debugLog(LOG_DEBUG,"[%s:%d] key data info: (asn:%d) (ski:%02x)\n",
      __FUNCTION__, __LINE__, outKeyInfo->asn, *outKeyInfo->ski);
  sca_debugLog (LOG_DEBUG, "key data :%p length:%d\n",
      (void*)outKeyInfo->keyData, (void*)outKeyInfo->keyLength);

#if 0
  printf("\n ----- received from loadkey() printing out buf hex ----- \n");
  int i;
  for(i=0; i< outKeyInfo->keyLength; i++)
  {
    printf("%02x ", outKeyInfo->keyData[i]);
  }
  printf("\n\n");
#endif

  id = crypto->registerPrivateKey(outKeyInfo);

  sca_debugLog (LOG_DEBUG, "------ Call again for test of duplication ----------\n");
  id = crypto->registerPrivateKey(outKeyInfo);

  *retID = id;
  return outKeyInfo;
}

int testUnregisterKey(SRxCryptoAPI* crypto, BGPSecKey* keyInfo, u_int8_t keyID)
{
  char   tempSkiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
  GEN_SKI_ASCII(tempSkiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);

  printf("[%s:%d] keyInfo -> keyData: %p keyLength: %d \n",
      __FUNCTION__, __LINE__, keyInfo->keyData, keyInfo->keyLength);
  crypto->unregisterPrivateKey(keyID);

  if(keyInfo)
  {
    if(keyInfo->keyData)
    {
      free(keyInfo->keyData);
      keyInfo->keyData = NULL;
    }

    /* TODO: originally need to release here, but intentionaly defer to free later */
   //free(keyInfo);
  }

  return 0;
}


int testSignWithID(SRxCryptoAPI *crypto, u_int8_t retID)
{
  GEN_SKI_ASCII(skiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);

  put_u32(hashbuff, targetAS);
  put_u32(hashbuff+4, signerAS);
  *(hashbuff+8) = pCount;                        // pCount
  *(hashbuff+9) = flags;                        // Flags
  hashbuff[10] = API_BGPSEC_ALGO_ID_256;    // Algorithm Suite Id
  hashbuff[11] = prefixLen;                      // NLRI length
  hashbuff[12] = 0x0a;                      // NLRI 10.0.0.0/8
  hashLen = 12+psize;

  BGPSecSignData bgpsecSignData = {
    .dataLength       = hashLen,
    .data             = hashbuff,
    //.skiLen           = strlen(skiBuffer),
    //.ski              = skiBuffer,
    //.algoID           = BGPSEC_ALGO_ID,
    .sigLen           = BGPSEC_MAX_SIG_LENGTH,
    .signature        = sigbuff,
  };

  //bgpsecSignData.ski = (u_int8_t *)malloc(BGPSEC_SKI_LENGTH);
  //memcpy(bgpsecSignData.ski, bgpsec_ski, BGPSEC_SKI_LENGTH);

  sca_debugLog(LOG_DEBUG, "retrieved ID:%x \n", retID - RET_ID_OFFSET);

  /* debugging */
  if (IS_DEBUG_ENABLED)
  {
    int i;
    sca_debugLog(LOG_INFO,"[IN] -- Target AS:%d Origin AS: %d \n", targetAS, signerAS);
    sca_debugLog(LOG_DEBUG,"[IN] ---- HASH recv for Origin [total length: %d] ----\n", hashLen);
    for(i=0; i < hashLen ; i++ )
    {
      if((i+1)%16 ==0) printf("\n");
      printf("%02x ", hashbuff[i]);
    }
    printf("\n");
    char skiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
    GEN_SKI_ASCII(skiBuffer, bgpsec_ski, BGPSEC_SKI_LENGTH);
    sca_debugLog(LOG_DEBUG,"[IN] ---- current SKI: 0x%s ----\n", skiBuffer);
  }

  /* sign */
  sig_length = crypto->sign_with_id(&bgpsecSignData, retID);

  if (sig_length <= 1)
  {
    sca_debugLog(LOG_ERR,"[IN] bgpsec_sign:%d>%d:o: signing failed", signerAS,targetAS);
    sig_length = 0; /* initialization for later use with another function */
    return -1;
  }

  if (IS_DEBUG_ENABLED)
  {
    int i;
    sca_debugLog(LOG_DEBUG,"[IN] signature print");
    /* signature print out */
    for(i=0; i<sig_length; i++ )
    {
      if(i%16 ==0) printf("\n");
      printf("%02x ", (unsigned char)sigbuff[i]);
    }
    printf("\n");
  }

  return 0;
}

void testOpenssl(SRxCryptoAPI* crypto)
{

#if 0
  printf("\n\n\n--------------------------------------------------\n");
  sca_debugLog(LOG_DEBUG,"[+] SIGN TEST.....\n");
  printf("--------------------------------------------------\n");
  testSigning(crypto);

  printf("\n\n\n--------------------------------------------------\n");
  sca_debugLog(LOG_DEBUG,"[+] VALIDATION TEST.....\n");
  printf("--------------------------------------------------\n");
  testVerify(crypto);
#endif

#if 0
  printf("\n\n\n--------------------------------------------------\n");
  sca_debugLog(LOG_DEBUG,"[+] REGISTER PRIVATE KEY TEST.....\n");
  printf("--------------------------------------------------\n");

  /* test with the retrieved data */
  printf("\n ++++++++ [ test 1: Register key info ] +++++++ \n\n");
  u_int8_t retKeyID=0;
  BGPSecKey* keyInfo  = testRegisterPrivateKey(crypto, &retKeyID);
  printf("\n\n\n");

  printf("\n ++++++++ [ test 2: Sign with key info ] +++++++ \n\n");
  if(keyInfo)
    testSigningWithKeyInfo(crypto, keyInfo);
  printf("\n\n\n");

  printf("\n ++++++++ [ test 3-1: Verify with key info ] ++++++++++ \n\n");
  if(keyInfo)
    testVerify_withKeyInfo(crypto, &keyInfo, 1); // 1 key
  printf("\n\n\n");
#endif

  printf("\n ++++++++ [ test 3-2: Verify with pub key info ] ++++++++++ \n\n");
  testVerify_withPublicKeyInfo(crypto, 1); // 1 key with using loadKey()
  printf("\n\n\n");

#if 0 // test later
  /* test with NULL data and retrieve key info */
  printf("\n ++++++++ [ test 4: Sign with null key info ] ++++++++++ \n\n");
  BGPSecKey* bsRestoreKeyInfo=NULL;
  testSigningWithKeyInfo(crypto, bsRestoreKeyInfo);
  printf("\n\n\n");

  printf("\n ++++++++ [ test 5: Sign with id ] ++++++++++ \n\n");
  testSignWithID(crypto, retKeyID);
  if(keyInfo)
    testVerify_withKeyInfo(crypto, &keyInfo, 1); // 1 key
  printf("\n\n\n");

#endif

#if 0 // test later
  printf("\n\n\n--------------------------------------------------\n");
  sca_debugLog(LOG_DEBUG,"[+] UNREGISTER KEY TEST.....\n");
  printf("--------------------------------------------------\n");
  testUnregisterKey(crypto, keyInfo, retKeyID);
  if(keyInfo)
    testVerify_withKeyInfo(crypto, &keyInfo, 1); // 1 key

  /* release key info */
  if(keyInfo)
    free(keyInfo);
#endif
}

int main()
{
  SRxCryptoAPI* crypto = malloc(sizeof(SRxCryptoAPI));
  crypto->configFile = "./srxcryptoapi.conf";
  int retVal = 0;
  retVal = srxCryptoInit(crypto);
  int keyID = 0;
  if (retVal)
  {
    printf ("Success!!!\n");
    keyID = crypto->registerPrivateKey(NULL);
    crypto->unregisterPrivateKey(keyID);
    crypto->validate(NULL,0,NULL,NULL,0);
    crypto->sign_with_key(NULL,NULL);
    crypto->sign_with_id(NULL, NULL);
  }
  else
  {
    printf ("Failure!!!\n");
  }

  if(retVal)
  {
    sca_debugLog(LOG_DEBUG,"\n\n[+] bgpsec_openssl test\n");
    testOpenssl(crypto);
  }

  srxCryptoUnbind(crypto);
  free(crypto);
  printf ("done\n");

  return 0;
}


/* compile switches
 * gcc -g -O0 -Wall -I/usr/local/ssl/include -o test_loadKey test_loadKey.c  -ldl -L/usr/local/ssl/lib ./.libs/libSRxCryptoAPI.so -lssl -lcrypto -lconfig -Wl,-rpath -Wl,./.libs/
 */
