static int eckey_param2type(int *pptype, void **ppval, EC_KEY *ec_key)
{
  const EC_GROUP  *group;
  int nid;
  if (ec_key == NULL || (group = EC_KEY_get0_group(ec_key)) == NULL)
  {
    ECerr(EC_F_ECKEY_PARAM2TYPE, EC_R_MISSING_PARAMETERS);
    return 0;
  }
  if (EC_GROUP_get_asn1_flag(group)
      && (nid = EC_GROUP_get_curve_name(group)))
    /* we have a 'named curve' => just set the OID */
  {
    *ppval = OBJ_nid2obj(nid);
    *pptype = V_ASN1_OBJECT;
  }
  else    /* explicit parameters */
  {
    ASN1_STRING *pstr = NULL;
    pstr = ASN1_STRING_new();
    if (!pstr)
      return 0;
    pstr->length = i2d_ECParameters(ec_key, &pstr->data);
    if (pstr->length < 0)
    {
      ASN1_STRING_free(pstr);
      ECerr(EC_F_ECKEY_PARAM2TYPE, ERR_R_EC_LIB);
      return 0;
    }
    *ppval = pstr;
    *pptype = V_ASN1_SEQUENCE;

    sca_debugLog(LOG_INFO, " ======== ASN STRING V_ASN1_SEQUENCE ===== [%d]\n", pstr->length);
    if( LOG_DEBUG <= g_loglevel )
    {
      int i;
      for(i=0; i< pstr->length; i++)
      {
        printf("%02x ", pstr->data[i]);
      }
      printf("\n\n");
    }
  }
  return 1;
}

static EC_KEY *eckey_type2param(int ptype, void *pval)
{
  EC_KEY *eckey = NULL;
  if (ptype == V_ASN1_SEQUENCE)
  {
    ASN1_STRING *pstr = pval;
    const unsigned char *pm = NULL;
    int pmlen;
    pm = pstr->data;
    pmlen = pstr->length;
    if (!(eckey = d2i_ECParameters(NULL, &pm, pmlen)))
    {
      ECerr(EC_F_ECKEY_TYPE2PARAM, EC_R_DECODE_ERROR);
      goto ecerr;
    }
  }
  else if (ptype == V_ASN1_OBJECT)
  {
    ASN1_OBJECT *poid = pval;
    EC_GROUP *group;

    /* type == V_ASN1_OBJECT => the parameters are given
     * by an asn1 OID
     */
    if ((eckey = EC_KEY_new()) == NULL)
    {
      ECerr(EC_F_ECKEY_TYPE2PARAM, ERR_R_MALLOC_FAILURE);
      goto ecerr;
    }
    group = EC_GROUP_new_by_curve_name(OBJ_obj2nid(poid));
    if (group == NULL)
      goto ecerr;
    EC_GROUP_set_asn1_flag(group, OPENSSL_EC_NAMED_CURVE);
    if (EC_KEY_set_group(eckey, group) == 0)
      goto ecerr;
    EC_GROUP_free(group);
  }
  else
  {
    ECerr(EC_F_ECKEY_TYPE2PARAM, EC_R_DECODE_ERROR);
    goto ecerr;
  }

  return eckey;

ecerr:
  if (eckey)
    EC_KEY_free(eckey);
  return NULL;
}

struct ec_key_st {
  int version;

  EC_GROUP *group;

  EC_POINT  *pub_key;
  BIGNUM    *priv_key;

  unsigned int enc_flag;
  point_conversion_form_t conv_form;

  int references;

  void *method_data;  /* was EC_EXTRA_DATA */
} /* EC_KEY */;

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
 * Load the key from the key volt location configured within the API. The key
 * needs the SKI specified in binary format.
 * The returned key is in DER format. The parameter fPrivate is used to
 * indicate if the private or public key will be returned. This is of importance
 * in case both keys exist. Both keys will have the same SKI.
 *
 * @param key Pre-allocated memory where the ley will be loaded into.
 * @param fPrivate indicates if the key is private or public.
 *
 * @return 1 if key was loaded successfully, 0 otherwise
 */
