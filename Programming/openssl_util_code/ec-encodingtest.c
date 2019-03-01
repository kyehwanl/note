/* codingerror.c - Demonstration of a coding/decoding error in OpenSSL.

Version of 2005.11.29.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/obj_mac.h>    /* For NID_secp521r1. */
#include <openssl/ecdsa.h>
#include <openssl/ecdh.h>
#include <openssl/err.h>
//#include <openssl/ec_lcl.h>
#include <ec_lcl.h>


#define MAX_PRIVATE_BYTESTRING_LEN      2000

static long n_tests;
EC_KEY *private_key;
unsigned char private_bytestring[ MAX_PRIVATE_BYTESTRING_LEN ];
long private_bytestring_len;


static void die( const char *m )
{
  fprintf( stderr, "Fatal error after %ld tests: %s\n",
           n_tests, m );
  ERR_print_errors_fp( stderr );
  exit( 1 );
}


static void show_bytestring( long len, const unsigned char *b )
{
  printf("length: %d\n", len);
  while ( len-- > 0 )  printf( " %02X", *b++ );
  printf( "\n" );
}


static void dump_key( EC_KEY *k )
{
  //if (!EC_POINT_get_affine_coordinates_GFp(group, P, x, y, ctx))
    //die( "Error in EC_POINT_get_affine_coordinates_GFp." );
}

/**
 *  print out public key and priv key
 */
static void print_pub_priv_hex (EC_KEY *ec_key_orig)
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
  printf("\n\n");
  printf("\n ----- printing out pub key hex ----- \n");
  BN_CTX *ctx1;
  ctx1 = BN_CTX_new();
  char * ccc = EC_POINT_point2hex(ec_key_orig->group, ec_key_orig->pub_key, POINT_CONVERSION_UNCOMPRESSED/*4*/, ctx1);
  printf("pub: %s\n", ccc);
  printf("\n\n");
}



static void test_once( void )
{
  unsigned char *temp_pointer;
  EC_KEY *reconstituted_private_key;

  //private_key = EC_KEY_new_by_curve_name( NID_secp521r1 );
  private_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

  if ( private_key == NULL )
    die( "Couldn't get new point by curve name." );

  if ( EC_KEY_generate_key( private_key ) == 0 )
    die( "Error generating a private key." );


  printf("\n\n +++++ [STEP 1] original form of generated EC_KEY +++++- \n");
  /**
   *  print out public key and priv key
   */
  print_pub_priv_hex (private_key);



  printf("\n\n +++++ [STEP 2-1] PRIVATE KEY transform internal to DER   +++++- \n");
  /**
   * test to see if transform from DER to Internal PRIVATE key vise versa
   */
  const unsigned char *ip2 = private_bytestring;
  temp_pointer = private_bytestring;
  printf("1: temp pointer:%p priv_byte:%p ip2:%p\n", temp_pointer, private_bytestring, ip2);
  private_bytestring_len = i2d_ECPrivateKey( private_key,
                                             &temp_pointer );
  if ( private_bytestring_len == 0 )
    die( "private_bytestring_len == 0" );

  printf("2: temp pointer:%p priv_byte:%p ip2:%p\n", temp_pointer, private_bytestring, ip2);
  //show_bytestring( private_bytestring_len, temp_pointer );
  printf("\n <PRIVATE KEY DER string> \n");
  show_bytestring( private_bytestring_len, ip2);

  //memset(private_bytestring, 0, 2000);
  reconstituted_private_key = d2i_ECPrivateKey( NULL,
                                                &ip2,
                                                private_bytestring_len );
#if 1
  printf( ( reconstituted_private_key == NULL ) ? "bad:  " : "good: " );
  //show_bytestring( private_bytestring_len, private_bytestring );
  printf("\n\n +++++ [STEP 2-2] PRIVATE KEY DER string to interanl EC_key form (reconstructing) +++++- \n");
  print_pub_priv_hex (reconstituted_private_key );
#else
  if ( reconstituted_private_key == NULL )
    {
      printf( "An error occurred for this private key:\n" );
      dump_key( private_key );
      die( "reconstituted_private_key == NULL" );
    }
#endif


#if 1
  printf("\n\n +++++ [STEP 3-1] PUB KEY transform internal to DER   +++++- \n");
  /**
   * test to see if transform from DER to Internal PUB key vise versa
   */
  unsigned char pub_bytestring[ MAX_PRIVATE_BYTESTRING_LEN/2 ];
  unsigned char *p1, *p2;
  unsigned long pub_bytestring_len=0;
  EC_KEY *reconstituted_pub_key;
  p1 = p2 = pub_bytestring;
  printf("\n+ [pub] 1: p1 pointer:%p pub_byte:%p p2:%p\n", p1, pub_bytestring, p2);
  pub_bytestring_len = i2d_EC_PUBKEY( private_key, &p1);

  if ( pub_bytestring_len == 0 )
    die( "pub_bytestring_len == 0" );

  printf("+ [pub] 2: p1 pointer:%p pub_byte:%p p2:%p\n", p1, pub_bytestring, p2);

  // show hex string
  printf("\n < PUB KEY DER string > \n");
  show_bytestring( pub_bytestring_len, p2);

  reconstituted_pub_key = (EC_KEY*)d2i_EC_PUBKEY(NULL, &p2, pub_bytestring_len);
  printf( ( reconstituted_pub_key == NULL ) ? "bad:  " : "good: " );

  // show ec key
  printf("\n +++++ [STEP 3-2] PUB KEY DER string to interanl EC_key form (reconstructing)  +++++- \n");
  print_pub_priv_hex (reconstituted_pub_key);
#endif



  printf("\n\n +++++ [STEP 4] making DER from EC_POINT hex   +++++- \n");


  /**
   *  free
   */
  EC_KEY_free( reconstituted_private_key );
  EC_KEY_free( reconstituted_pub_key );
  EC_KEY_free( private_key );
}


