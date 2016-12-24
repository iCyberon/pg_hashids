#include "postgres.h"
#include "fmgr.h"

#include "hashids.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

extern Datum hash_encode( PG_FUNCTION_ARGS );
extern Datum hash_encode_salt( PG_FUNCTION_ARGS );
extern Datum hash_decode( PG_FUNCTION_ARGS );

char *to_char(text *what)
{
    size_t len = VARSIZE(what)-VARHDRSZ;
    char *dup = palloc(len+1);
    memcpy(dup, VARDATA(what), len);
    dup[len] = 0;
    return dup;
}

PG_FUNCTION_INFO_V1( hash_encode );
Datum
hash_encode( PG_FUNCTION_ARGS )
{
  if (PG_ARGISNULL(0)) {
    PG_RETURN_NULL();
  }

  // Declaration
  text *hash_string;
  hashids_t *hashids;
  unsigned int bytes_encoded;
  char hash[512];

  // Arguments
  unsigned long long input = PG_GETARG_INT64(0);

  hashids = hashids_init(NULL);

  bytes_encoded = hashids_encode_one(hashids, hash, input);
  hash_string = (text *)palloc( bytes_encoded );

  SET_VARSIZE(hash_string, bytes_encoded + VARHDRSZ);
  strncpy( VARDATA(hash_string), hash, bytes_encoded );

  hashids_free(hashids);
  PG_RETURN_TEXT_P( hash_string );
}

PG_FUNCTION_INFO_V1( hash_encode_salt );
Datum
hash_encode_salt( PG_FUNCTION_ARGS )
{
  if (PG_ARGISNULL(0)) {
    PG_RETURN_NULL();
  }

  if (PG_ARGISNULL(1)) {
    PG_RETURN_NULL();
  }

  // Declaration
  text *hash_string;
  hashids_t *hashids;
  unsigned int bytes_encoded;
  char hash[512];

  // Arguments
  unsigned long long input = PG_GETARG_INT64(0);
  text* salt = PG_GETARG_TEXT_P(1);

  hashids = hashids_init2(to_char(salt), (PG_ARGISNULL(2)) ? 0 : PG_GETARG_INT32(2));

  bytes_encoded = hashids_encode_one(hashids, hash, input);
  hash_string = (text *)palloc( bytes_encoded );

  SET_VARSIZE(hash_string, bytes_encoded + VARHDRSZ);
  strncpy( VARDATA(hash_string), hash, bytes_encoded );

  hashids_free(hashids);
  PG_RETURN_TEXT_P( hash_string );
}

PG_FUNCTION_INFO_V1( hash_decode );
Datum
hash_decode( PG_FUNCTION_ARGS )
{
  if (PG_ARGISNULL(0)) {
    PG_RETURN_NULL();
  }

  if (PG_ARGISNULL(1)) {
    PG_RETURN_NULL();
  }

  // Declaration
  unsigned long long numbers[1];
  hashids_t *hashids;

  // Arguments
  text* hash = PG_GETARG_TEXT_P(0);
  text* salt = PG_GETARG_TEXT_P(1);
  unsigned int length = PG_GETARG_INT64(2);

  hashids = hashids_init2(to_char(salt), length);
  hashids_decode(hashids, to_char(hash), numbers);

  hashids_free(hashids);
  PG_RETURN_INT64( numbers[0] );
}