int sca_loadKey(BGPSecKey* key, bool fPrivate)
{
#define USE_EC_PUB_EVP_PKEY
#undef  USE_EC_PUB_OCTET_KEY
#define DECODING_TEST_CODE
#define HEX_DEBUG_OUTPUT

  EC_KEY    *ec_key=NULL;
  char      filePrefix[MAXPATHLEN];
  char      szFileName[MAXPATHLEN];
  struct stat statbuf;
  int       status, pub_status=0, priv_status=0;
  char *    arrExt[MAX_EXT] = {DEFAULT_CERTFILE_EXT, DEFAULT_KEYFILE_EXT, NULL};

  /* get a dir/filename pair without an extension from the function below */
  if (!sca_FindDirInSKI(filePrefix, sizeof(filePrefix), key->ski))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] failed to access a file name from a ski\n");
    return API_LOADKEY_FAILURE;
  }

  /* file prefix validity check */
  int i;
  for(i=0; arrExt[i]; i++)
  {
    BIO_snprintf(szFileName, sizeof(szFileName)-1, "%s.%s", filePrefix, arrExt[i]);
    sca_debugLog(LOG_INFO, "+ [libcrypto] file name: %s\n", szFileName);

    /* if successful, return value will be 0, otherwise -1 and set the errno */
    status = stat(szFileName, &statbuf);

    if(status != -1)
    {
      if (((statbuf.st_mode & S_IFMT) == S_IFREG) || S_ISLNK(statbuf.st_mode))
      {
        if(i==0) pub_status = 1;
        else if(i==1) priv_status =1;

        sca_debugLog(LOG_INFO, "+ [libcrypto] %s is  a regular file being readable and accessable\n",\
            szFileName);
      }
    }
    else
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] failed to access a file %s from a ski\n", szFileName);
    }
  }

  /* get pub and/or private key and set into EC_KEY variable */
  if(pub_status)
  {
    if (API_SUCCESS !=
        sca_BgpsecSetEcPublicKey(filePrefix, &ec_key, API_DEFAULT_CURVE))
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to load a bgpsec pub key from an SKI\n");
      goto int_err;
    }
    sca_debugLog(LOG_INFO, "+ [libcrypto] pub key load success\n");
  }
  else
  {
    /* Create a dummy public key. The reason is that you cannot run i2d_ECPrivateKey
     * without setting a public key, probably due to a bug in OpenSSL
     */
    unsigned char pkinit_1024_dhprime[16] = {
      0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE6, 0x53, 0x81,
    };
    EC_GROUP * group = EC_GROUP_new_by_curve_name(API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256);
    EC_POINT * point = EC_POINT_new(group);
    BN_CTX * ctx = BN_CTX_new();
    BIGNUM * bn = BN_bin2bn(pkinit_1024_dhprime, sizeof(pkinit_1024_dhprime), NULL);

    EC_POINT_mul(group, point, bn, NULL, NULL, ctx);

    BN_CTX_free(ctx);
    EC_GROUP_free(group);
    BN_free(bn);

    ec_key = EC_KEY_new_by_curve_name(API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256);
    if (!EC_KEY_set_public_key(ec_key, point))
      goto int_err;
    sca_debugLog(LOG_INFO, "+ [libcrypto] DUMMY pub key load success\n");

    EC_POINT_free(point);
  }

  //if(priv_status)
  if(priv_status && fPrivate)
  {
    if(API_SUCCESS !=
        sca_BgpsecSetEcPrivateKey(filePrefix, &ec_key, API_BGPSEC_ALGO_ID_256))
    {
      sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to load a bgpsec private key from an SKI\n");
      goto int_err;
    }
    sca_debugLog(LOG_INFO, "+ [libcrypto] priv key load success\n");
  }

  if (!EC_KEY_check_key(ec_key)) {
    sca_debugLog(LOG_ERR, "+ [libcrypto] EC_KEY_check failed: EC key check error\n");
    goto int_err;
  }
  sca_debugLog(LOG_INFO, "+ [libcrypto] successfully finished to check the bgpsec EC keys\n");


  if(fPrivate)
  {
    sca_debugLog(LOG_INFO,
        "\n---- [ENCODING TEST4] EC Private Key convert to DER ----\n\n");
    unsigned char *ep, *p, *p2;
    int           eplen;


#if defined (HEX_DEBUG_OUTPUT)
    if( LOG_DEBUG <= g_loglevel )
      PrintPrivPubKeyHex(ec_key);
#endif

    eplen = i2d_ECPrivateKey(ec_key, NULL);
    if (!eplen)
    {
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_EC_LIB);
      goto int_err;
    }

    ep = (unsigned char *) OPENSSL_malloc(eplen);
    sca_debugLog(LOG_INFO,"[%s:%d] ep data pointer:%p\n", __FUNCTION__, __LINE__, ep);
    if (!ep)
    {
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_MALLOC_FAILURE);
      goto int_err;
    }

    p = p2 = ep;
    if(!(eplen = i2d_ECPrivateKey(ec_key, &p)))
    {
      OPENSSL_free(ep);
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_EC_LIB);
      goto int_err;
    }

    if (ec_key)
      EC_KEY_free(ec_key);

