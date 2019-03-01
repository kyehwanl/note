#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <openssl/obj_mac.h>    /* For NID_secp521r1. */
#include <openssl/ecdsa.h>
#include <openssl/ecdh.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
//#include <openssl/ec_lcl.h>
#include <ec_lcl.h>

#define DEFAULT_CERTFILE_EXT  "pub"
#define DEFAULT_KEYFILE_EXT   "private"
#define API_BGPSEC_ALGO_ID_256 1
#define API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256 NID_X9_62_prime256v1
#define API_SUCCESS         0
#define API_FAILURE         -1
#define SIZE_DER_PUBKEY (0x59+2)

char * genPubDer(const char* bin_ecpoint);

/**
 *  print out public key and priv key
 */
static void EC_KEY_print_pub_priv_hex (EC_KEY *ec_key_orig)
{
  // if only priv key field exist
  if ( ((struct ec_key_st*)ec_key_orig)->priv_key)
  {
    printf("\n ----- printing out private key hex ----- \n");
    int i=0;
    for(i=0; i<((struct ec_key_st*)ec_key_orig)->priv_key->dmax; i++)
    {
      printf("%x ", (((struct ec_key_st*)ec_key_orig)->priv_key->d)[i]);
    }
  }
  //printf("\n\n");
  if(ec_key_orig->group && ec_key_orig->pub_key)
  {
    printf("\n ----- printing out pub key hex ----- \n");
    BN_CTX *ctx1;
    ctx1 = BN_CTX_new();
    char * ccc = EC_POINT_point2hex(ec_key_orig->group, ec_key_orig->pub_key,
        POINT_CONVERSION_UNCOMPRESSED/*4*/, ctx1);
    printf("pub: %s\n", ccc);
    printf("\n\n");
  }
}


EVP_PKEY * sca_GetPrivateKey(const char *fn)
{

  char          szFileName[MAXPATHLEN];
  EVP_PKEY      *privateKey = NULL;
  BIO           *bio = BIO_new(BIO_s_file());

  if (!bio)
  {
    printf("+ [libcrypto] Unable to create BIO object\n");
    return NULL;;
  }

  BIO_snprintf(szFileName, sizeof(szFileName)-1, "%s." DEFAULT_KEYFILE_EXT, fn);
  printf(" filename: %s\n", szFileName);

  if (BIO_read_filename(bio, szFileName) == 1)
  {
      privateKey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
      if (!privateKey)
      {
          printf("+ [libcrypto] Unable to load the private key from the bio\n");
          BIO_vfree(bio);
          return NULL;
      }
      printf(" + private key read ok\n");
  }
  else
  {
      BIO_vfree(bio);
      printf( "+ [libcrypto] Error reading a private key from a BIO");
      return NULL;
  }

  (void)BIO_free_all(bio);
  return privateKey;

}

EVP_PKEY * fdGetPrivateKey(const char *fn)
{
  int fd, size_read;
  char          szFileName[MAXPATHLEN];
  char buffer[2048];
  BIO   *bio;// = BIO_new(BIO_s_mem());
  EVP_PKEY      *privateKey = NULL;

#ifdef PEM
  BIO_snprintf(szFileName, sizeof(szFileName)-1, "%s." DEFAULT_KEYFILE_EXT, fn); // pem
#else
  BIO_snprintf(szFileName, sizeof(szFileName)-1, "%s." DEFAULT_KEYFILE_EXT ".der", fn); //  der
#endif
  fd = fopen (szFileName, "rb");

  size_read=fread(buffer,sizeof(char),2048,fd);

#if defined (__DEBUG__)
  int i;
  printf("+ total read byte for private : %d\n", size_read);
  for (i=0; i<size_read; i++)
  {
    if (i%16==0) printf("\n");
    printf("%02x ", buffer[i]&0xff);
  }
  printf("\n\n");
#endif /* __DEBUG__ */

  bio = BIO_new_mem_buf(buffer, size_read);

#ifdef PEM
  privateKey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
#else
  char *p1 = buffer;
  privateKey = d2i_PrivateKey(EVP_PKEY_EC, NULL, &p1, size_read);
#endif

#if 1 // TeMP
  char *p2 = buffer;
  EC_KEY    *tmpEckey;
  tmpEckey = d2i_ECPrivateKey(NULL, (const unsigned char**)&p2, size_read);
#ifdef __DEBUG__
  printf("+ [%s:%d] ***************** print out the private tempec key: \n", __FUNCTION__, __LINE__);
  EC_KEY_print_pub_priv_hex(tmpEckey);
#endif
#endif

  if (!privateKey)
  {
    printf("+  Unable to load the private key from the bio\n");
    BIO_vfree(bio);
    return NULL;
  }
  printf(" + private key read ok\n");

  return privateKey;
}