int main( void )
{
  ERR_load_crypto_strings();

  //for ( n_tests = 0 ; n_tests < 1000 ; ++n_tests )
  for ( n_tests = 0 ; n_tests < 1; ++n_tests )
    test_once();

  return 0;
}


#if 0 /* how to compile */

(1) how to compile
gcc -g -O0 -I/usr/local/ssl/include/ -I/users/kyehwanl/Download/bgpsec-test/openssl-1.0.0j/crypto/ec/ -o ec-encodingtest ec-encodingtest.c -L/usr/local/ssl/lib/ -lssl -lcrypto -ldl



(2) additional information about keys

  $ openssl ec -in C30433FA1975FF193181458FB902B501EA9789DC.private.der -inform der -text -noout
  read EC key
  Private-Key: (256 bit)
  priv:
      00:be:93:cc:1a:32:b7:2a:d0:aa:1c:88:83:35:2e:
      e5:9e:f6:37:bc:e5:33:af:08:e8:f9:9e:b3:18:fc:
      f0:a2:8f
  pub:
      04:d6:d3:27:ec:6c:f7:05:80:8d:cd:80:0f:97:f4:
      9f:4a:5a:f0:ff:77:10:d7:de:d3:12:8c:bd:fc:1c:
      12:59:92:4a:8b:0a:f4:7b:ff:6c:35:9f:41:4d:e4:
      0b:4d:1f:8d:57:9c:c1:c0:d6:47:be:34:8e:3e:ee:
      06:d5:9f:29:f3
  ASN1 OID: prime256v1

  $ openssl asn1parse -in C30433FA1975FF193181458FB902B501EA9789DC.pubkey.der -inform der
      0:d=0  hl=2 l=  89 cons: SEQUENCE
      2:d=1  hl=2 l=  19 cons: SEQUENCE
      4:d=2  hl=2 l=   7 prim: OBJECT            :id-ecPublicKey
     13:d=2  hl=2 l=   8 prim: OBJECT            :prime256v1
     23:d=1  hl=2 l=  66 prim: BIT STRING

  $ openssl asn1parse -in C30433FA1975FF193181458FB902B501EA9789DC.private.der -inform der
      0:d=0  hl=2 l= 119 cons: SEQUENCE
      2:d=1  hl=2 l=   1 prim: INTEGER           :01
      5:d=1  hl=2 l=  32 prim: OCTET STRING      [HEX DUMP]:BE93CC1A32B72AD0AA1C8883352EE59EF637BCE533AF08E8F99EB318FCF0A28F
     39:d=1  hl=2 l=  10 cons: cont [ 0 ]
     41:d=2  hl=2 l=   8 prim: OBJECT            :prime256v1
     51:d=1  hl=2 l=  68 cons: cont [ 1 ]
     53:d=2  hl=2 l=  66 prim: BIT STRING

   -- pub key in DER
  30 59 30 13 06 07 2a 86 48 ce 3d 02 01 06 08 2a
  86 48 ce 3d 03 01 07 03 42 00 04 d6 d3 27 ec 6c
  f7 05 80 8d cd 80 0f 97 f4 9f 4a 5a f0 ff 77 10
  d7 de d3 12 8c bd fc 1c 12 59 92 4a 8b 0a f4 7b
  ff 6c 35 9f 41 4d e4 0b 4d 1f 8d 57 9c c1 c0 d6
  47 be 34 8e 3e ee 06 d5 9f 29 f3


  -- priv key in DER
  30 77 02 01 01 04 20 be  93 cc 1a 32 b7 2a d0 aa
  1c 88 83 35 2e e5 9e f6  37 bc e5 33 af 08 e8 f9
  9e b3 18 fc f0 a2 8f a0  0a 06 08 2a 86 48 ce 3d
  03 01 07 a1 44 03 42 00  04 d6 d3 27 ec 6c f7 05
  80 8d cd 80 0f 97 f4 9f  4a 5a f0 ff 77 10 d7 de
  d3 12 8c bd fc 1c 12 59  92 4a 8b 0a f4 7b ff 6c
  35 9f 41 4d e4 0b 4d 1f  8d 57 9c c1 c0 d6 47 be
  34 8e 3e ee 06 d5 9f 29  f3

#endif