#if defined (HEX_DEBUG_OUTPUT)
    sca_debugLog(LOG_INFO, "\n ----- [Encoding TEST4] Encoding buf (priv + [pub]) hex [len:%d]----- \n", eplen);

    if( LOG_DEBUG <= g_loglevel )
    {
      for(i=0; i< eplen; i++)
      {
        printf("%02x ", ep[i]);
      }
      printf("\n\n");
    }
#endif

#if defined (DECODING_TEST_CODE)
    /*
     *  Decoding Test
     */
    EC_KEY* recon_key=NULL;
    recon_key = d2i_ECPrivateKey(NULL, (const unsigned char**)&p2, eplen);

    if (!recon_key)
    {
      ECerr(EC_F_ECKEY_PRIV_DECODE, EC_R_DECODE_ERROR);
      goto int_err;
    }

#if defined (HEX_DEBUG_OUTPUT)
    if( LOG_DEBUG <= g_loglevel )
      PrintPrivPubKeyHex(recon_key);
#endif

    /* calculate public key (if necessary) */
    if(EC_KEY_get0_public_key(recon_key) == NULL)
    {
      //unsigned int old_flags = EC_KEY_get_enc_flags(recon_key);
      //if(recon_key->enc_flag & EC_PKEY_NO_PUBKEY)
      {
        if(pub_status)
        {
          if (API_SUCCESS !=
              sca_BgpsecSetEcPublicKey(filePrefix, &recon_key, API_DEFAULT_CURVE))
          {
            sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to load a bgpsec pub key from an SKI\n");
            goto int_err;
          }
          sca_debugLog(LOG_INFO, "+ [libcrypto] pub key load success\n");
        }
      }
    }

    if (recon_key)
      EC_KEY_free(recon_key);
#endif /* DECODING_TEST_CODE */

    /* feeding return data */
    key->keyLength = eplen;
    key->keyData = (u_int8_t*)ep;
  }

  /* calling for loading pub key */
  else
  {
    sca_debugLog(LOG_INFO, "\n---------- [ENCODING TEST5] EC Public Key convert to DER ------------\n\n");

#if defined(USE_EC_PUB_OCTET_KEY)
    unsigned char *ep, *p, *p2;
    int           eplen;

#if defined (HEX_DEBUG_OUTPUT)
    if( LOG_DEBUG <= g_loglevel )
      PrintPrivPubKeyHex(ec_key);
#endif

    eplen = i2o_ECPublicKey(ec_key, NULL);
    if (!eplen)
    {
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_EC_LIB);
      goto int_err;
    }

    ep = (unsigned char *) OPENSSL_malloc(eplen);
    sca_debugLog(LOG_INFO,"[%s:%d] ep data pointer:%p\n", __FUNCTION__, __LINE__, ep);
    if (!ep)
    {
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_MALLOC_FAILURE);
      goto int_err;
    }

    p = p2 = ep;
    if(!(eplen = i2o_ECPublicKey(ec_key, &p)))
    {
      OPENSSL_free(ep);
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_EC_LIB);
      goto int_err;
    }

    if (ec_key)
      EC_KEY_free(ec_key);

