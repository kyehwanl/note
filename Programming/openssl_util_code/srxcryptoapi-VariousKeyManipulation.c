#include <syslog.h>
#include <libconfig.h>
#include <dlfcn.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <openssl/bio.h>

#include "srxcryptoapi.h"

// Below might be removed again
#ifdef CPU_64
#define CT_INT long
#else
#define CT_INT int
#endif

#define MAX_CMD_LEN              1024

#define CRYPTO_CFG_FILE          "srxcryptoapi.conf"

#define SCA_KEY_VOLT             "key_volt"
#define SCA_LIBRARY_NAME         "library_name"
#define SCA_LIBRARY_CONF         "library_conf"

#define SCA_VALIDATE             "method_validate"
#define SCA_EXT_VALIDATE         "method_exitValidate"
#define SCA_SIGN_WITH_KEY        "method_sign_with_key"
#define SCA_SIGN_WITH_ID         "method_sign_with_id"
#define SCA_REGISTER_PRIVATE_KEY "method_registerPrivateKey"
#define SCA_UNREGISTER_PRIVATE_KEY "method_unregisterPrivateKey"
#define SCA_REGISTER_PUBLIC_KEY    "method_registerPublicKey"
#define SCA_UNREGISTER_PUBLIC_KEY  "method_unregisterPublicKey"
#define SCA_IS_EXTENDED            "method_isExtended"
#define SCA_IS_PRIVATE_KEY_STORAGE "method_isPrivateKeyStorage"

#define SCA_DEF_VALIDATE             "validate"
#define SCA_DEF_EXT_VALIDATE           "extValidate"
#define SCA_DEF_SIGN_WITH_KEY        "sign_with_key"
#define SCA_DEF_SIGN_WITH_ID         "sign_with_id"
#define SCA_DEF_REGISTER_PRIVATE_KEY "registerPrivateKey"
#define SCA_DEF_UNREGISTER_PRIVATE_KEY "unregisterPrivateKey"
#define SCA_DEF_REGISTER_PUBLIC_KEY    "registerPublicKey"
#define SCA_DEF_UNREGISTER_PUBLIC_KEY  "unregisterPublicKey"
#define SCA_DEF_IS_EXTENDED            "isExtended"
#define SCA_DEF_IS_PRIVATE_KEY_STORAGE "isPrivateKeyStorage"

#ifndef SYSCONFDIR
#define SYSCONFDIR               "/etc"
#endif // SYSCONFDIR

#define SYS_CFG_FILE SYSCONFDIR "/" CRYPTO_CFG_FILE
#define LOC_CFG_FILE "./" CRYPTO_CFG_FILE

#if HAVE_LTDL_H
#include <ltdl.h>
int ltdl;
lt_dlhandle module;

#ifndef lt__PROGRAM__LTX_preloaded_symbols
#define lt_preloaded_symbols    lt_libltdl_LTX_preloaded_symbols
extern LT_DLSYM_CONST lt_dlsymlist lt_libltdl_LTX_preloaded_symbols[];
#endif
#endif


// Default logging information will be changed once configuration is loaded.
static int g_loglevel = LOG_INFO;
// term_debug is needed for the bgpsec_openssl library to determine if debugging
// has to be performed. This value will be set in srxCryptoInit
unsigned long term_debug = LOG_ERR;

// Default function implementation.

/**
 * This is the internal wrapper function. Currently it does return only the
 * error code and provides a debug log.
 *
 * @param bgpsec_path The BGPSEC Path Segment
 * @param number_keys The number of keys provided
 * @param keys The array of keys
 * @param prefix pointer to the prefix.
 * @param localAS the callers local AS number.
 *
 * @return -1: error -> implementation missing
 */
int wrap_validate(BgpsecPathAttr* bgpsec_path, u_int16_t number_keys,
                  BGPSecKey** keys, void *prefix, u_int32_t localAS )
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'validate'\n");
  return -1;
}

/**
 * Perform BGPSEC path validation. (Optional) This function uses the list of
 * registered public keys and returns the validation state or -1 for an error.
 * The caller manages the memory and MUST assure the memory is intact until
 * the function returns. The implementation itself DOES NOT modify the given
 * data.
 *
 * @param bgpsec_path pointer to the BGPSEC path attribute.
 * @param prefix pointer to the prefix.
 * @param localAS the callers local AS number.
 * @param extCode contains more information in case the validation value is
 *                invalid. 0: validation failed, 1: key not found.
 *
 * @return -1: error -> Missing implementation
 */
int wrap_extValidate(BgpsecPathAttr* bgpsec_path, void *prefix,
    u_int32_t localAS, u_int8_t* extCode)
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'wrap_extValidate'\n");
  return -1;
}

/**
 * Sign the given BGPSEC path data with the provided key. This implementation
 * does not sign the path.
 *
 * @param bgpsec_data The BGPSEc data to be signed.
 * @param key The key to be used for signing
 *
 * @return 1 for success, 0 for failure
 */
int wrap_sign_with_key(BGPSecSignData* bgpsec_data, BGPSecKey* key)
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'sign_with_key'\n");
  return 0;
}

/**
 *
 * Wrapper for sign the given BGPSEC path data with the provided key. This
 * implementation does not sign the path.
 *
 * @param dataLength The length of the data to be signed.
 * @param data The data to be signed.
 * @param keyID The id of the key
 * @param sigLen The length og the preallocated signature buffer
 * @param signature The pre-allocated buffer used to store the signature in.
 *
 * @return 0 failure, 1 success
 */
