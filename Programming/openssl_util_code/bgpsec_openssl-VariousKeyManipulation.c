#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <stdbool.h>


/* general API header which will be public to the customer side */
#include "bgpsec_openssl/bgpsec_openssl.h"

int static stBgpsecReqSigning(BGPSecSignData*, BGPSecKey *);
int static stcl_BgpsecVerifySignatureSSL (BGPSecSignData*);

Record_t *g_records = NULL;

struct Enbuf_Eckey_t
{
  void  *en_buf;
  void  *ec_key;
  int   id;
};
static struct Enbuf_Eckey_t  stEnbuf_Eckey[ID_NUM_MAX];

/* global variable for containing strings which indicates the key location */

// Foward declaration
int cl_SignParamWithKeySanityCheck(BGPSecSignData *signData, BGPSecKey *key);
int cl_SignParamWithIDSanityCheck(BGPSecSignData *signData, u_int8_t keyID);
void PrintPrivPubKeyHex(EC_KEY* ec_key);

/**
 * Compare both given SKI Arrays. The arrays MUST be of lendth SKI_LENGTH.
 *
 * @param arr1 The first array to compare
 * @param arr2 The second array to compare.
 *
 * @return true if the arrays contain the same byte information,
 *              otherwise false.
 */
static bool cmpSKI(u_int8_t* arr1, u_int8_t* arr2)
{
  bool retVal = true;
  int idx = 0;
  for (idx = 0; idx < SKI_LENGTH; idx++)
  {
    if (arr1[idx] != arr2[idx])
    {
      retVal = false;
      break;
    }
  }
  return retVal;
}

/**
 * @brief
 *
 * @param digest
 * @param digest_len
 * @param ecdsa_key
 * @param signature_algorithm
 * @param signature
 * @param signature_len
 *
 * @return
 */

int cl_BgpsecDoVerifySignature (u_int8_t *digest, int digest_len,
                                      EC_KEY    *eckey_nistp256,
                                      int signature_algorithm,
                                      u_int8_t *signature, int signature_len)
{
    if (IS_DEBUG_ENABLED)
    {
      int i;
      printf("Received signatgure: 0x:");
      for(i=0; i< signature_len; i++)
      {
        printf("%02x ", signature[i]);
      }
      printf("\n");
    }

    if(! ECDSA_verify(0, digest, digest_len, signature, signature_len, eckey_nistp256))
    {
    /* error return handling */
      //SSL_load_error_strings();
      sca_debugLog(LOG_ERR, "+ [libcrypto] ECDSA_verify error: %s\n", \
          ERR_error_string(ERR_get_error(), NULL));
      ERR_print_errors_fp(stderr);
      goto verify_err;
    }
    else
      return API_BGPSEC_VERIFY_SUCCESS;


// openssl clean up needs to be done
verify_err:
  //if (eckey_nistp256)
    //EC_KEY_free(eckey_nistp256);

  //if(ecdsa_signature)
    //ECDSA_SIG_free(ecdsa_signature);

  return API_BGPSEC_VERIFY_FAILURE ; /* error return */

}


/**
 * @brief
 *
 * @param sslVerifyData
 *
 * @return
 */
int stcl_BgpsecVerifySignatureSSL (BGPSecSignData* sslVerifyData)
{
    EC_KEY  *ecdsa_key=NULL;
    int status;

    char szFileName[MAXPATHLEN];
    if (!sca_FindDirInSKI(szFileName, sizeof(szFileName), sslVerifyData->ski))
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] failed to find a key filename from a ski\n");
      status = -1;
      goto err_cleanup;
    }

    if (API_BGPSEC_SUCCESS !=
        cl_BgpsecSetEcPublicKey(szFileName, &ecdsa_key, API_BGPSEC_DEFAULT_CURVE))
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to set a bgpsec ecdsa key \n");
      status = -1;
      goto err_cleanup;
    }


    unsigned char md[SHA256_DIGEST_LENGTH];
    cl_BgpsecOctetDigest(sslVerifyData->data, sslVerifyData->dataLength, md);
    status = cl_BgpsecDoVerifySignature (md, SHA256_DIGEST_LENGTH,
                                       ecdsa_key,
                                       sslVerifyData->algoID,
                                       sslVerifyData->signature,
                                       sslVerifyData->sigLen);

    if(status != API_BGPSEC_VERIFY_SUCCESS)
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] Error at verifying\n");
      status = API_BGPSEC_VERIFY_FAILURE;
      goto err_cleanup;
    }
    else
      return status;

err_cleanup:
    // openssl cleanup
    if (ecdsa_key) EC_KEY_free(ecdsa_key);
    //
    // CRYPTO_cleanup_all_ex_data();
    // ERR_free_strings();
    // ERR_remove_state(0);
    return status;
}