X509 * sca_GetPublicKey(const char *fn)
{

  char          szFileName[MAXPATHLEN];
  BIO           *bio = BIO_new(BIO_s_file());
  X509          *cert = NULL;

  if (!bio)
  {
    printf ("+ [libcrypto] Unable to create BIO object\n");
    return NULL;;
  }

  /* load the public key */
  BIO_snprintf(szFileName, sizeof(szFileName)-1,
          "%s." DEFAULT_CERTFILE_EXT, fn);

  if (BIO_read_filename(bio, szFileName) <= 0)
  {
      (void)BIO_free_all(bio);
      printf ("+ [libcrypto] Unable to read the key from the bio\n");
      return NULL;
  }

  /* this is ASN.1 format */
  cert = X509_new();
  cert = d2i_X509_bio(bio, &cert);
  if (!cert) {
      printf ("+ [libcrypto] Error occured when loading cert\n");
      (void)BIO_free_all(bio);
      return NULL;
  }

  (void)BIO_free_all(bio);
  return cert;

}

/**
 * from pubkey(der) to internal EVP_PKEY and transform into EC_KEY
 */
EVP_PKEY * getPubKey_der(const char *fn)
{
  int       fd, size_read;
  char      szFileName[MAXPATHLEN];
  char      buffer[2048];
  char      *p1 = buffer;
  EVP_PKEY  *pubKey = NULL;

  BIO_snprintf(szFileName, sizeof(szFileName)-1, "%s." DEFAULT_CERTFILE_EXT "key.der", fn); //  der
  fd = fopen (szFileName, "rb");
  size_read = fread(buffer, sizeof(char), 2048, fd);


#if defined (__DEBUG__)
  int i;
  printf("+ [%s:%d] bin from file:\n", __FUNCTION__, __LINE__);
  for (i=0; i<size_read; i++)
  {
    if (i%16==0) printf("\n");
    printf("%02x ", buffer[i]&0xff);
  }
  printf("\n\n");
#endif /* __DEBUG__ */

  //pubKey = d2i_PublicKey(EVP_PKEY_EC, NULL, &p1, size_read); // this function only deals with RSA public key
  pubKey = d2i_PUBKEY(NULL, &p1, size_read);
  if (!pubKey)
  {
    printf("+  Unable to load the public key from the stream\n");
    return NULL;
  }

  EC_KEY *recon_key = EVP_PKEY_get1_EC_KEY(pubKey);

  printf("+ display  reconstructed EC key\n");
  EC_KEY_print_pub_priv_hex(recon_key);

  printf(" ###################\n");
  /* extract ec point value */
  EC_POINT *ecpoint = EC_KEY_get0_public_key(recon_key);

  BN_CTX *ctx1 = BN_CTX_new();

#ifdef HEX
  char * ccc = EC_POINT_point2hex(recon_key->group, recon_key->pub_key,
        POINT_CONVERSION_UNCOMPRESSED/*4*/, ctx1);
  printf("pub: %s\n", ccc);
#else // BIN
  size_t buf_len = 0;
  unsigned char *buf;

  buf_len = EC_POINT_point2oct(recon_key->group, recon_key->pub_key,
      POINT_CONVERSION_UNCOMPRESSED/*4*/ , NULL, 0, ctx1);
  if (buf_len == 0)
    return NULL;
  if ((buf = OPENSSL_malloc(buf_len)) == NULL)
    return NULL;
  if (!EC_POINT_point2oct(recon_key->group, recon_key->pub_key,
        POINT_CONVERSION_UNCOMPRESSED/*4*/ , buf, buf_len, ctx1))
  {
    OPENSSL_free(buf);
    return NULL;
  }

  printf("+ [%s:%d] bin :\n", __FUNCTION__, __LINE__);
  for (i=0; i<buf_len ; i++)
  {
    if (i%16==0) printf("\n");
    printf("%02x ", buf[i] & 0xff);
  }
  printf("\n\n");

#endif

  /* call generate DER form function */
  genPubDer((const char*)buf);

  return pubKey;
}