int wrap_sign_with_id(u_int16_t dataLength, u_int8_t* data, u_int8_t keyID,
                      u_int16_t sigLen, u_int8_t* signature)
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'sign_with_id'\n");
  return 0;
}

/**
 * Register the private key. This method does not store the key. the return
 * value is 0
 *
 * @param key The key to be stored
 *
 * @return 0
 */
u_int8_t wrap_registerPrivateKey(BGPSecKey* key)
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'registerPrivateKey'\n");
  return 0;
}

/**
 * Unregister the Private key. This method actually does not register unregister
 * the private key. It returns 0
 *
 * @param keyID The key id to unregister.
 *
 * @return 0
 */
u_int8_t wrap_unregisterPrivateKey(u_int8_t keyID)
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper "
                           "'unregisterPrivateKey'\n");
  return 0;
}

/**
 * Register the public key. This method does not store the key. the return
 * value is 0
 *
 * @param key The key to be stored
 *
 * @return 0
 */
u_int8_t wrap_registerPublicKey(BGPSecKey* key)
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'registerPublicKey'\n");
  return 0;
}

/**
 * Unregister the Private key. This method actually does not register unregister
 * the private key. It returns 0
 *
 * @param keyID The key id to unregister.
 *
 * @return 0
 */
int wrap_unregisterPublicKey(BGPSecKey* key)
{
  // Return an error for missing implementation.
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'unregisterPublicKey'\n");
  return 0;
}

/**
 * This method determines if the API provides the extended public key
 * management. In this case the extended validation method extValidate can be
 * called.
 *
 * @return 0: Does NOT provide the extended method. 1: does provide extended
 *         functionality
 */
int wrap_isExtended()
{
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'isExtended'\n");
  return 1;
}

/**
 * Return 1 if this API allows the storage of private keys, otherwise 0.
 *
 * @return 0: Does not provide private key storage, 1: Does provide key
 *         private storage
 */
int wrap_isPrivateKeyStorage()
{
  sca_debugLog (LOG_DEBUG, "Called local test wrapper 'isPrivateKeyStorage'\n");
  return 1;
}


/**
 * Look if the configuration file is specified and if not, attempt to load the
 * default one in the system configuration folder or the current folder.
 *
 * @param api The SRxCryptoAPI object.
 *
 * @return 0 if no configuration file is specified and no file is found.
 */
static int _checkConfigFile(SRxCryptoAPI* api)
{
  int retVal = 0;

  // 1st check if file is provided in api
  if (api->configFile != NULL)
  {
    sca_debugLog(LOG_INFO, "Use custom crypto configuration located in %s\n",
                           api->configFile);
    retVal = 1;
  }
  else
  {
    FILE* file;
    // Find configuration file.
    //1. Check for system configuration
    file = fopen(SYS_CFG_FILE , "r");
    if (file != NULL)
    {
      // System configuration exist
      api->configFile = SYS_CFG_FILE;
      fclose(file);
      retVal = 1;
      sca_debugLog(LOG_INFO, "Use crypto configuration located in %s\n",
                            SYS_CFG_FILE);
    }
    else
    {
      //2. Check for local configuration
      file = fopen(LOC_CFG_FILE, "r");
      if (file != NULL)
      {
        fclose(file);
        retVal = 1;
        api->configFile = LOC_CFG_FILE;
        sca_debugLog(LOG_INFO, "Use crypto configuration located in %s\n",
                               LOC_CFG_FILE);
      }
    }
  }

  return retVal;
}

/**
 * Initialize the crypto api by reading the crypto configuration and linking the
 * implementation library to the crypto pi object. The configuration is located
 * in the system config folder or the current "./" folder. In case another
 * location is requested the attribute api->configFile must be set.
 *
 * @param api the crypto api container.
 *
 * @return int 0 for failure 1 for success.
 */