int stcl_BgpsecVerifySignatureSSL_withKeyInfo (BGPSecSignData* sslVerifyData,
                BGPSecKey** keys, u_int16_t keyCnt)
{
#define USE_EC_PRIVATE
#define USE_EC_PUB_EVP_PKEY
#undef  USE_EC_PUB_OCTET_KEY
#undef  USE_EC_TYPE2PARAM
    EC_KEY  *ecdsa_key=NULL;
    int status = -1;
    int bDerPrivate=0;
    int retType=0;
    unsigned char *p = NULL;

    //char skiBuffer[sslVerifyData->skiLen * 2 + 1];
    //GEN_SKI_ASCII(skiBuffer, sslVerifyData->ski, sslVerifyData->skiLen);

    /* insert one of keys into EC_KEY pointer */
    if(keys && keys[keyCnt])
    {
      p = keys[keyCnt]->keyData;
      if(!p)
      {
        sca_debugLog(LOG_ERR, "keyData is NULL, verification cancel\n");
        goto err_cleanup;
      }

      /* determine whether the given key is DER or not and private or public */
      retType = IS_DER_PRIVATE(p, keys[keyCnt]->keyLength);
      if(retType == 1)
      {
        bDerPrivate = 1;
        sca_debugLog(LOG_INFO, "[%s:%d] Private key in DER\n", __FUNCTION__, __LINE__);
      }
      else if(retType == 0)
        sca_debugLog(LOG_INFO, "[%s:%d] Public key in DER\n", __FUNCTION__, __LINE__);
      else if(retType < 0)
        sca_debugLog(LOG_INFO, "[%s:%d] key is not DER format \n", __FUNCTION__, __LINE__);

/* convert DER to Internal */
#if defined (USE_EC_PRIVATE)
      BIO *out_err = BIO_new_fp(stderr, BIO_NOCLOSE);

      if(bDerPrivate)
      {
        ecdsa_key = d2i_ECPrivateKey(NULL, (const unsigned char**)&p, keys[keyCnt]->keyLength);
        if (!ecdsa_key )
        {
          ECerr(EC_F_ECKEY_PRIV_DECODE, EC_R_DECODE_ERROR);
          ERR_print_errors(out_err);
          sca_debugLog(LOG_ERR, " EC priv key failed \n");
          goto err_cleanup;
        }
      }
      else
      {
        EVP_PKEY *pkey_tmp;
        int eplen = keys[keyCnt]->keyLength;

        /* We have parameters now set public key */
        pkey_tmp = d2i_PUBKEY(NULL, (const unsigned char**)&p, eplen);
        if (!pkey_tmp)
        {
          ECerr(EC_F_ECKEY_PUB_DECODE, EC_R_DECODE_ERROR);
          ERR_print_errors(out_err);
          sca_debugLog(LOG_ERR, " PUBKEY pub key failed \n");
          goto err_cleanup;
        }

        ecdsa_key = EVP_PKEY_get1_EC_KEY(pkey_tmp);
        EVP_PKEY_free(pkey_tmp);

        if(ecdsa_key == NULL)
        {
          sca_debugLog(LOG_ERR, " fail to create a new eckey from EVP_PKEY \n");
          goto err_cleanup;
        }

      }
#else /* USE_EC_PRIVATE */


#if defined (USE_EC_TYPE2PARAM)
      ASN1_STRING *pstr = p;
      int pmlen = keys[keyCnt]->keyLength;

      //EC_KEY *ec_key = NULL;
      if (!(ecdsa_key = d2i_ECParameters(NULL, &p, pmlen)))
      {
        sca_debugLog(LOG_ERR, " fail to d2i_ECParameters() \n");
        ECerr(EC_F_ECKEY_TYPE2PARAM, EC_R_DECODE_ERROR);
        goto err_cleanup;
      }

      /* We have parameters now set public key */
      if (!o2i_ECPublicKey(&ecdsa_key, &p, pmlen))
      {
        sca_debugLog(LOG_ERR, " fail to o2i_ECPublicKey()  \n");
        ECerr(EC_F_ECKEY_PUB_DECODE, EC_R_DECODE_ERROR);
        goto err_cleanup;
      }
#elif defined (USE_EC_PUB_EVP_PKEY)
      EVP_PKEY *pkey_tmp;
      BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
      int eplen = keys[keyCnt]->keyLength;

      /* We have parameters now set public key */
      pkey_tmp = d2i_PUBKEY(NULL, &p, eplen);
      if (!pkey_tmp)
      {
        ECerr(EC_F_ECKEY_PUB_DECODE, EC_R_DECODE_ERROR);
        ERR_print_errors(out);
        sca_debugLog(LOG_ERR, " PUBKEY pub key failed \n");
        goto err_cleanup;
      }

      ecdsa_key = EVP_PKEY_get1_EC_KEY(pkey_tmp);
      EVP_PKEY_free(pkey_tmp);

      if(ecdsa_key == NULL)
      {
        sca_debugLog(LOG_ERR, " fail to create a new eckey from EVP_PKEY [%s:%d] \n",
            __FUNCTION__, __LINE__);
        goto err_cleanup;
      }
/* convert Octet to Internal */
#elif defined (USE_EC_PUB_OCTET_KEY)
      ecdsa_key = EC_KEY_new();
      if(ecdsa_key==NULL)
      {
        sca_debugLog(LOG_ERR, " fail to create a new eckey \n");
        goto err_cleanup;
      }

      EC_GROUP * ecgroup = EC_GROUP_new_by_curve_name(API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256);
      if(ecgroup == NULL)
      {
        sca_debugLog(LOG_ERR, " fail to create a new group \n");
        goto err_cleanup;
      }

      int setGroupStatus = EC_KEY_set_group(ecdsa_key, ecgroup);
      if(!setGroupStatus)
      {
        sca_debugLog(LOG_ERR, " fail to set group for ec key \n");
        goto err_cleanup;
      }

      /* We have parameters now set public key */
      if (!o2i_ECPublicKey(&ecdsa_key, &p, keys[keyCnt]->keyLength))
      {
        ECerr(EC_F_ECKEY_PUB_DECODE, EC_R_DECODE_ERROR);
        sca_debugLog(LOG_ERR, " EC pub key failed \n");
        goto err_cleanup;
      }
#endif
      if (IS_DEBUG_ENABLED)
        PrintPrivPubKeyHex(ecdsa_key);

#endif /* USE_EC_PRIVATE */

      /* check public key */
      if(EC_KEY_get0_public_key(ecdsa_key) == NULL)
      {
        ECerr(EC_F_ECKEY_PUB_DECODE, ERR_R_EC_LIB);
        sca_debugLog(LOG_ERR, "EC_KEY_get0_public_key failed \n");
        goto err_cleanup;
      }
    }
    else
      goto err_cleanup;

    /* check validity of ec key */
    if (!EC_KEY_check_key(ecdsa_key)) {
      status = -1;
      sca_debugLog(LOG_ERR, "+ [libcrypto] EC_KEY_check failed: EC key check error \
          [current num:%d, ski:%02x, ASN:%d] \n",\
          keyCnt, *keys[keyCnt]->ski, keys[keyCnt]->asn);
      goto err_cleanup;
    }

    sca_debugLog(LOG_INFO, "+ [%s:%d] EC_KEY_check [Current Num:%d, ski:%02x, ASN:%d] --- OK \n",
        __FUNCTION__, __LINE__, keyCnt, *keys[keyCnt]->ski, keys[keyCnt]->asn);

    /* message digest and verification */
    unsigned char md[SHA256_DIGEST_LENGTH];
    cl_BgpsecOctetDigest(sslVerifyData->data, sslVerifyData->dataLength, md);
    status = cl_BgpsecDoVerifySignature (md, SHA256_DIGEST_LENGTH,
                                       ecdsa_key,
                                       sslVerifyData->algoID,
                                       sslVerifyData->signature,
                                       sslVerifyData->sigLen);

    if(status != API_BGPSEC_VERIFY_SUCCESS)
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] Error at verifying\n");
      status = API_BGPSEC_VERIFY_FAILURE;
      goto err_cleanup;
    }
    else
    {
      if (ecdsa_key) EC_KEY_free(ecdsa_key);
      return status;
    }

err_cleanup:
    // openssl cleanup
    if (ecdsa_key) EC_KEY_free(ecdsa_key);
    //
    // CRYPTO_cleanup_all_ex_data();
    // ERR_free_strings();
    // ERR_remove_state(0);
    return status;
}

EVP_PKEY * cl_GetPrivateKey(const char *fn)
{

  char          szFileName[MAXPATHLEN];
  EVP_PKEY      *privateKey = NULL;
  BIO           *bio = BIO_new(BIO_s_file());

  if (!bio)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Unable to create BIO object\n");
    return NULL;;
  }

  BIO_snprintf(szFileName, sizeof(szFileName)-1, "%s." DEFAULT_KEYFILE_EXT, fn);

  if (BIO_read_filename(bio, szFileName) == 1)
  {
      privateKey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
      if (!privateKey)
      {
          sca_debugLog(LOG_ERR, "+ [libcrypto] Unable to load the private key from the bio\n");
          BIO_vfree(bio);
          return NULL;
      }
  }
  else
  {
      BIO_vfree(bio);
      sca_debugLog(LOG_ERR, "+ [libcrypto] Error reading a private key from a BIO");
      return NULL;
  }

  (void)BIO_free_all(bio);
  return privateKey;

}

X509 * cl_GetPublicKey(const char *fn)
{

  char          szFileName[MAXPATHLEN];
  BIO           *bio = BIO_new(BIO_s_file());
  X509          *cert = NULL;

  if (!bio)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Unable to create BIO object\n");
    return NULL;;
  }

  /* load the public key */
  BIO_snprintf(szFileName, sizeof(szFileName)-1,
          "%s." DEFAULT_CERTFILE_EXT, fn);

  if (BIO_read_filename(bio, szFileName) <= 0)
  {
      (void)BIO_free_all(bio);
      sca_debugLog(LOG_ERR, "+ [libcrypto] Unable to read the key from the bio\n");
      return NULL;
  }

  /* this is ASN.1 format */
  cert = X509_new();
  cert = d2i_X509_bio(bio, &cert);
  if (!cert) {
      sca_debugLog(LOG_ERR, "+ [libcrypto] Error occured when loading cert\n");
      (void)BIO_free_all(bio);
      return NULL;
  }

  (void)BIO_free_all(bio);
  return cert;

}

/**
 * @brief Load the private key
 *
 * @param filePrefix
 * @param ecdsa_key
 *
 * @return
 */
int cl_BgpsecSetEcPrivateKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId)
{

  EVP_PKEY      *privateKey = NULL;

  if(curveId == API_BGPSEC_ALGO_ID_256)
    curveId = API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256;

  /* get a private key from a file */
  privateKey =  cl_GetPrivateKey(filePrefix);

  if(!privateKey)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to get the private key from the file\n");
    return API_BGPSEC_FAILURE;
  }

  if (!EC_KEY_set_private_key(*ecdsa_key, privateKey->pkey.ec->priv_key))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to set the private key into the EC key object\n");
    return API_BGPSEC_FAILURE;
  }

  sca_debugLog(LOG_INFO, "+ [libcrypto] Private key verified OK\n");
  return API_BGPSEC_SUCCESS;
}


int cl_BgpsecSetEcPublicKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId)
{
  //char szFileName[MAXPATHLEN];
  char szPubkey[4096];
  size_t pubkeyLen;

  EC_POINT      *ecpointPublicKey = NULL;
  EC_GROUP      *ecGroup=NULL;
  X509          *cert = NULL;

  memset(szPubkey, 0x00, MAXPATHLEN);

  /* load the public key */
  cert = cl_GetPublicKey(filePrefix);
  if (cert == NULL)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to get a key\n");
    goto err_cleanup;
  }

  if(curveId == API_BGPSEC_ALGO_ID_256)
    curveId = API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256;

  /* create the basic key structure with EC_POINT */
  *ecdsa_key = EC_KEY_new_by_curve_name(curveId);
  ecGroup = EC_GROUP_new_by_curve_name(curveId);

  if (NULL == ecGroup) {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to create a EC_GROUP\n");
    goto err_cleanup;
  }

  memcpy(szPubkey,
      ASN1_STRING_data(cert->cert_info->key->public_key),
      pubkeyLen = ASN1_STRING_length(cert->cert_info->key->public_key));

  ecpointPublicKey = EC_POINT_new(ecGroup);
  if (!ecpointPublicKey
      || !EC_POINT_oct2point(ecGroup, ecpointPublicKey, (const unsigned char*)szPubkey, pubkeyLen, NULL)
      || !EC_KEY_set_public_key(*ecdsa_key, ecpointPublicKey))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to create a new EC_POINT and load the public key\n");
    if(ecpointPublicKey)
      EC_POINT_free(ecpointPublicKey);
    goto err_cleanup;
  }

  if (!EC_KEY_check_key(*ecdsa_key))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] %s\n",ERR_error_string(ERR_get_error(),NULL));
    goto err_cleanup;
  }
  else
  {
    ;
    sca_debugLog(LOG_INFO, "+ [libcrypto] Public key verified OK\n");
  }

  return API_BGPSEC_SUCCESS;

// openssl cleanup
err_cleanup:
  if (*ecdsa_key)  EC_KEY_free(*ecdsa_key);
  if (ecGroup) EC_GROUP_free(ecGroup);
  if (cert) X509_free(cert);
  //
  // CRYPTO_cleanup_all_ex_data();
  // ERR_free_strings();
  // ERR_remove_state(0);
  return API_BGPSEC_FAILURE;
}





/**
 * @brief
 *
 * @param digest
 * @param digest_len
 * @param ecdsa_key
 * @param sigAlgoID
 * @param signature
 * @param signature_len
 *
 * @return signature length or -1
 */
int cl_BgpsecECDSA_Sign (u_int8_t *digest, int digest_len,
                               EC_KEY    *ecdsa_key,
                               u_int8_t *signature, int signature_len)
{

  int length=0;
  unsigned int sig_len;
  BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);

  sig_len = ECDSA_size(ecdsa_key);

  sca_debugLog(LOG_INFO, " + [libcrypto] sig length: %d \n", sig_len);

  if(!ECDSA_sign(0, digest, digest_len, signature, &sig_len, ecdsa_key))
  {
    /* error */
    sca_debugLog(LOG_ERR, "+ [libcrypto] ECDSA_sing error: %s\n", \
        ERR_error_string(ERR_get_error(), NULL));
    ERR_print_errors_fp(stderr);
    goto int_err;
  }

  length = signature_len = sig_len;

  if (IS_DEBUG_ENABLED)
  {
    int i;
    printf("Genereated Signatgure: 0x:");
    for(i=0; i< sig_len; i++)
    {
      printf("%02x ", signature[i]);
    }
    printf("\n");
  }

  /* verify the signature */
  if (ECDSA_verify(0, digest, digest_len, signature, sig_len, ecdsa_key) != 1)
    goto int_err;

  return length;

int_err:
  if (!length)
  {
    BIO_printf(out, " failed\n");
    sca_debugLog(LOG_ERR, " + [libcrypto] Error at signing\n");
  }
  if (ecdsa_key)
    EC_KEY_free(ecdsa_key);
  return length;
}


/**
 * @brief static function performing to check the keys and call the openssl signing functions
 *
 * @param sslSignData
 *
 * @return signature length or -1
 */
int static stBgpsecReqSigning(BGPSecSignData *sslSignData, BGPSecKey  *bgpsecKey)
{
  if(cl_SignParamSanityCheck(sslSignData, bgpsecKey) != 0)
  {
    sca_debugLog(LOG_ERR,"%s:[OUT] bgpsec Sign data or Key data structure error\n", __FUNCTION__);
    return -1;
  }

  EC_KEY    *ecdsa_key=NULL;
  char szFileName[MAXPATHLEN];
  int ret=0;

  if (!sca_FindDirInSKI(szFileName, sizeof(szFileName),sslSignData->ski))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] failed to generate a file name from a ski\n");
  }

  if (API_BGPSEC_SUCCESS !=
        cl_BgpsecSetEcPublicKey(szFileName, &ecdsa_key, API_BGPSEC_DEFAULT_CURVE) ||
        cl_BgpsecSetEcPrivateKey(szFileName, &ecdsa_key, API_BGPSEC_ALGO_ID_256))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to load a bgpsec key from an SKI\n");
    ret = -1;
    goto int_err;
  }

  if (!EC_KEY_check_key(ecdsa_key)) {
        sca_debugLog(LOG_ERR, "+ [libcrypto] EC_KEY_check failed: EC key check error\n");
        goto int_err;
  }

  sca_debugLog(LOG_INFO, "+ [libcrypto] successfully finished to load bgpsec load key\n");

  unsigned char md[SHA256_DIGEST_LENGTH];
  switch (sslSignData->algoID )
  {
    case API_BGPSEC_ALGO_ID_256 :
      cl_BgpsecOctetDigest(sslSignData->data, sslSignData->dataLength, md);

      ret = cl_BgpsecECDSA_Sign (md, SHA256_DIGEST_LENGTH, ecdsa_key,
                          sslSignData->signature, sslSignData->sigLen);
      break;

    /* in case other id algorithm */
    default:
      break;
  }
  return ret;

// openssl cleanup
int_err:
  if (ecdsa_key) EC_KEY_free(ecdsa_key);
  //
  // CRYPTO_cleanup_all_ex_data();
  // ERR_free_strings();
  // ERR_remove_state(0);
  return ret;
}

/**
 * @brief  Signing requrest combined with getting key function
 *
 * @param sslSignData
 * @param bgpsecKey
 *
 * @return
 */
int static stBgpsecReqSigningWithKeyInfo(BGPSecSignData *sslSignData, BGPSecKey  *bgpsecKey)
{
  int ret=0;
  if(cl_SignParamWithKeySanityCheck(sslSignData, bgpsecKey) != 0)
  {
    sca_debugLog(LOG_ERR,"%s:[OUT] bgpsec Sign data or Key data structure error\n", __FUNCTION__);
    return -1;
  }

  EC_KEY    *ecdsa_key;
  unsigned char *p = NULL;
  p = bgpsecKey->keyData;

  ecdsa_key = d2i_ECPrivateKey(NULL, (const unsigned char**)&p, bgpsecKey->keyLength);

  if (IS_DEBUG_ENABLED)
    PrintPrivPubKeyHex(ecdsa_key);

  sca_debugLog(LOG_INFO, "+ [libcrypto] [%s:%d] ec key:%p\n", __FUNCTION__, __LINE__, bgpsecKey->keyData);


  if (!EC_KEY_check_key(ecdsa_key)) {
        sca_debugLog(LOG_ERR, "+ [libcrypto][%s] EC_KEY_check failed: EC key check error\n", __FUNCTION__);
        goto int_err;
  }

  sca_debugLog(LOG_INFO, "+ [libcrypto] [%s] successfully finished to load bgpsec load key\n", __FUNCTION__);

  unsigned char md[SHA256_DIGEST_LENGTH];
  switch (sslSignData->algoID )
  {
    case API_BGPSEC_ALGO_ID_256 :
      cl_BgpsecOctetDigest(sslSignData->data, sslSignData->dataLength, md);

      ret = cl_BgpsecECDSA_Sign (md, SHA256_DIGEST_LENGTH, ecdsa_key,
                          sslSignData->signature, sslSignData->sigLen);
      break;

    /* in case other id algorithm */
    default:
      break;
  }
  if (ecdsa_key) EC_KEY_free(ecdsa_key);
  return ret;

// openssl cleanup
int_err:
  //if (ecdsa_key) EC_KEY_free(ecdsa_key);
  //
  // CRYPTO_cleanup_all_ex_data();
  // ERR_free_strings();
  // ERR_remove_state(0);
  return ret;
}


