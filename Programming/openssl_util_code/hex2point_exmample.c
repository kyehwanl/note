/**
* http://stackoverflow.com/questions/1171207/how-do-i-feed-openssl-random-data-for-use-in-ecdsa-signing
*
* this shows how to get ec_key from char strings
*/
void example1 (void)
{

  EC_KEY    *key = NULL;
  EC_POINT *pub_key;
  const EC_GROUP *group;

  SSL_library_init();
  SSL_load_error_strings();

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