int srxCryptoInit(SRxCryptoAPI* api)
{
  int retVal = 0;

#ifdef MEM_CHECK
  mallopt(M_CHECK_ACTION, 3);
  sca_debugLog(LOG_INFO, "CryptoAPI activated M_CHECK_ACTION 3. To disable, "
                        "reconfigure without --enable-mcheck and recompile.\n");
#endif

#if HAVE_LTDL_H
  LTDL_SET_PRELOADED_SYMBOLS();
  ltdl = lt_dlinit();
#endif

  if (api->libHandle != NULL)
  {
    sca_debugLog(LOG_ERR, "API already initialized.\n");
    return retVal;
  }

  if (api == NULL)
  {
    sca_debugLog(LOG_ERR, "API instance NULL.\n");
    return retVal;
  }

  // First initialize the api and set the pointers to null.
  api->libHandle = NULL;
  api->registerPrivateKey = NULL;
  api->sign_with_id = NULL;
  api->sign_with_key = NULL;
  api->validate = NULL;

  // Read Configuration script
  config_t         cfg;
  config_setting_t *set = NULL;

  const char* str_library_name = NULL;
  const char* str_library_conf = NULL;
  const char* str_method_validate = NULL;
  const char* str_method_extValidate = NULL;
  const char* str_method_sign_with_key = NULL;
  const char* str_method_sign_with_id = NULL;
  const char* str_method_registerPrivateKey = NULL;
  const char* str_method_unregisterPrivateKey = NULL;
  const char* str_method_registerPublicKey = NULL;
  const char* str_method_unregisterPublicKey = NULL;
  const char* str_method_isExtended = NULL;
  const char* str_method_isPrivateKeyStorage = NULL;

  bool useLocal = false;

  if (!_checkConfigFile(api))
  {
    sca_debugLog(LOG_WARNING, "No configuration File specified, "
                              "use default local settings\n");
    useLocal = true;
  }
  else // Load the Configuration.
  {
    config_init(&cfg);
    sca_debugLog(LOG_INFO, "Use configuration file \"%s\"\n", api->configFile);

    if (config_read_file(&cfg, api->configFile))
    {
      /* debug level */
      if(config_lookup_int(&cfg, "debug-type", &g_loglevel))
      {
        sca_debugLog(LOG_INFO, "- debug type: %d\n", g_loglevel);
      }
      else
      {
        // Loglevel not specified, switch to default log level ERROR.
        g_loglevel = LOG_ERR;
        sca_debugLog(LOG_INFO, "- debug type: not configured! use value %d \n",
                               g_loglevel);
      }
      // Update library log level to configured one.
      term_debug = g_loglevel;

      // Set the default key location if configured.
      const char* key_volt;
      if (config_lookup_string(&cfg, SCA_KEY_VOLT, &key_volt))
      {
        sca_SetKeyPath((char *)key_volt);
        void* file = fopen(key_volt , "r");
        if (!file)
        {
          sca_debugLog(LOG_WARNING, "%s - Invalid directory \"%s\"\n",
                       SCA_KEY_VOLT, key_volt);
        }
        else
        {
          fclose(file);
          sca_debugLog(LOG_INFO, "- %s=\"%s\"\n", SCA_KEY_VOLT, key_volt);
        }
      }

      // Load the mapping information.
      if (config_lookup_string(&cfg, SCA_LIBRARY_CONF, &str_library_conf))
      {
        set = config_lookup(&cfg, str_library_conf);
        // The configuration requires all parameters to be available, even if
        // the implementation is missing.
        if (set != NULL)
        {
          retVal = 1;
          if (!config_setting_lookup_string(set, SCA_LIBRARY_NAME,
                                            &str_library_name))
          {
//            retVal = 0;
            sca_debugLog(LOG_WARNING, "- %s: MISSING\n", SCA_LIBRARY_NAME);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n",
                                   SCA_LIBRARY_NAME, str_library_name);
          }

          if (!config_setting_lookup_string(set, SCA_VALIDATE,
                                            &str_method_validate))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n", SCA_VALIDATE);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n",
                                   SCA_VALIDATE, str_method_validate);
          }

          if (!config_setting_lookup_string(set, SCA_EXT_VALIDATE,
                &str_method_extValidate))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n", SCA_EXT_VALIDATE);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n",
                SCA_EXT_VALIDATE, str_method_extValidate);
          }

          if (!config_setting_lookup_string(set, SCA_SIGN_WITH_KEY,
                                            &str_method_sign_with_key))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n", SCA_SIGN_WITH_KEY);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n",
                                   SCA_SIGN_WITH_KEY, str_method_sign_with_key);
          }

          if (!config_setting_lookup_string(set, SCA_SIGN_WITH_ID,
                                            &str_method_sign_with_id))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n", SCA_SIGN_WITH_ID);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n",
                                   SCA_SIGN_WITH_ID, str_method_sign_with_id);
          }

          if (!config_setting_lookup_string(set, SCA_REGISTER_PRIVATE_KEY,
                                            &str_method_registerPrivateKey))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n", SCA_REGISTER_PRIVATE_KEY);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n",
                       SCA_REGISTER_PRIVATE_KEY, str_method_registerPrivateKey);
          }

          if (!config_setting_lookup_string(set, SCA_UNREGISTER_PRIVATE_KEY,
                                            &str_method_unregisterPrivateKey))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n",SCA_UNREGISTER_PRIVATE_KEY);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n", SCA_UNREGISTER_PRIVATE_KEY,
                str_method_unregisterPrivateKey);
          }

          if (!config_setting_lookup_string(set, SCA_REGISTER_PUBLIC_KEY,
                &str_method_registerPublicKey))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n", SCA_REGISTER_PUBLIC_KEY);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n",
                SCA_REGISTER_PUBLIC_KEY, str_method_registerPublicKey);
          }

          if (!config_setting_lookup_string(set, SCA_UNREGISTER_PUBLIC_KEY,
                &str_method_unregisterPublicKey))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n",SCA_UNREGISTER_PUBLIC_KEY);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n", SCA_UNREGISTER_PUBLIC_KEY,
                str_method_unregisterPublicKey);
          }

          if (!config_setting_lookup_string(set, SCA_IS_EXTENDED,
                &str_method_isExtended))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n",SCA_IS_EXTENDED);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n", SCA_IS_EXTENDED,
                str_method_isExtended);
          }

          if (!config_setting_lookup_string(set, SCA_IS_PRIVATE_KEY_STORAGE,
                &str_method_isPrivateKeyStorage))
          {
//            retVal = 0;
            sca_debugLog(LOG_ERR, "- %s: MISSING\n",SCA_IS_PRIVATE_KEY_STORAGE);
          }
          else
          {
            sca_debugLog(LOG_INFO, "- %s=\"%s\"\n", SCA_IS_PRIVATE_KEY_STORAGE,
                str_method_isPrivateKeyStorage);
          }

        }
      }
      else
      {
        sca_debugLog(LOG_WARNING, "%s - Parameter library_conf is missing, use "
                                  "local mapping!\n", api->configFile);
        useLocal = true;
        retVal = 0;
      }
    }
    else
    {
      sca_debugLog(LOG_ERR, "%s:%d - %s\n", api->configFile,
                   cfg.error_line, cfg.error_text);
    }

    // Use library an load / map functions]
    if (retVal)
    {
#if HAVE_LTDL_H
      module = lt_dlopen(str_library_name);
      if(module != 0)
      {
        api->libHandle = (void*) module;
      }
#else
      // Map the library
      api->libHandle = dlopen(str_library_name, RTLD_LOCAL | RTLD_LAZY);
#endif
      if (!api->libHandle)
      {
        sca_debugLog(LOG_ERR, "Library %s could not be opened (%s)!\n",
                     str_library_name, dlerror());
        retVal = 0;
      }
      else
      {
        char* error = NULL;
        // clear any existing errors.
        dlerror();
        bool loadDefault = false;
        // Now bind the functions.

        // Bind validate
        loadDefault = true;
        if (str_method_validate != NULL)
        {
          if (strlen(str_method_validate) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_VALIDATE, str_method_validate);
#if HAVE_LTDL_H
            api->validate = lt_dlsym(module, str_method_validate);
            loadDefault = lt_dlerror() != NULL;
#else
            api->validate = dlsym(api->libHandle, str_method_validate);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_VALIDATE);
#if HAVE_LTDL_H
          api->validate = lt_dlsym(api->libHandle, SCA_DEF_VALIDATE);
          error = lt_dlerror();
#else
          api->validate = dlsym(api->libHandle, SCA_DEF_VALIDATE);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_VALIDATE);
            api->validate = wrap_validate;
          }
        }

        // Bind extValidate
        loadDefault = true;
        if (str_method_extValidate != NULL)
        {
          if (strlen(str_method_extValidate) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_EXT_VALIDATE, str_method_extValidate);
#if HAVE_LTDL_H
            api->extValidate = lt_dlsym(module, str_method_extValidate);
            loadDefault = lt_dlerror() != NULL;
#else
            api->extValidate = dlsym(api->libHandle, str_method_extValidate);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_EXT_VALIDATE);