int stBgpsecReqSigningWithID(BGPSecSignData *sslSignData, u_int8_t inID)
{
  int ret=0;
  u_int8_t keyID=0;
  EC_KEY    *ecdsa_key = NULL;

  if(cl_SignParamWithIDSanityCheck(sslSignData, inID) != 0)
  {
    sca_debugLog(LOG_ERR,"%s:[OUT] bgpsec Sign data or key ID data error\n", __FUNCTION__);
    return -1;
  }

  keyID = inID - RET_ID_OFFSET;

  /* retrieve eckey from key id */
  sca_debugLog(LOG_INFO, "key ID:%x \n", keyID);
  ecdsa_key = stEnbuf_Eckey[keyID].ec_key;

  sca_debugLog(LOG_INFO, "+ [libcrypto] [%s:%d] ec key:%p\n", __FUNCTION__, __LINE__, ecdsa_key);

  if (!EC_KEY_check_key(ecdsa_key)) {
        sca_debugLog(LOG_ERR, "+ [libcrypto][%s] EC_KEY_check failed: EC key check error\n", __FUNCTION__);
        goto int_err;
  }

  sca_debugLog(LOG_INFO, "+ [libcrypto] [%s] successfully finished to load bgpsec load key\n", __FUNCTION__);

  unsigned char md[SHA256_DIGEST_LENGTH];
  switch (sslSignData->algoID )
  {
    case API_BGPSEC_ALGO_ID_256 :
      cl_BgpsecOctetDigest(sslSignData->data, sslSignData->dataLength, md);

      ret = cl_BgpsecECDSA_Sign (md, SHA256_DIGEST_LENGTH, ecdsa_key,
                          sslSignData->signature, sslSignData->sigLen);
      break;

    /* in case other id algorithm */
    default:
      break;
  }
  return ret;

// openssl cleanup
int_err:
  return ret;

}




/**
 * @brief
 *
 * @param data
 * @param bpa
 *
 * @return
 */

int cl_BgpsecVerify (BgpsecPathAttr *bpa, u_int16_t number_keys, BGPSecKey** keys,
                    void *prefix, u_int32_t local_as)
{
  if(cl_BgpsecSanityCheck(bpa) != 0)
  {
    sca_debugLog(LOG_ERR,"%s:[OUT] bgpsec Path Attr structure error\n", __FUNCTION__);
    return API_BGPSEC_VERIFY_ERROR;
  }

  PathSegment   *seg = bpa->pathSegments;
  SigBlock      *sb = bpa->sigBlocks;
  SigSegment    *ss = sb->sigSegments;

  struct prefix *p=(struct prefix *)prefix;
  //struct KeyInfoData *keyInfo = NULL;
  int currNum =0;

  int psize=0;
  if(p)
    psize = PSIZE (p->prefixlen);

  u_int8_t hashbuff[BGPSEC_MAX_SIG_LENGTH + 10];
  //u_int8_t sigbuff[BGPSEC_MAX_SIG_LENGTH];
  size_t hashLen = 0;

  u_short iter;
  int numSecurePathSegment = 0;

  /* Secure_Path length includes two octets used to express its own length field */
  iter = numSecurePathSegment =
    (bpa->securePathLen - OCTET_SECURE_PATH_LEN) / OCTET_SECURE_PATH_SEGMENT;

  /* Validation Routine */
  as_t targetAS = local_as;

  while(iter && seg && sb && ss)
  {
    /* encapsulated and stacked at least 2 and more segments */
    if(iter > 1)
    {
      /*  #### hashbuff generation (transit) ####
       * +-----------------------------------------+
       * | Target AS Number               4 octets |
       * +-----------------------------------------+
       * | Signer's AS Number             4 octets |
       * +-----------------------------------------+
       * | pCount                         1 octet  |
       * +-----------------------------------------+
       * | Flags                          1 octet  |
       * +-----------------------------------------+
       * | Sig Field in the Next Segment (variable)|
       * +-----------------------------------------+
       */

      /* hashbuff */
      put_u32(hashbuff, targetAS);          // Target AS: MUST be same as number in BGP_OPEN message
      put_u32(hashbuff+4, seg->as);         // Signer's AS

      *(hashbuff+8) = seg->pCount;      // pCount
      *(hashbuff+9) = seg->flags;       // Flags

      // most recent sigSegment elements
      hashLen = 10;
      if(!ss->next)
      {
        sca_debugLog(LOG_ERR,"Error: bad segment arrangement, ignoring\n", __FUNCTION__);
        return API_BGPSEC_VERIFY_ERROR;
      }
      //memcpy(hashbuff +10, ss->next->ski, BGPSEC_SKI_LENGTH);
      //hashLen = 10 + BGPSEC_SKI_LENGTH;
      //memcpy(hashbuff + hashLen, &ss->next->sigLen, OCTET_SIGNATURE_LEN);
      //hashLen += OCTET_SIGNATURE_LEN;
      //memcpy(hashbuff + hashLen, ss->next->signature, ss->next->sigLen);
      memcpy(hashbuff + hashLen, ss->next->signature, ss->next->sigLen);
      hashLen += ss->next->sigLen;


      if (IS_DEBUG_ENABLED)
      {
        int i;
        sca_debugLog(LOG_DEBUG,"[IN]:%d ---- HASH recv [total length: %d] ----\n", __LINE__, hashLen);
        for(i=0; i < hashLen ; i++ )
        {
          if(i%16 ==0) printf("\n");
          printf("%02x ", hashbuff[i]);
        }
        printf("\n");
        char skiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
        GEN_SKI_ASCII(skiBuffer, ss->ski, BGPSEC_SKI_LENGTH);
        sca_debugLog(LOG_DEBUG,"[IN] ---- current SKI: 0x%s ----\n", skiBuffer);
      }

      BGPSecSignData sslVerifyData = {
        .data           = hashbuff,
        .dataLength     = hashLen,
        .ski            = ss->ski,
        .algoID         = sb->algoSuiteId,
        .signature      = ss->signature,
        .sigLen         = ss->sigLen,
      };

      // calculate current number in array
      currNum = number_keys - iter;

      if(keys && keys[currNum])
      {
        if ( API_BGPSEC_VERIFY_SUCCESS!=
            stcl_BgpsecVerifySignatureSSL_withKeyInfo(&sslVerifyData, keys, currNum))
        {
          sca_debugLog(LOG_ERR,"Error: %s: bad signature, ignoring\n", __FUNCTION__);
          return API_BGPSEC_VERIFY_ERROR;
        }
        else
        {
          sca_debugLog(LOG_INFO,"BGPSEC Update Verification SUCCESS !!! [Signer AS:%d] \n\n",
              seg->as);
        }
      }
      else
      {
        /* call api library function */
        if ( API_BGPSEC_VERIFY_SUCCESS!= stcl_BgpsecVerifySignatureSSL(&sslVerifyData))
        {
          sca_debugLog(LOG_ERR,"Error: %s: bad signature, ignoring\n", __FUNCTION__);
          return API_BGPSEC_VERIFY_ERROR;
        }
        else
          sca_debugLog(LOG_INFO,"BGPSEC Update Verification SUCCESS !!! [Signer AS:%d] \n\n", seg->as);
      }


      /* next target as concatenation */
      targetAS = seg->as;
    } /* end of if */

    /* last segments */
    else
    {
      /* #### hashbuff generation (origin) ####
       * +-------------------------------------+
       * | Target AS number        : 4 octec   |
       * +-------------------------------------+
       * | Origin's AS number      : 4 octec   |
       * +-------------------------------------+
       * | pCount                  : 1 octet   |
       * +-------------------------------------+
       * | Flags                   : 1 octet   |
       * +-------------------------------------+
       * | Algorithm Suite Id.     : 1 octet   |
       * +-------------------------------------+
       * | NLRI Length             : 1 octet   |
       * +-------------------------------------+
       * | NLRI prefix             : (variable)|
       * +-------------------------------------+
       */

      hashLen = 0;
      /* hashbuff */
      put_u32(hashbuff, targetAS);          // Target AS: MUST be same as number in BGP_OPEN message
      put_u32(hashbuff+4, seg->as);         // Origin AS
      sca_debugLog(LOG_INFO,"[IN]:%d -- Target AS:%d Origin AS: %d \n", __LINE__, targetAS, seg->as);
      *(hashbuff+8) = seg->pCount;                 // pCount
      *(hashbuff+9) = seg->flags;                  // Flags
      hashbuff[10] = sb->algoSuiteId;             // Algorithm Suite Id
      hashbuff[11] = p->prefixlen;                 // NLRI length
      memcpy(hashbuff+12, &p->u.prefix, psize);    // NLRI prefix
      hashLen = 12 + psize;


      if (IS_DEBUG_ENABLED)
      {
        int i;
        sca_debugLog(LOG_DEBUG,"[IN]:%d ---- HASH recv for Origin [total length: %d] ----\n", __LINE__, hashLen);
        for(i=0; i < hashLen ; i++ )
        {
          if(i%16 ==0) printf("\n");
          printf("%02x ", hashbuff[i]);
        }
        printf("\n");
        char skiBuffer[BGPSEC_SKI_LENGTH * 2 + 1];
        GEN_SKI_ASCII(skiBuffer, ss->ski, BGPSEC_SKI_LENGTH);
        sca_debugLog(LOG_DEBUG,"[IN] ---- current SKI: 0x%s ----\n", skiBuffer);
      }

      BGPSecSignData sslVerifyData = {
        .data           = hashbuff,
        .dataLength     = hashLen,
        .ski            = ss->ski,
        .algoID         = sb->algoSuiteId,
        .signature      = ss->signature,
        .sigLen         = ss->sigLen,
      };

      if(keys && *keys)
      {
        if ( API_BGPSEC_VERIFY_SUCCESS!=
            stcl_BgpsecVerifySignatureSSL_withKeyInfo(&sslVerifyData, keys, (number_keys - iter)))
        {
          sca_debugLog(LOG_ERR,"Error: %s: bad signature, ignoring\n", __FUNCTION__);
          return API_BGPSEC_VERIFY_ERROR;
        }
        else
        {
          sca_debugLog(LOG_INFO,"BGPSEC Update Verification SUCCESS !!! [Origin AS:%d] \n\n",
              seg->as);
        }
      }
      else
      {
        /* call api library function */
        if ( API_BGPSEC_VERIFY_SUCCESS!= stcl_BgpsecVerifySignatureSSL(&sslVerifyData))
        {
          sca_debugLog(LOG_ERR,"Error: %s: bad signature, ignoring\n", __FUNCTION__);
          return API_BGPSEC_VERIFY_ERROR;
        }
        else
          sca_debugLog(LOG_INFO,"BGPSEC Update Verification SUCCESS !!! [Origin AS:%d] \n\n", seg->as);
      }

    } /* end of else */


    /* next Secure_Path segment and Signature Segment */
    seg = seg->next;
    ss = ss->next;
    iter--;

  } /* end of while */

  return API_BGPSEC_VERIFY_SUCCESS;
}

