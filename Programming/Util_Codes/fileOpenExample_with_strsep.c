
AS_REL_DIR check_AsRelationship(char* as_relationship_file, PATH_LIST* asPathList, uint8_t length)
{
 AS_REL_DIR retVal = ASPA_UNKNOWNSTREAM;
  FILE* fp=NULL;
  char buf[1024];
  uint32_t count=0;

#define NUM_FIELDS 5
  /*
    The as-rel files contain p2p and p2c relationships.  The format is:
    <provider-as>|<customer-as>|-1
    <peer-as>|<peer-as>|0|<source>
  */
  char *field[NUM_FIELDS]; // f[0] provider, f[1] customer, f[2] marks
  const char *delimiters = "|\n";
  uint32_t provider_as   = 0;
  uint32_t customer_as   = 0;
  int option             = 1;

  uint32_t as1 = asPathList[0];
  uint32_t as2 = asPathList[1];

  fp = fopen(as_relationship_file, "rt");
  if(fp)
  {
    while (fgets(buf, 1024, fp) != NULL)
    {
      char *s = buf;
      int len = strlen(buf);

      count++;
      //if (count > 100) break;

      if (!isdigit(*s)) // if a first character is not a number, continue to the next line
        continue;

      for(int i=0;  (field[i] = strsep(&s, delimiters)) != NULL && i < NUM_FIELDS; i++ )
      {
        // Multiple spaces will show up as multiple empty fields. Skip them.
        if( *field[i] == '\0' )
          continue;

        //printf("field[%d]: %s\n", i, field[i]);
      }

      provider_as = strtoul(field[0], NULL, 10);
      customer_as = strtoul(field[1], NULL, 10);
      option      = strtoul(field[2], NULL, 10);

      // find provider as in the file
      if (provider_as == as1 && customer_as == as2)
      {
        retVal = ASPA_UPSTREAM;  // upstream
        break;
      }
      else if (provider_as == as2 && customer_as == as1)
      {
        retVal = ASPA_DOWNSTREAM; // downstream
        break;
      }
      else
        continue;
    }
  }
  else
  {
    LOG(LEVEL_ERROR, "Can't open configuration file [%s]", as_relationship_file);
  }

  printf("total count: %d retrun value: %d\n", count, retVal);
  fclose(fp);

  return retVal;
}
