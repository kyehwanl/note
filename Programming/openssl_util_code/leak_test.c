#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#define CURVE_ECDSA_P_256 NID_X9_62_prime256v1

int main(void)
{

  EC_KEY*   ecdsa_key        = NULL;
  ecdsa_key = EC_KEY_new_by_curve_name(CURVE_ECDSA_P_256);
  //EC_KEY_free(ecdsa_key);

  return 0;
}


/* compile command
 *
 * gcc -g -I/usr/local/ssl/include/ -L/usr/local/ssl/lib -o leak_test leak_test.c -lssl -lcrypto -ldl
 */