#if defined (HEX_DEBUG_OUTPUT)
    if( LOG_DEBUG <= g_loglevel )
    {
      printf("\n ----- [Encoding TEST5] Encoding  buf (public) hex [len:%d]----- \n", eplen);
      for(i=0; i< eplen; i++)
      {
        printf("%02x ", ep[i]);
      }
      printf("\n\n");
    }
#endif


#if defined (DECODING_TEST_CODE)
    /*
     * decoding test
     */
    EC_KEY* ecdsa_key = EC_KEY_new();
    if(ecdsa_key==NULL)
    {
      sca_debugLog(LOG_ERR, " fail to create a new eckey \n");
      goto int_err;
    }

    EC_GROUP * ecgroup = EC_GROUP_new_by_curve_name(API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256);
    if(ecgroup == NULL)
    {
      sca_debugLog(LOG_ERR, " fail to create a new group \n");
      goto int_err;
    }

    int setGroupStatus = EC_KEY_set_group(ecdsa_key, ecgroup);
    if(!setGroupStatus)
    {
      sca_debugLog(LOG_ERR, " fail to set group for ec key \n");
      goto int_err;
    }

    /* We have parameters now set public key */
    if (!o2i_ECPublicKey(&ecdsa_key, &p2, eplen))
    {
      ECerr(EC_F_ECKEY_PUB_DECODE, EC_R_DECODE_ERROR);
      sca_debugLog(LOG_ERR, " EC pub key failed \n");
      goto int_err;
    }

    EC_KEY* recon_key = ecdsa_key;


#if defined (HEX_DEBUG_OUTPUT)
    if( LOG_DEBUG <= g_loglevel )
      PrintPrivPubKeyHex(recon_key );
#endif


    /* calculate public key (if necessary) */
    if(EC_KEY_get0_public_key(recon_key) == NULL)
    {
      if(pub_status)
      {
        if (API_SUCCESS !=
            sca_BgpsecSetEcPublicKey(filePrefix, &recon_key, API_DEFAULT_CURVE))
        {
          sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to load a bgpsec pub key from an SKI\n");
          goto int_err;
        }
        sca_debugLog(LOG_INFO, "+ [libcrypto] pub key load success\n");
      }
    }

    if (recon_key)
      EC_KEY_free(recon_key);

#endif /* DECODING_TEST_CODE */
#endif /* USE_EC_PUB_OCTET_KEY */


#if defined(USE_EC_PUB_EVP_PKEY)
    unsigned char *ep, *p, *p2;
    int           eplen;
    BIO             *out = BIO_new_fp(stdout, BIO_NOCLOSE);
    EVP_PKEY        *pkey = EVP_PKEY_new();

    if(pkey==NULL)
    {
      ECerr(EC_F_EC_KEY_NEW, EC_R_BUFFER_TOO_SMALL);
      ERR_print_errors(out);
      sca_debugLog(LOG_ERR,"[%s:%d] PUBKEY generate error \n", __FUNCTION__, __LINE__);
      goto int_err;
    }

    //EVP_PKEY_assign_EC_KEY(pkey, ec_key);
    EVP_PKEY_set1_EC_KEY(pkey, ec_key);

    eplen = i2d_PUBKEY(pkey, NULL);
    if (!eplen)
    {
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_EC_LIB);
      ERR_print_errors(out);
      sca_debugLog(LOG_ERR,"[%s:%d] i2d PUBKEY error \n", __FUNCTION__, __LINE__);
      goto int_err;
    }

    ep = (unsigned char *) OPENSSL_malloc(eplen);
    sca_debugLog(LOG_INFO,"[%s:%d] ep data pointer:%p\n", __FUNCTION__, __LINE__, ep);
    if (!ep)
    {
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_MALLOC_FAILURE);
      ERR_print_errors(out);
      goto int_err;
    }

    p = p2 = ep;
    if(!(eplen = i2d_PUBKEY(pkey, &p)))
    {
      OPENSSL_free(ep);
      ECerr(EC_F_ECKEY_PRIV_ENCODE, ERR_R_EC_LIB);
      ERR_print_errors(out);
      goto int_err;
    }

    /* release resources */
    if (ec_key)
      EC_KEY_free(ec_key);
    EVP_PKEY_free(pkey);