/* API function call match */
int validate (BgpsecPathAttr *bpa, u_int16_t number_keys, BGPSecKey** keys, void *prefix,
                u_int32_t local_as)
{
  return cl_BgpsecVerify (bpa, number_keys, keys, prefix, local_as);
}


int sign_with_key (BGPSecSignData* bgpsec_data, BGPSecKey *key)
{
  return( (key && key->keyData) ?
      stBgpsecReqSigningWithKeyInfo(bgpsec_data, key):
      stBgpsecReqSigning(bgpsec_data, key));
}


int sign_with_id(u_int16_t dataLength, u_int8_t* data, u_int8_t keyID,
                      u_int16_t sigLen, u_int8_t* signature)
{
  BGPSecSignData bgpsecSignData = {
    .dataLength       = dataLength,
    .data             = data,
    .ski              = NULL,
    .algoID           = API_BGPSEC_ALGO_ID_256,
    .sigLen           = sigLen,
    .signature        = signature,
  };

  /*
  BGPSecKey   bgpsecKey = {
    .algoID           = API_BGPSEC_ALGO_ID_256,
    .keyLength        = 0,
    .keyData          = NULL,
  };
  */

  return stBgpsecReqSigningWithID(&bgpsecSignData, keyID);
}

int cl_SignParamSanityCheck(BGPSecSignData *signData, BGPSecKey *key)
{
  if(!signData )
    return -1;

  if(!signData->ski || !signData->data || !signData->signature)
    return -1;

#if 0
  if(!key)
    return -1;

  if(!key->keyData)
    return -1;
#endif

  return 0;
}

int cl_SignParamWithKeySanityCheck(BGPSecSignData *signData, BGPSecKey *key)
{
  if(!signData )
    return -1;

  if(!signData->ski || !signData->data || !signData->signature)
    return -1;

  if(!key)
    return -1;

  if(!key->keyData)
    return -1;

  return 0;
}

int cl_SignParamWithIDSanityCheck(BGPSecSignData *signData, u_int8_t keyID)
{
  if(!signData )
    return -1;

  if(!signData->data || !signData->signature)
    return -1;

  if(!keyID)
    return -1;

  return 0;
}

int cl_BgpsecSanityCheck(BgpsecPathAttr *bpa)
{
  PathSegment *seg;
  SigBlock *sb;
  SigSegment *ss;

  if(!bpa)
    return -1;

  seg = bpa->pathSegments;
  sb  = bpa->sigBlocks;

  if(!seg || !sb)
    return -1;

  ss = sb->sigSegments;

  if(!ss)
    return -1;

  if(!ss->signature || !ss->ski)
    return -1;

  return 0;
}