#if HAVE_LTDL_H
          api->extValidate = lt_dlsym(api->libHandle, SCA_DEF_EXT_VALIDATE);
          error = lt_dlerror();
#else
          api->extValidate = dlsym(api->libHandle, SCA_DEF_EXT_VALIDATE);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_EXT_VALIDATE);
            api->extValidate = wrap_extValidate;
          }
        }

        // Bind sign_with_id
        loadDefault = true;
        if (str_method_sign_with_id != NULL)
        {
          if (strlen(str_method_sign_with_id) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_SIGN_WITH_ID, str_method_sign_with_id);
#if HAVE_LTDL_H
            api->sign_with_id = lt_dlsym(module, str_method_sign_with_id);
            loadDefault = lt_dlerror() != NULL;
#else
            api->sign_with_id = dlsym(api->libHandle, str_method_sign_with_id);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_SIGN_WITH_ID);
#if HAVE_LTDL_H
          api->sign_with_id = lt_dlsym(module, SCA_DEF_SIGN_WITH_ID);
          error = lt_dlerror();
#else
          api->sign_with_id = dlsym(api->libHandle, SCA_DEF_SIGN_WITH_ID);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_SIGN_WITH_ID);
            api->sign_with_id = wrap_sign_with_id;
          }
        }

        // Bind sign_with_key
        loadDefault = true;
        if (str_method_sign_with_key != NULL)
        {
          if (strlen(str_method_sign_with_key) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_SIGN_WITH_KEY, str_method_sign_with_key);
#if HAVE_LTDL_H
            api->sign_with_key = lt_dlsym(module, str_method_sign_with_key);
            loadDefault = lt_dlerror() != NULL;
#else
            api->sign_with_key = dlsym(api->libHandle, str_method_sign_with_key);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_SIGN_WITH_KEY);
#if HAVE_LTDL_H
          api->sign_with_key = lt_dlsym(api->libHandle, SCA_DEF_SIGN_WITH_KEY);
          error = lt_dlerror();
#else
          api->sign_with_key = dlsym(api->libHandle, SCA_DEF_SIGN_WITH_KEY);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_SIGN_WITH_KEY);
            api->sign_with_key = wrap_sign_with_key;
          }
        }

        // Bind registerPrivateKey
        loadDefault = true;
        if (str_method_registerPrivateKey != NULL)
        {
          if (strlen(str_method_registerPrivateKey) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_REGISTER_PRIVATE_KEY, str_method_registerPrivateKey);
#if HAVE_LTDL_H
            api->registerPrivateKey = lt_dlsym(api->libHandle,
                str_method_registerPrivateKey);
            loadDefault = lt_dlerror() != NULL;
#else
            api->registerPrivateKey = dlsym(api->libHandle,
                str_method_registerPrivateKey);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_REGISTER_PRIVATE_KEY);