/**
 * from EC_KEY to EVP_PKEY and to DER in turn
 */
EVP_PKEY * setPubKey_der(const char *fn, EC_KEY *ec_key)
{
  //EC_KEY    *ec_key;
  EVP_PKEY  *pkey = EVP_PKEY_new();
  char buffer[2048];

  BIO       *out;// = BIO_new(BIO_s_file());
  int       i;

  //BIO_set_fp(out, stdout, BIO_NOCLOSE);

  // set to EVP_PKEY from EC_KEY
  EVP_PKEY_set1_EC_KEY(pkey, ec_key);

  out = BIO_new_mem_buf(buffer, 2048);

  i = i2d_EC_PUBKEY_bio(out, ec_key);

  return pkey;
}


int sca_BgpsecSetEcPrivateKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId)
{

  EVP_PKEY  *privateKey = NULL;
  EC_KEY    *tmpEckey;
  const BIGNUM    *bn;

  if(curveId == API_BGPSEC_ALGO_ID_256)
    curveId = API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256;

#if 1 // get a EVP_PKEY key from a stream
  privateKey = fdGetPrivateKey(filePrefix);
#else
  /* get a private key from a file */
  privateKey =  sca_GetPrivateKey(filePrefix);
#endif

  if(!privateKey)
  {
    printf( "+ [libcrypto] Failed to get the private key from the file\n");
    return API_FAILURE;
  }

  tmpEckey = EVP_PKEY_get1_EC_KEY(privateKey);
  bn = EC_KEY_get0_private_key(tmpEckey);

  setPubKey_der(filePrefix, tmpEckey);
#ifdef __DEBUG__
  printf(" print out the private tempec key: \n");
  EC_KEY_print_pub_priv_hex(tmpEckey);
#endif

  /* fill the received ec_key structure with big_number data of private key */
  //if (!EC_KEY_set_private_key(*ecdsa_key, privateKey->pkey.ec->priv_key))
  if (!EC_KEY_set_private_key(*ecdsa_key, bn))
  {
    printf( "+ [libcrypto] Failed to set the private key into the EC key object\n");
    return API_FAILURE;
  }

  return API_SUCCESS;
}

char * genPubDer(const char* bin_ecpoint)
{
  char arrPubDerHead[26] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86,
    0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
    0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00};

/* 65 byte of EC_POINT value
 *
  04 d6 d3 27 ec 6c f7 05  80 8d cd 80 0f 97 f4 9f
  4a 5a f0 ff 77 10 d7 de  d3 12 8c bd fc 1c 12 59
  92 4a 8b 0a f4 7b ff 6c  35 9f 41 4d e4 0b 4d 1f
  8d 57 9c c1 c0 d6 47 be  34 8e 3e ee 06 d5 9f 29
  f3
*/
  char *pubDer=NULL;
  pubDer = (char*)malloc(SIZE_DER_PUBKEY);
  memcpy(pubDer, (void*)arrPubDerHead, 26); // 26: der header
  memcpy(pubDer+26, bin_ecpoint, 65); // 65: ec point length

#if defined (__DEBUG__)
  int i=0;
  printf("+ [%s] generated PUB DER\n", __FUNCTION__);
  for(i=0; i< SIZE_DER_PUBKEY; i++)
  {
    printf("%02x ", pubDer[i] & 0xff);
    if( i%16 == 15) printf("\n");
  }
  printf("\n");
#endif



  /* test for obtaining EC KEY from pub der stream */
  char      *p1 = pubDer;
  EVP_PKEY  *pubKey = NULL;
  EC_KEY    *recon_key = NULL;

  pubKey = d2i_PUBKEY(NULL, &p1, 0x59+2);
  if (!pubKey)
  {
    printf("+  Unable to load the public key from the stream\n");
    return NULL;
  }
  recon_key = EVP_PKEY_get1_EC_KEY(pubKey);
  printf("+ display  reconstructed EC key\n");
  EC_KEY_print_pub_priv_hex(recon_key);

  return pubDer;
}


int setEcPublicKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId)
{
  char szPubkey[4096];
  size_t pubkeyLen;
  X509          *cert = NULL;
  char *p1 = szPubkey;
  EVP_PKEY      *pubKey = NULL;
  EC_KEY    *tmpEckey;
  EC_POINT      *ecpointPublicKey = NULL;
  EC_GROUP      *ecGroup=NULL;

  memset(szPubkey, 0x00, MAXPATHLEN);
  /* load the public key */
  cert = sca_GetPublicKey(filePrefix);

  curveId = NID_X9_62_prime256v1;
  *ecdsa_key = EC_KEY_new_by_curve_name(curveId);

  memcpy(szPubkey,
      ASN1_STRING_data(cert->cert_info->key->public_key),
      pubkeyLen = ASN1_STRING_length(cert->cert_info->key->public_key));

  genPubDer(szPubkey);

  ecGroup = EC_GROUP_new_by_curve_name(curveId);
  if (NULL == ecGroup) {
    printf("+ Failed to create a EC_GROUP\n");
    return -1;
  }

  ecpointPublicKey = EC_POINT_new(ecGroup);
  if(!ecpointPublicKey)
  {
    printf("+  Failed to create a new EC_POINT and load the public key\n");
    return -1;
  }

  if(!EC_POINT_oct2point(ecGroup, ecpointPublicKey, (const unsigned char *)szPubkey, pubkeyLen, NULL))
  {
    printf("+  Failed to EC_POINT_oct2point to load the public key\n");
    return -1;
  }

  if (! EC_KEY_set_public_key(*ecdsa_key, ecpointPublicKey))
  {
    printf("+  Failed to EC_KEY_set_public_key to load the public key\n");
    return -1;
  }

#if defined (__DEBUG__)
  printf("\n+ [%s:%d] Encoding TEST *** pubkey octet hex [len:%d]\n", __FUNCTION__, __LINE__, pubkeyLen );
  int i=0;
  for(i=0; i< pubkeyLen ; i++)
    printf("%02x ", szPubkey[i] & 0xff);
  printf("\n\n");


  printf("+ printing out pub key hex \n");
  BN_CTX *ctx1;
  ctx1 = BN_CTX_new();
  char * ccc = EC_POINT_point2hex(
      ((struct ec_key_st*)*ecdsa_key)->group,
      ((struct ec_key_st*)*ecdsa_key)->pub_key,
      POINT_CONVERSION_UNCOMPRESSED/*4*/, ctx1);
  printf("pub: %s\n", ccc);
  printf("\n\n");

#endif

#if 0
  pubKey = d2i_PublicKey(EVP_PKEY_EC, NULL, &p1, pubkeyLen);
  if (!pubKey)
  {
    printf("+  Unable to load the public key from the stream\n");
    return NULL;
  }
  printf(" + public key read ok\n");

  tmpEckey = EVP_PKEY_get1_EC_KEY(pubKey);
#endif

#ifdef __DEBUG__
  //printf(" print out the public temp ec key: \n");
  //EC_KEY_print_pub_priv_hex(tmpEckey);
#endif
  return 0;
}

int main( void )
{
  ERR_load_crypto_strings();
  char      filePrefix[MAXPATHLEN] =
    "/users/kyehwanl/proj-bgp/extras/srxcryptoapi/keys//C3/0433/FA1975FF193181458FB902B501EA9789DC";

  EC_KEY    *ec_key=NULL;
  EC_KEY    *ec_key2=NULL;
  ec_key = EC_KEY_new_by_curve_name(API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256);
  //EC_KEY_set_public_key(ec_key, point);

  printf(" ++++++++ [step1] private key processing \n");
  /* private key manipulation */
  sca_BgpsecSetEcPrivateKey(filePrefix, &ec_key, API_BGPSEC_ALGO_ID_256);

  printf(" ++++++++ [step2] public key processing \n");
  /* pub key manipulation */
  setEcPublicKey(filePrefix, &ec_key2, 1);

  printf("+ test for der pub key\n ");
  getPubKey_der(filePrefix);


  //setPubKey_der(filePrefix, ec_key);
  /*
  printf("\n+ result out 1\n");
  EC_KEY_print_pub_priv_hex(ec_key);
  printf("\n+ result out 2\n");
  EC_KEY_print_pub_priv_hex(ec_key2);
  */

  printf(" Program Normally terminated\n");
  return 0;
}

#if 0
 (1) how to compile
gcc -g -O0 -I/usr/local/ssl/include/ -I/users/kyehwanl/Download/bgpsec-test/openssl-1.0.0j/crypto/ec/ -o util_openssl util_openssl.c -L/usr/local/ssl/lib/ -lssl -lcrypto -ldl

#endif