unsigned char* cl_BgpsecOctetDigest(const unsigned char* octet, unsigned int octet_len, unsigned char* md)
{
  unsigned char result[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256ctx;
  SHA256_Init(&sha256ctx);
  SHA256_Update(&sha256ctx, octet, octet_len);
  SHA256_Final(result, &sha256ctx);

  if(md)
  {
    memcpy(md, result, SHA256_DIGEST_LENGTH);
  }
  else
    return NULL;

  if (IS_DEBUG_ENABLED)
  {
    int i=0;
    printf("octet leng: %d\nmessage digest: 0x", octet_len);
    for( i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
      printf("%02x ", md[i]);
    }
    putchar('\n');
  }

  return md;
}



/**
* @brief
*
* @param evp_pkey
* @param privkey
* @param privkey_len
* @param pubkey
* @param pubkey_len
* @param bn_ctx
*/
int EVP_PKEY_set_keys(EVP_PKEY *evp_pkey,
        const unsigned char *privkey, size_t privkey_len,
           const unsigned char *pubkey, size_t pubkey_len,
           BN_CTX *bn_ctx)
{
    EC_KEY *ec_key = NULL;
    EC_POINT *ec_point = NULL;
    BIGNUM *bn = NULL;
    int ok = 0;
    const EC_GROUP *group;

    switch (EVP_PKEY_type(evp_pkey->type))
    {
        case EVP_PKEY_EC:
            ec_key = EVP_PKEY_get1_EC_KEY(evp_pkey);
            if (!ec_key)
                goto err;
            group = EC_KEY_get0_group(ec_key);

            if (pubkey) {
                ec_point = EC_POINT_new(group);
                if (!ec_point
                        || !EC_POINT_oct2point(group, ec_point, pubkey,
                            pubkey_len, bn_ctx)
                        || !EC_KEY_set_public_key(ec_key, ec_point))
                    goto err;
            }
            if (privkey) {
                bn = BN_bin2bn(privkey, privkey_len, bn);
                if (!bn || !EC_KEY_set_private_key(ec_key, bn))
                    goto err;
            }

            if (!EVP_PKEY_set1_EC_KEY(evp_pkey, ec_key))
                goto err;
            break;

        default:
            //sca_debugLog(LOG_ERR, "Unknown type of key");
            goto err;
            break;
    }

    ok = 1;

err:
    if (bn)
        BN_clear_free(bn);
    if (ec_key)
        EC_KEY_free(ec_key);
    if (ec_point)
        EC_POINT_clear_free(ec_point);

    return ok;
}


EVP_PKEY * dupEVP_PKEY(EVP_PKEY *key)
{
    EVP_PKEY *out = NULL;
    EC_KEY *ec_in = NULL, *ec_out = NULL;

    out = EVP_PKEY_new();

    if (!out)
        goto err;

    switch (EVP_PKEY_type(key->type)) {

      case EVP_PKEY_EC:
            ec_in = EVP_PKEY_get1_EC_KEY(key);
            if (!ec_in)
                goto err;

            ec_out = EC_KEY_dup(ec_in);
            if (!ec_out)
                goto err;

            EVP_PKEY_set1_EC_KEY(out, ec_out);
            EC_KEY_free(ec_out);
            EC_KEY_free(ec_in);
            break;


        default:
            //sca_debugLog(LOG_DEBUG,"Unknown protocol");
            goto err;
    }

    return out;

err:
    if (ec_in)
        EC_KEY_free(ec_in);

    return NULL;
}

// @TODO: Remove prior publishing is not used anymore
/*
static EVP_PKEY * cl_getEVP_PKEYfromPubkey(EVP_PKEY *key, const unsigned char *pub_data,
    size_t pubkey_len, BN_CTX *bn_ctx)
{
    EVP_PKEY *out = NULL;

    //sca_debugLog(LOG_DEBUG,"Invalid arguments");

    out = dupEVP_PKEY(key);

    if (!EVP_PKEY_set_keys(out, NULL, 0,
                (const unsigned char *) pub_data, pubkey_len, bn_ctx)) {
        EVP_PKEY_free(out);
        out = NULL;
        goto err;
    }

err:
    return out;
}
*/

/**
 * @brief
 *
 * @param record
 * @param ec_key
 * @param ski
 * @param asn
 *
 * @return  hash id
 */
int inputKeyInfo(Record_t *record, EC_KEY* ec_key, u_int8_t* ski, u_int32_t asn,
                 BGPSecKey* outKey)
{

  unsigned int id=0;
  BGPSecKey *bsKeyInfo;

  if(ec_key && outKey)
  {
    /* entry data preparation */
    bsKeyInfo = (BGPSecKey*)malloc(sizeof(BGPSecKey));
    if(!bsKeyInfo)
      goto err;
    bsKeyInfo->algoID   = API_BGPSEC_ALGO_ID_256;
    bsKeyInfo->keyLength = sizeof(EC_KEY);
    //bsKeyInfo->keyData = (u_int8_t*)ec_key;
    bsKeyInfo->keyData  = outKey->keyData;
    bsKeyInfo->asn      = outKey->asn;
    memcpy(bsKeyInfo->ski, outKey->ski, SKI_LENGTH);
  }
  else
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto](%s:%d) ec key is NULL\n", __FUNCTION__, __LINE__);
    return -1;
  }

  /* new entry input */
  Record_t *new = (Record_t*) malloc(sizeof(Record_t));
  memset(new, 0, sizeof(Record_t));

  new->key.asn = asn;
  new->ski = malloc(SKI_LENGTH);

  if (new->ski != NULL)
  {
    memcpy(new->ski, ski, SKI_LENGTH);

  new->data = (void*)bsKeyInfo;

  HASH_ADD(hh, g_records, key, sizeof(DataKey_t), new);

  id = new->hh.hashv;
  sca_debugLog (LOG_DEBUG,"[%s:%d] ski: %p\n",
                  __FUNCTION__, __LINE__, new->ski);
  sca_debugLog (LOG_DEBUG, "[%s] new data (bgpsec key): %p Total num of data:%d\n",
      __FUNCTION__, bsKeyInfo, (g_records)->hh.tbl->num_items);
  sca_debugLog (LOG_DEBUG," number of bucket:%d  hashv:%x\n", (g_records)->hh.tbl->num_buckets, new->hh.hashv);
  sca_debugLog (LOG_DEBUG," ha_bkt(id): %x\n", (new->hh.hashv) & (((g_records)->hh.tbl->num_buckets) -1) );

  return (id & (ID_NUM_MAX-1)) ;
  }

err:
  if(bsKeyInfo) free(bsKeyInfo);
  if(new) free(new);
  id = -1;
  return id;
}


/**
 * @brief
 *
 * @param record
 * @param ski
 * @param asn
 *
 * @return
 */
Record_t* restoreKeyInfo(u_int8_t* ski, u_int32_t asn)
{
  BGPSecKey *outKeyInfo =NULL;

  Record_t temp, *out;
  temp.key.asn = asn;
  //temp.ski = ski;

  sca_debugLog (LOG_DEBUG, "[%s:%d] given ski:%02x asn:%ld\n",
      __FUNCTION__, __LINE__, *ski, asn);

  HASH_FIND(hh, g_records, &temp.key, sizeof(DataKey_t), out);

  if(out)
  {
    if (cmpSKI(out->ski, ski))
    {
      sca_debugLog (LOG_DEBUG, "[%s] FOUND a key with matching SKI and ASN\n",
          __FUNCTION__);
      outKeyInfo = (BGPSecKey *) out->data;
      sca_debugLog (LOG_DEBUG, "[%s] EC KEY data : %p\n", __FUNCTION__, (EC_KEY*)outKeyInfo->keyData);
    }
    else
    {
      sca_debugLog (LOG_DEBUG, "[%s] FOUND a key with matching ASN(%d) but SKI is different\n",
          __FUNCTION__, out->key.asn);
      return NULL;
    }
  }
  else
  {
    sca_debugLog (LOG_DEBUG,"(%s:%d) NOT FOUND a key\n", __FUNCTION__, __LINE__);
    return NULL;
  }

  //return outKeyInfo;
  return out;
}

Record_t* restoreKeyInfo_withID(u_int8_t* ski, u_int32_t asn, u_int32_t id)
{

  Record_t *s;

  for(s= g_records; s!=NULL; s=(Record_t*)(s->hh.next))
  {
    if (s->key.asn == asn && cmpSKI(s->ski, ski))
    {
      if(id == s->hh.hashv)
        sca_debugLog (LOG_DEBUG, "=== Item Found === id:%x asn: %d  ski:%s\n", id, s->key.asn, s->ski);
      else
        return NULL;
      return s;
    }
    else
    {
      sca_debugLog (LOG_DEBUG, " Item not Found \n");
      return NULL;
    }

  }
  return NULL;
}

BGPSecKey * hashKey_get_BGPSecKey(u_int32_t asn, u_int8_t* ski)
{
  unsigned int ret_hash_id=0;
  Record_t  *rec = NULL;
  BGPSecKey *tmpBsKey = NULL;

  if (!ski || !asn)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] input parameters error\n");
    return NULL;
  }

  rec = restoreKeyInfo(ski, asn);
  if (rec)
  {
    tmpBsKey = (BGPSecKey *)rec->data;
    ret_hash_id = rec->hh.hashv;
    sca_debugLog (LOG_DEBUG, "[%s:%d] FOUND --> hash id : %x, tmpBsKey:%p\n",
        __FUNCTION__, __LINE__, ret_hash_id, tmpBsKey);
  }
  else
  {
    sca_debugLog(LOG_DEBUG, "+ [libcrypto] NOT FOUND\n");
  }

  return tmpBsKey;
}

/**
 * @brief
 *
 * @param ski
 * @param skiLen
 * @param asn
 * @param
 * @param skiLen
 * @param asn
 * @param in_id
 * @param outKey : this will be used as an output along with outKey
 *
 * @return          : key pointer array
 */