#if HAVE_LTDL_H
          api->registerPrivateKey = lt_dlsym(api->libHandle,
              SCA_DEF_REGISTER_PRIVATE_KEY);
          error = lt_dlerror();
#else
          api->registerPrivateKey = dlsym(api->libHandle,
              SCA_DEF_REGISTER_PRIVATE_KEY);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_REGISTER_PRIVATE_KEY);
            api->registerPrivateKey = wrap_registerPrivateKey;
          }
        }

        // Bind unregisterPrivateKey
        loadDefault = true;
        if (str_method_unregisterPrivateKey != NULL)
        {
          if (strlen(str_method_unregisterPrivateKey) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_UNREGISTER_PRIVATE_KEY,
                str_method_unregisterPrivateKey);
#if HAVE_LTDL_H
            api->unregisterPrivateKey = lt_dlsym(api->libHandle,
                str_method_unregisterPrivateKey);
            loadDefault = lt_dlerror() != NULL;
#else
            api->unregisterPrivateKey = dlsym(api->libHandle,
                str_method_unregisterPrivateKey);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_UNREGISTER_PRIVATE_KEY);
#if HAVE_LTDL_H
          api->unregisterPrivateKey = lt_dlsym(api->libHandle,
              SCA_DEF_UNREGISTER_PRIVATE_KEY);
          error = lt_dlerror();
#else
          api->unregisterPrivateKey = dlsym(api->libHandle,
              SCA_DEF_UNREGISTER_PRIVATE_KEY);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_UNREGISTER_PRIVATE_KEY);
            api->unregisterPrivateKey = wrap_unregisterPrivateKey;
          }
        }


        // Bind registerPublicKey
        loadDefault = true;
        if (str_method_registerPublicKey != NULL)
        {
          if (strlen(str_method_registerPublicKey) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_REGISTER_PUBLIC_KEY, str_method_registerPublicKey);
#if HAVE_LTDL_H
            api->registerPublicKey = lt_dlsym(api->libHandle,
                str_method_registerPublicKey);
            loadDefault = lt_dlerror() != NULL;
#else
            api->registerPublicKey = dlsym(api->libHandle,
                str_method_registerPublicKey);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_REGISTER_PUBLIC_KEY);
#if HAVE_LTDL_H
          api->registerPublicKey = lt_dlsym(api->libHandle,
              SCA_DEF_REGISTER_PUBLIC_KEY);
          error = lt_dlerror();
#else
          api->registerPublicKey = dlsym(api->libHandle,
              SCA_DEF_REGISTER_PUBLIC_KEY);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_REGISTER_PUBLIC_KEY);
            api->registerPublicKey = wrap_registerPublicKey;
          }
        }

        // Bind unregisterPublicKey
        loadDefault = true;
        if (str_method_unregisterPublicKey != NULL)
        {
          if (strlen(str_method_unregisterPublicKey) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_UNREGISTER_PUBLIC_KEY,
                str_method_unregisterPublicKey);
#if HAVE_LTDL_H
            api->unregisterPublicKey = lt_dlsym(api->libHandle,
                str_method_unregisterPublicKey);
            loadDefault = lt_dlerror() != NULL;
#else
            api->unregisterPublicKey = dlsym(api->libHandle,
                str_method_unregisterPublicKey);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_UNREGISTER_PUBLIC_KEY);
#if HAVE_LTDL_H
          api->unregisterPublicKey = lt_dlsym(api->libHandle,
              SCA_DEF_UNREGISTER_PUBLIC_KEY);
          error = lt_dlerror();
#else
          api->unregisterPublicKey = dlsym(api->libHandle,
              SCA_DEF_UNREGISTER_PUBLIC_KEY);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_UNREGISTER_PUBLIC_KEY);
            api->unregisterPublicKey = wrap_unregisterPublicKey;
          }
        }

        // Bind isExtended
        loadDefault = true;
        if (str_method_isExtended != NULL)
        {
          if (strlen(str_method_isExtended) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_IS_EXTENDED,
                str_method_isExtended);
#if HAVE_LTDL_H
            api->isExtended = lt_dlsym(api->libHandle, str_method_isExtended);
            loadDefault = lt_dlerror() != NULL;
#else
            api->isExtended = dlsym(api->libHandle, str_method_isExtended);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_IS_EXTENDED);
#if HAVE_LTDL_H
          api->isExtended = lt_dlsym(api->libHandle,  SCA_DEF_IS_EXTENDED);
          error = lt_dlerror();