#if defined (HEX_DEBUG_OUTPUT)
    if( LOG_DEBUG <= g_loglevel )
    {
      printf("\n ----- [Encoding TEST5] Encoding  PUBKEY buf (public) hex [len:%d]----- \n", eplen);
      for(i=0; i< eplen; i++)
      {
        printf("%02x ", ep[i]);
      }
      printf("\n\n");
    }
#endif

#if defined (DECODING_TEST_CODE)
    /*
     * decoding test
     */
    EVP_PKEY *pkey_tmp;

    /* We have parameters now set public key */
    pkey_tmp = d2i_PUBKEY(NULL, (const unsigned char**)&p2, eplen);
    if (!pkey_tmp)
    {
      ECerr(EC_F_ECKEY_PUB_DECODE, EC_R_DECODE_ERROR);
      ERR_print_errors(out);
      sca_debugLog(LOG_ERR, " PUBKEY pub key failed \n");
      goto int_err;
    }

    EC_KEY * recon_key = EVP_PKEY_get1_EC_KEY(pkey_tmp);

    if(recon_key ==NULL)
    {
      sca_debugLog(LOG_ERR, " fail to create a new eckey from EVP_PKEY \n");
      goto int_err;
    }

#if defined (HEX_DEBUG_OUTPUT)
    if( LOG_DEBUG <= g_loglevel )
      PrintPrivPubKeyHex(recon_key );
#endif


    /* calculate public key (if necessary) */
    if(EC_KEY_get0_public_key(recon_key) == NULL)
    {
      if(pub_status)
      {
        if (API_SUCCESS !=
            sca_BgpsecSetEcPublicKey(filePrefix, &recon_key, API_DEFAULT_CURVE))
        {
          sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to load a bgpsec pub key from an SKI\n");
          goto int_err;
        }
        sca_debugLog(LOG_INFO, "+ [libcrypto] pub key load success\n");
      }
    }

    if (recon_key)
      EC_KEY_free(recon_key);

#endif /* DECODING_TEST_CODE */
#endif /* USE_EC_PUB_EVP_PKEY */


    /* feeding return data */
    key->keyLength = eplen;
    key->keyData = (u_int8_t*)ep;
  }

  //key->keyData = (u_int8_t*)ec_key;
  sca_debugLog(LOG_INFO, "+ [libcrypto] successfully finished to convert bgpsec EC keys to DER\n");

  return API_LOADKEY_SUCCESS;


int_err:
  if (ec_key) EC_KEY_free(ec_key);
  if(key) key->keyData = NULL;
  //if(buff) OPENSSL_free(buff);
  return API_LOADKEY_FAILURE;

}

/**
* convinient function for EVP_PKEY handling
*
* @param evp_pkey
* @param privkey
* @param privkey_len
* @param pubkey
* @param pubkey_len
* @param bn_ctx
*
* @return  1 on success, 0 on error
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


/**
 * convinient function for duplicating EVP_PKEY
 *
 * @param key
 *
 * @return  EVP_PKEY instance, otherwise NULL
 */
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
 * same with stcl_BgpsecVerifySignatureSSL function except this one uses key information
 *
 * @param sslVerifyData: BGPSecSignData strucutre
 * @param keys: used as a container which include key info
 * @param keyCnt: the number of keys
 *
 * @return status sucess(API_BGPSEC_VERIFY_SUCCESS), failure(API_BGPSEC_VERIFY_FAILURE)
 */
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