u_int8_t registerPrivateKey(BGPSecKey* outKey)
{
  u_int8_t  ret_hash_id=0;
  EC_KEY    *ec_key=NULL;
  Record_t  *rec=NULL;
  //BGPSecKey* bsKeyInfo = NULL;

  if(!outKey )
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] out key container doesn't exist \n");
    return 0;
  }

  u_int8_t* ski    = outKey->ski;
  u_int32_t     asn     = outKey->asn;

  if(!asn)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] input parameters error\n");
    return 0;
  }

  sca_debugLog (LOG_DEBUG, "(%s:%d) params: ski:%02x, asn:%ld  out:%p\n",
      __FUNCTION__, __LINE__, *ski, asn, outKey);

  /*
   * first try to find if already existed or not.
   * otherwise, proceed to register
   */
  rec = restoreKeyInfo(ski, asn);
  if(rec)
  {
    ret_hash_id = (rec->hh.hashv & (ID_NUM_MAX-1));
    sca_debugLog (LOG_DEBUG, "[%s] ALREADY REGISTERED--> hash id : %x, BGPSecKey:%p\n",
        __FUNCTION__, ret_hash_id, (BGPSecKey *)rec->data);
    return ret_hash_id + RET_ID_OFFSET;
  }

  /* get a private ec_key from DER buffer */
  sca_debugLog (LOG_DEBUG, "(%s:%d) key_Data:%p, len:%d \n",
      __FUNCTION__, __LINE__, outKey->keyData, outKey->keyLength);

  unsigned char *p = outKey->keyData;

  if (IS_DEBUG_ENABLED)
  {
    printf("\n ----- printing out buf hex:%p ----- {%s:%d}\n",
        outKey->keyData, __FUNCTION__, __LINE__);
    int i;
    for(i=0; i< outKey->keyLength; i++)
    {
      printf("%x ", outKey->keyData[i]);
    }
    printf("\n\n");
  }

  /* get EC_POINT pubkey and BN_ private key info into EC_KEY */
  ec_key = d2i_ECPrivateKey(NULL, (const unsigned char**)&p, outKey->keyLength);

  if (!ec_key )
  {
    ECerr(EC_F_ECKEY_PRIV_DECODE, EC_R_DECODE_ERROR);
    sca_debugLog(LOG_ERR, "+ [libcrypto](%s:%d) obtaining ec key error\n", __FUNCTION__,__LINE__);
    return 0;
  }

  sca_debugLog (LOG_DEBUG, "ec key : %p\n", ec_key);
  sca_debugLog (LOG_DEBUG, "ec point pub key returned : %p\n", ec_key->pub_key);
  sca_debugLog (LOG_DEBUG, "ec point private key returned : %p (dmax:%d)\n",
      ec_key->priv_key->d, ec_key->priv_key->dmax);

  if (IS_DEBUG_ENABLED)
    PrintPrivPubKeyHex(ec_key);


  /* register key info into hash */
  int retVal= inputKeyInfo(g_records, ec_key, ski, asn, outKey);
  if(retVal == -1)
  {
    ret_hash_id = 0; //error
  }
  else
  {
    ret_hash_id = (u_int8_t)retVal;
  }

  /* store the key into key array */

  stEnbuf_Eckey[ret_hash_id].id     = ret_hash_id;
  stEnbuf_Eckey[ret_hash_id].ec_key = (void*)ec_key;
  stEnbuf_Eckey[ret_hash_id].en_buf = (void*)outKey->keyData;

  sca_debugLog (LOG_DEBUG, "[%s] hash id : %x, outKey:%p keydata:%p ec_key:%p\n",
      __FUNCTION__, ret_hash_id, outKey, outKey->keyData, ec_key);


  // test
  //restoreKeyInfo_withID(ski, asn, ret_hash_id);

  /* return hash id */
  return ret_hash_id + RET_ID_OFFSET;  // 0: error, so start from 1 offset


}

/**
 * API method for registerPublicKey
 *
 * @param outKey containes key pointer to be converted
 *
 * @return          : key pointer array
 */
u_int8_t registerPublicKey(BGPSecKey* outKey)
{
  u_int8_t  ret_hash_id=0;
  EC_KEY    *ec_key=NULL;
  Record_t  *rec=NULL;

  sca_debugLog(LOG_DEBUG, "+ [libcrypto]  registerPublicKey called \n");

  if(!outKey )
  {
      sca_debugLog(LOG_ERR, "+ [libcrypto] out Pub-key container doesn't exist \n");
      return 0;
  }

  u_int8_t* ski = outKey->ski;
  u_int32_t asn = outKey->asn;

  if(!asn)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] pub input parameters error\n");
    return 0;
  }

  sca_debugLog (LOG_DEBUG, "(%s:%d) params: ski:%02x, asn:%ld  out:%p\n",
      __FUNCTION__, __LINE__, *ski, asn, outKey);

  /*
   * first try to find if already existed or not.
   * otherwise, proceed to register
   */
  rec = restoreKeyInfo(ski, asn);
  if(rec)
  {
    ret_hash_id = (rec->hh.hashv & (ID_NUM_MAX-1));

    BGPSecKey *tmpBSKey = (BGPSecKey *)rec->data;
    BGPSecKey bsKeyInfo;
    memset(&bsKeyInfo, 0x0, sizeof(BGPSecKey));
    bsKeyInfo = *tmpBSKey; /* struct copy */
    bsKeyInfo.keyData = (u_int8_t *)malloc(tmpBSKey->keyLength); // this pointer will be freed by caller
    if(bsKeyInfo.keyData)
      memcpy(bsKeyInfo.keyData, tmpBSKey->keyData, tmpBSKey->keyLength);

    /* prevent from approaching error status */
    if (outKey->keyData == NULL) outKey->keyData = bsKeyInfo.keyData;
    else
      if(bsKeyInfo.keyData) free(bsKeyInfo.keyData);
    if (outKey->keyLength == 0)  outKey->keyLength = bsKeyInfo.keyLength;

    sca_debugLog (LOG_DEBUG, "[%s] ALREADY REGISTERED--> hash id : %x, BGPSecKey:%p\n",
        __FUNCTION__, ret_hash_id, (BGPSecKey *)rec->data);
    return ret_hash_id + RET_ID_OFFSET;
  }

  /* a public ec_key from DER buffer */
  sca_debugLog (LOG_DEBUG, "(%s:%d) key_Data:%p, len:%d \n",
      __FUNCTION__, __LINE__, outKey->keyData, outKey->keyLength);

  unsigned char *p = outKey->keyData;

  if(!p)
  {
    sca_debugLog (LOG_DEBUG, "keyData is NULL\n");
    return 0;
  }

  if (IS_DEBUG_ENABLED)
  {
    printf("\n ----- printing out buf hex:%p ----- {%s:%d}\n",
        outKey->keyData, __FUNCTION__, __LINE__);
    int i;
    for(i=0; i< outKey->keyLength; i++)
    {
      printf("%x ", outKey->keyData[i]);
    }
    printf("\n\n");
  }

  /* get EVP_PKEY pubkey from the key data and retrieve an EC_KEY object from EVP_PKEY object */
  EVP_PKEY *pkey_tmp;
  int eplen = outKey->keyLength;

  /* We have parameters now set public key */
  pkey_tmp = d2i_PUBKEY(NULL, (const unsigned char**)&p, eplen);
  if (!pkey_tmp)
  {
      ECerr(EC_F_ECKEY_PUB_DECODE, EC_R_DECODE_ERROR);
      sca_debugLog(LOG_ERR, "+ [libcrypto](%s:%d) PUBKEY pub key failed\n", __FUNCTION__,__LINE__);
      return 0;
  }
  ec_key = EVP_PKEY_get1_EC_KEY(pkey_tmp);
  EVP_PKEY_free(pkey_tmp);


  /* showing pub key info */
  sca_debugLog (LOG_DEBUG, "ec key : %p\n", ec_key);
  sca_debugLog (LOG_DEBUG, "ec point pub key returned : %p\n", ec_key->pub_key);

  if (IS_DEBUG_ENABLED)
    PrintPrivPubKeyHex(ec_key);


  /* register key info into hash */
  void *outKeyData = NULL;
  int retVal= inputKeyInfo(g_records, ec_key, ski, asn, outKey, &outKeyData);
  if(retVal == -1)
  {
    ret_hash_id = 0; //error
  }
  else
  {
    ret_hash_id = (u_int8_t)retVal;
  }

  if(!outKeyData)
  {
    sca_debugLog (LOG_ERR, "outKeyData error \n");
  }


  /* store the key into key array */
#if 0 // ret_hash_id once duplicated, so that it need to have a better mechanism to store
  stEnbuf_Eckey[ret_hash_id].id     = ret_hash_id;
  stEnbuf_Eckey[ret_hash_id].ec_key = (void*)ec_key;
  stEnbuf_Eckey[ret_hash_id].en_buf = (void*)outKeyData;
#endif

  sca_debugLog (LOG_DEBUG, "[%s] hash id : %x, outKey:%p outkey->keydata:%p ec_key:%p outKeyData:%p\n",
      __FUNCTION__, ret_hash_id, outKey, outKey->keyData, ec_key, outKeyData);


  /* return hash id */
  return ret_hash_id + RET_ID_OFFSET;  // 0: error, so start from 1 offset

}