#else
          api->isExtended = dlsym(api->libHandle,  SCA_DEF_IS_EXTENDED);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_IS_EXTENDED);
            api->isExtended = wrap_isExtended;
          }
        }

        // Bind isPrivateKeyStorage
        loadDefault = true;
        if (str_method_isPrivateKeyStorage != NULL)
        {
          if (strlen(str_method_isPrivateKeyStorage) > 0)
          {
            sca_debugLog(LOG_INFO, "Linking \"%s\" to \"%s\"!\n",
                SCA_DEF_IS_PRIVATE_KEY_STORAGE,
                str_method_isPrivateKeyStorage);
#if HAVE_LTDL_H
            api->isPrivateKeyStorage = lt_dlsym(api->libHandle,
                str_method_isPrivateKeyStorage);
            loadDefault = lt_dlerror() != NULL;
#else
            api->isPrivateKeyStorage = dlsym(api->libHandle,
                str_method_isPrivateKeyStorage);
            loadDefault = dlerror() != NULL;
            dlerror();
#endif
          }
        }
        if (loadDefault)
        {
          // Try the default method name
          sca_debugLog(LOG_INFO, "Linking \"%s\" to default method!\n",
              SCA_DEF_IS_PRIVATE_KEY_STORAGE);
#if HAVE_LTDL_H
          api->isPrivateKeyStorage = lt_dlsym(api->libHandle,
              SCA_DEF_IS_PRIVATE_KEY_STORAGE);
          error = lt_dlerror();
#else
          api->isPrivateKeyStorage = dlsym(api->libHandle,
              SCA_DEF_IS_PRIVATE_KEY_STORAGE);
          error = dlerror();
#endif
          if (error != NULL)
          {
            sca_debugLog(LOG_WARNING, "Linking error (%s) for function \"%s\""
                " - use internal wrapper!\n",
                error, SCA_DEF_IS_PRIVATE_KEY_STORAGE);
            api->isPrivateKeyStorage = wrap_isPrivateKeyStorage;
          }
        }
      }
    }
    else
    {
      // Not all field where found in configuration.
      if (useLocal)
      {
        sca_debugLog(LOG_INFO, "Use local wrapper for all methods!\n");

        api->sign_with_id       = wrap_sign_with_id;
        api->sign_with_key      = wrap_sign_with_key;
        api->validate           = wrap_validate;
        api->registerPrivateKey = wrap_registerPrivateKey;
        api->unregisterPrivateKey = wrap_unregisterPrivateKey;
        retVal = 1;
      }
      else
      {
        // Mapping completely failed!
        sca_debugLog(LOG_ERR, "Library linking error.\n", str_library_name);
      }
    }
    // Free configuration structure
    config_destroy(&cfg);
  }

  // Check if configuration worked:
  if (retVal == 0)
  {
    sca_debugLog(LOG_ERR, "Library not loaded!\n");
    // Just in case the failure occurred not only during the beginning,
    // close the dll lnk and set all functions to NULL.
    srxCryptoUnbind(api);
  }

  return retVal;
}

/**
 * close the linked library.
 *
 * @return 0
 */
int srxCryptoUnbind(SRxCryptoAPI* api)
{
  if (api->libHandle != NULL)
  {
    dlclose(api->libHandle);
  }
  api->libHandle = NULL;
  api->registerPrivateKey = NULL;
  api->unregisterPrivateKey = NULL;
  api->sign_with_id = NULL;
  api->sign_with_key = NULL;
  api->validate = NULL;

#if HAVE_LTDL_H
  if(ltdl ==0)
  {
    if(module !=0)
      lt_dlclose(module);
    lt_dlexit();
  }
#endif
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Helper Functions
////////////////////////////////////////////////////////////////////////////////

static char _keyPath [MAXPATHLEN];

/**
 * This function sets the key path.
 *
 * @param key_path the path to the keys (\0 terminated).
 *
 * @return 0 error, 1 success
 *
 */
int sca_SetKeyPath (char* key_path)
{
  int retVal = 0;

  if (strlen(key_path) < (MAX_CMD_LEN-1))
  {
    memset(_keyPath, '\0', MAXPATHLEN);
    sprintf(_keyPath, "%s%c", key_path, '\0');
    retVal = 1;
  }

  return retVal;
}



/**
 * This method generates a filename out of the given SKI.
 *
 * @param filenamebuf The pre-allocated filename buffer which will be filled
 *                    with the filename.
 * @param filenamebufLen Maximum length of the buffer.
 * @param ski       The SKI where the filename will be generated from
 *
 * @return The filename buffer that was handed over.
 */
char* sca_FindDirInSKI (char* filenamebuf, size_t filenamebufLen, u_int8_t* ski)
{
  char *path = _keyPath;
  char skiHex[SKI_HEX_LENGTH+1];
  char* skiPtr = skiHex;
  int idx;

  // Transform the ski into a hex string
  for (idx = 0; idx < SKI_LENGTH; idx++)
  {
    skiPtr += sprintf(skiPtr, "%02X", ski[idx]);
  }

  filenamebuf[filenamebufLen-1] = '\0';

  BIO_snprintf(filenamebuf, filenamebufLen-1, "%s/%2.2s/%4.4s/%s",
      path,
      skiHex, skiHex + 2, skiHex + 6);

  sca_debugLog(LOG_INFO, "+ [libcrypto] Key File name prefix: %s \n",
               filenamebuf);

  return filenamebuf;
}


/**
 * Writes the loging information.
 *
 * @param level The logging level
 * @param format The format of the logging info
 * @param ...
 */
void sca_debugLog( int level, const char *format, ...)
{
  char buffer[0xff] = {0};
  char *slevel;

  va_list ap;

  if( level <= g_loglevel || 1)
  {
    va_start(ap, format);
    vsnprintf( buffer, 0xff, format, ap);
    va_end(ap);

    switch (level)
    {
      case LOG_DEBUG    : slevel = "DEBUG"; break;
      case LOG_WARNING  : slevel = "WARNING"; break;
      case LOG_INFO     : slevel = "INFO"; break;
      case LOG_ERR      : slevel = "ERROR"; break;
      default: slevel = "??";
    }
    fprintf(stdout, "[SRxCryptoAPI - %s] %s", slevel, buffer);
    //vsyslog((int)level, format, ap);
  }
}


/* Openssl ecdsa include files */
#include <openssl/ec.h>
#include <openssl/ecdsa.h>

#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
//#include <openssl/evp.h>


#include <sys/stat.h>

//#define DEFAULT_CERTFILE_EXT  "cert"
//#define DEFAULT_KEYFILE_EXT   "key"
#define DEFAULT_CERTFILE_EXT  "pub"
#define DEFAULT_KEYFILE_EXT   "private"
#define MAX_EXT             3
#define API_SUCCESS         0
#define API_FAILURE         -1
#define API_DEFAULT_CURVE   1

#define API_LOADKEY_SUCCESS 1
#define API_LOADKEY_FAILURE 0

#define API_BGPSEC_ALGO_ID_256          1
#define API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256 NID_X9_62_prime256v1

#define PRIVKEY_SIZE 32

int sca_BgpsecSetEcPublicKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId);
int sca_BgpsecSetEcPrivateKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId);

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

    printf(" ======== ASN STRING V_ASN1_SEQUENCE ===== [%d]\n", pstr->length);
    int i;
    for(i=0; i< pstr->length; i++)
    {
      printf("%02x ", pstr->data[i]);
    }
    printf("\n\n");
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
    EC_GROUP * group = EC_GROUP_new_by_curve_name(API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256);
    EC_POINT * point = EC_POINT_new(group);
    BN_CTX * ctx = BN_CTX_new();
    BIGNUM * bn = BN_bin2bn((const unsigned char*)key, PRIVKEY_SIZE, NULL);

    EC_POINT_mul(group, point, bn, NULL, NULL, ctx);

    BN_CTX_free(ctx);
    EC_GROUP_free(group);
    BN_free(bn);

    if (!EC_KEY_set_public_key(ec_key, point))
      goto int_err;

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
    printf("\n---------- [ENCODING TEST4] EC Private Key convert to DER ------------\n\n");
    unsigned char *ep, *p, *p2;
    int           eplen;