int unregisterPublicKey(BGPSecKey* key)
{
}


int deleteKeyInfo(Record_t *rec)
{
  if(!rec)
    return -1;

  // free and release allocated heap memory
  BGPSecKey *bsKeyInfo = rec->data;
  EC_KEY* ec_key = (EC_KEY*)bsKeyInfo->keyData;

  if (ec_key)
  {
    sca_debugLog(LOG_DEBUG, "+ [libcrypto][%s:%d] EC_KEY removing \n", __FUNCTION__, __LINE__);
    EC_KEY_free(ec_key);
  }
  if(bsKeyInfo)
  {
    sca_debugLog(LOG_DEBUG, "+ [libcrypto][%s:%d] BGPSecKey info removing \n", __FUNCTION__, __LINE__);
    free(bsKeyInfo);
  }
  if(rec->ski)
  {
    sca_debugLog(LOG_DEBUG, "+ [libcrypto][%s:%d] SKI info removing \n", __FUNCTION__, __LINE__);
    free(rec->ski);
  }

  // remove the data from HASH
  sca_debugLog(LOG_DEBUG, "+ [libcrypto][%s:%d] HASH record removing \n", __FUNCTION__, __LINE__);
  HASH_DEL(g_records, rec);
  free(rec);

  return 0;
}

u_int32_t unregisterKey (u_int8_t* ski, u_int32_t asn, u_int32_t keyID)
{
  /* first parameters health check */
  if (asn == 0)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto][%s:%d] input parameters error\n", __FUNCTION__, __LINE__);
    return 0;
  }

  unsigned int hash_id=0;
  Record_t *rec = NULL;
  BGPSecKey *bsKey = NULL;


  rec = restoreKeyInfo(ski, asn);
  if(rec)
  {
    bsKey = (BGPSecKey *)rec->data;
    hash_id = rec->hh.hashv;

    sca_debugLog (LOG_DEBUG, "[%s:%d] Found a key record (hash id : %x, bgpsec key info data:%p, EC_KEY:%p) \n",
        __FUNCTION__, __LINE__, hash_id, bsKey, bsKey?bsKey->keyData:NULL);

    // unregister and delete key info data from HASH
    deleteKeyInfo(rec);
  }
  else
  {
    sca_debugLog (LOG_DEBUG, "[%s:%d] Can't find a key record from HASH\n", __FUNCTION__, __LINE__);
    return 0;
  }

  return 0;
}


u_int8_t unregisterPrivateKey(u_int8_t inID)
{
  EC_KEY    *ec_key     = NULL;
  BGPSecKey *tmpBsKey   = NULL;
  u_int8_t  keyID       = inID - RET_ID_OFFSET;
  void      *en_buf;

  sca_debugLog (LOG_DEBUG, "[%s:%d] UnregisterPrivateKey called with key id: %x \n",
      __FUNCTION__, __LINE__, keyID);

  if(keyID != stEnbuf_Eckey[keyID].id)
  {
    sca_debugLog(LOG_ERR, "keyID is different:%x \n", stEnbuf_Eckey[keyID].id);
    return 0;
  }

  ec_key = stEnbuf_Eckey[keyID].ec_key;
  en_buf = stEnbuf_Eckey[keyID].en_buf;

  /* Remove from uthash */
  Record_t *s;
  for(s= g_records; s!=NULL; s=(Record_t *)(s->hh.next))
  {
    tmpBsKey = s->data;
    if( (s->hh.hashv & (((g_records)->hh.tbl->num_buckets) -1)) == keyID )
    {
      //if( ec_key == (EC_KEY *)tmpBsKey->keyData )
      if( en_buf == (void*)tmpBsKey->keyData )
      {
        sca_debugLog(LOG_INFO, "+ [libcrypto][%s:%d] HASH record removing \n", __FUNCTION__, __LINE__);
        sca_debugLog (LOG_DEBUG, " === Item Found to be deleted === id:%x keyData:%p\n", s->hh.hashv, en_buf);
        //deleteKeyInfo(s);
        HASH_DEL(g_records,  s);
        free(s);
      }
    }
  }

  if (ec_key)
  {
    sca_debugLog(LOG_DEBUG, "ec key pointer: %p is about to be freed\n", ec_key);
    EC_KEY_free(ec_key);
  }

  if(en_buf)
  {
    sca_debugLog(LOG_DEBUG, "DER-encoded key pointer: %p is about to be freed\n", en_buf);
    OPENSSL_free(en_buf);
  }

  stEnbuf_Eckey[keyID].ec_key = NULL;
  stEnbuf_Eckey[keyID].en_buf = NULL;
  stEnbuf_Eckey[keyID].id   = 0;

  return 0;
}

void PrintPrivPubKeyHex(EC_KEY* ec_key)
{
  if(!ec_key)
  {
    printf(" ec_key error \n");
    return;
  }
  if(((struct ec_key_st*)ec_key)->priv_key)
  {
    printf("\n ----- [%s] printing out private key hex ----- \n", __FUNCTION__);
    int i=0;
    for(i=0; i<((struct ec_key_st*)ec_key)->priv_key->dmax; i++)
    {
      printf("%x ", (((struct ec_key_st*)ec_key)->priv_key->d)[i]);
    }
    printf("\n\n");
  }

  printf("\n ----- [%s] printing out pub key hex ----- \n", __FUNCTION__);
  BN_CTX *ctx;
  ctx = BN_CTX_new();
  char * cc = EC_POINT_point2hex(ec_key->group, ec_key->pub_key, POINT_CONVERSION_UNCOMPRESSED/*4*/, ctx);
  printf("pub: %s\n", cc);
  printf("\n\n");
}

/**
 * @brief
 *
 * @param p
 * @param length
 *
 * @return  1: private
 *          0: public
 *          negative: not DER format
 */
int IS_DER_PRIVATE(unsigned char *p, unsigned short length)
{
#define ID_SEQUENCE         0x30
#define ID_INTEGER          0x02
#define ID_OCTET_STRING     0x04

  unsigned char pos = 0;

  if(!p)
  {
    sca_debugLog(LOG_ERR, "buff is NULL\n");
    return -1;
  }
  if (p[pos++] != ID_SEQUENCE)
  {
    sca_debugLog(LOG_ERR, "Not DER data\n");
    return -1;
  }

  unsigned char  init_len = 0;
  unsigned short int_len = 0;
  unsigned char subseq_len =0;

  /* ITU-T Rec. X.690, 8.1.3.5: initial octet as a long term length octet indicator */
  init_len = p[pos++]; // offset(01): eigher total length or initial octet as a long term octet

  if(length < 127 && !(init_len & 0x80))
  {
    /* DER: SEQUENCE 0x30,  INTEGER 0x02, and OCTET_STRING 0x04 indicator position */
    if(p[pos] == ID_INTEGER) // offet(02)
    {
      pos++;
      int_len = p[pos++];
      pos += int_len;

      if(p[pos] == ID_OCTET_STRING)
      {
        sca_debugLog(LOG_INFO, "Private octet string in DER\n");
        return 1;
      }
    }

    /* DER: SEQUENCE 0x30, SEQUENCE 0x30 indicator position */
    else if(p[pos++] == ID_SEQUENCE) // offet(02)
    {
      sca_debugLog(LOG_INFO, "Public octet string in DER\n");
      return 0;
    }
  }
  else /* octet length is larger than 127 octec */
  {
    subseq_len = init_len & 0x7f; // the number of subsequent octet length
    pos += subseq_len;

    //printf(" ---- subsequent len:%02x\n", subseq_len);
    //printf(" ---- ID: %02x\n", p[pos]);

    if(p[pos] == ID_INTEGER)
    {
      pos++;
      int_len = p[pos++];
      pos += int_len;

      //printf(" ---- integer len:%02x\n", int_len);

      if(p[pos++] == ID_OCTET_STRING)
      {
        sca_debugLog(LOG_INFO, "Private octet string in DER (length > 127)\n");
        return 1;
      }
    }
    else if(p[pos] == ID_SEQUENCE)
    {
      sca_debugLog(LOG_INFO, "Public octet string in DER (length > 127)\n");
      return 0;
    }
  }

  return -1;
}