#if defined (HEX_DEBUG_OUTPUT)
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
    printf("\n ----- [Encoding TEST4] Encoding buf (priv + [pub]) hex [len:%d]----- \n", eplen);
    for(i=0; i< eplen; i++)
    {
      printf("%02x ", ep[i]);
    }
    printf("\n\n");
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
    printf("\n---------- [ENCODING TEST5] EC Public Key convert to DER ------------\n\n");

#if defined(USE_EC_PUB_OCTET_KEY)
    unsigned char *ep, *p, *p2;
    int           eplen;

#if defined (HEX_DEBUG_OUTPUT)
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
    printf("\n ----- [Encoding TEST5] Encoding  buf (public) hex [len:%d]----- \n", eplen);
    for(i=0; i< eplen; i++)
    {
      printf("%02x ", ep[i]);
    }
    printf("\n\n");
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
    printf("\n ----- [Encoding TEST5] Encoding  PUBKEY buf (public) hex [len:%d]----- \n", eplen);
    for(i=0; i< eplen; i++)
    {
      printf("%02x ", ep[i]);
    }
    printf("\n\n");
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
 * @brief  receives the file location string and processes to get a certificate into bio
 *
 * @param fn describing user's file location
 *
 * @return if success, returns a pointer of X509 certs, otherwise, NULL
 */
X509 * sca_GetPublicKey(const char *fn)
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
 * @brief  makes ecdsa public key as an EC_POINT type and set the appropriate curve value according to curve id.
 *          then, it fills  public key string into EC_POINT type variable
 *
 * @param filePrefix
 * @param ecdsa_key : it may return to the caller, if ecdsa checks ok
 * @param curveId
 *
 * @return if success, API_SUCCESS, or if failed, API_FAILURE
 */
int sca_BgpsecSetEcPublicKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId)
{
  char szPubkey[4096];
  size_t pubkeyLen;

  EC_POINT      *ecpointPublicKey = NULL;
  EC_GROUP      *ecGroup=NULL;
  X509          *cert = NULL;

  memset(szPubkey, 0x00, MAXPATHLEN);

  /* load the public key */
  cert = sca_GetPublicKey(filePrefix);
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
      || !EC_POINT_oct2point(ecGroup, ecpointPublicKey, (const unsigned char *)szPubkey, pubkeyLen, NULL)
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
    sca_debugLog(LOG_INFO, "+ [libcrypto] Public key check OK\n");
  }

  return API_SUCCESS;

// openssl cleanup
err_cleanup:
  if (*ecdsa_key)  EC_KEY_free(*ecdsa_key);
  if (ecGroup) EC_GROUP_free(ecGroup);
  if (cert) X509_free(cert);
  //
  // CRYPTO_cleanup_all_ex_data();
  // ERR_free_strings();
  // ERR_remove_state(0);
  return API_FAILURE;
}


/**
 * @brief receives the file location string(fn) and processes to get a PEM-type  private
 * key into bio pointer
 *
 * @param fn which describes the file location string
 *
 * @return if success, EVP_PKEY type privateKey pointer, otherwise NULL
 */
EVP_PKEY * sca_GetPrivateKey(const char *fn)
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



/**
 * @brief Load the private key
 *
 * @param filePrefix
 * @param ecdsa_key
 *
 * @return
 */
int sca_BgpsecSetEcPrivateKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId)
{

  EVP_PKEY  *privateKey = NULL;
  EC_KEY    *tmpEckey;
  const BIGNUM    *bn;

  if(curveId == API_BGPSEC_ALGO_ID_256)
    curveId = API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256;

  /* get a private key from a file */
  privateKey =  sca_GetPrivateKey(filePrefix);

  if(!privateKey)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to get the private key from the file\n");
    return API_FAILURE;
  }

  tmpEckey = EVP_PKEY_get1_EC_KEY(privateKey);
  bn = EC_KEY_get0_private_key(tmpEckey);

  /* fill the received ec_key structure with big_number data of private key */
  //if (!EC_KEY_set_private_key(*ecdsa_key, privateKey->pkey.ec->priv_key))
  if (!EC_KEY_set_private_key(*ecdsa_key, bn))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to set the private key into the EC key object\n");
    return API_FAILURE;
  }

  return API_SUCCESS;
}


unsigned char *sca_new_PrivateKeyStream(const char *fn, EC_KEY **ecdsa_key, unsigned int *length)
{
  int           fd, size_read;
  char          szFileName[MAXPATHLEN];
  char          buffer[2048];
  unsigned char* privateKeyStream = NULL;
  EVP_PKEY      *privateKey = NULL;
  char          *p1 = buffer;
  EC_KEY        *tmpEckey;

  snprintf(szFileName, sizeof(szFileName)-1, "%s." DEFAULT_KEYFILE_EXT, fn);

  fd = fopen (szFileName, "rb");
  if(!fd)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Unable to load the private key from the file \n");
    return NULL;
  }
  size_read = fread(buffer,sizeof(char),2048,fd);

  fclose(fd);
  if(size_read <= 0)
    return NULL;

  privateKeyStream = (unsigned char*)malloc(size_read);
  if(!privateKeyStream)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Unable to allocate the private key memory \n");
    return NULL;
  }
  memcpy(privateKeyStream, buffer, size_read);

  /* transform to internal structure */
  privateKey = d2i_PrivateKey(EVP_PKEY_EC, NULL, &p1, size_read);
  if (!privateKey)
  {
    sca_debugLog(LOG_ERR, "+ Unable to load the DER form of private key\n");
    if (privateKeyStream)
      free (privateKeyStream);
    return NULL;
  }

  /* get EC KEY */
  tmpEckey = EVP_PKEY_get1_EC_KEY(privateKey);


  EVP_PKEY_free(privateKey);
  /* return valudes */
  *length = size_read;
  *ecdsa_key = tmpEckey;
  return privateKeyStream;
}



/**
 * @brief Load the private key using the file prefix into the ecdsa key pointer
 *
 * @param filePrefix: file location prefix
 * @param ecdsa_key: a pointer of ecdsa key pointer
 * @param curveId
 *
 * @return Success:0 , failure: -1
 */
int sca_BgpsecSetEcPrivateKey(const char *filePrefix, EC_KEY **ecdsa_key, int curveId)
{

  EVP_PKEY  *privateKey = NULL;
  EC_KEY    *tmpEckey;
  const BIGNUM    *bn;

  if(curveId == API_BGPSEC_ALGO_ID_256)
    curveId = API_BGPSEC_OPENSSL_ID_SHA256_ECDSA_P_256;

  /* get a private key from a file */
  privateKey =  sca_GetPrivateKey(filePrefix);

  if(!privateKey)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to get the private key from the file\n");
    return API_FAILURE;
  }

  tmpEckey = EVP_PKEY_get1_EC_KEY(privateKey);
  bn = EC_KEY_get0_private_key(tmpEckey);

  /* fill the received ec_key structure with big_number data of private key */
  //if (!EC_KEY_set_private_key(*ecdsa_key, privateKey->pkey.ec->priv_key))
  if (!EC_KEY_set_private_key(*ecdsa_key, bn))
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to set the private key into the EC key object\n");
    return API_FAILURE;
  }

  return API_SUCCESS;
}

unsigned char *new_pub_ecpoint2Der(const unsigned char* bin_ecpoint)
{
  char arrPubDerHead[26] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86,
    0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
    0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00};

  unsigned char *pubDer=NULL;
  pubDer = (unsigned char*)malloc(SIZE_DER_PUBKEY);
  if(!pubDer)
  {
    sca_debugLog(LOG_ERR, "+ [libcrypto] Failed to malloc to get DER\n");
    return NULL;
  }
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

  return pubDer;
}

/**
* @brief
*
* @param buff
* @param out_key
*
* @return
*/
int getEcKeyFromHex(const char *buff, EC_KEY *out_key)
{
  EC_KEY    *key = NULL;
  EC_POINT *pub_key;
  const EC_GROUP *group;

  key = EC_KEY_new_by_curve_name(NID_sect163k1);
  group = EC_KEY_get0_group(key);
  pub_key = EC_POINT_new(group);

  EC_POINT_hex2point(group,
      "369368AF243193D001E39CE76BB1D5DA08A9BC0A63307AB352338E5EA5C0E05A0C2531866F3E3C2702", pub_key, NULL);

  EC_KEY_set_public_key(key, pub_key);

  if (!EC_KEY_check_key(key)) {
    printf("EC_KEY_check_key failed:\n");
    printf("%s\n",ERR_error_string(ERR_get_error(),NULL));
  } else {
    printf("Public key verified OK\n");
  }
}
