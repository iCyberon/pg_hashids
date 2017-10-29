#include "postgres.h"
#include "fmgr.h"

#include "funcapi.h"
#include "catalog/pg_type.h"
#include "utils/array.h"
#include "utils/builtins.h"

#include "hashids.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

extern Datum id_encode( PG_FUNCTION_ARGS );
extern Datum id_encode_array( PG_FUNCTION_ARGS );
extern Datum id_decode( PG_FUNCTION_ARGS );
extern Datum id_decode_once( PG_FUNCTION_ARGS );

static char *to_char(text *what)
{
    size_t len = VARSIZE(what)-VARHDRSZ;
    char *dup = palloc(len+1);
    memcpy(dup, VARDATA(what), len);
    dup[len] = 0;
    return dup;
}

static ArrayType * alloc_array(int nelems)
{
  // based on new_intArrayType contrib/intarray/_int.c
  ArrayType * result;
  int nbytes = sizeof(unsigned long long) * nelems + ARR_OVERHEAD_NONULLS(1);
  result = (ArrayType *) palloc(nbytes);
  MemSet(result, 0, nbytes);
  SET_VARSIZE(result, nbytes);
  ARR_NDIM(result) = 1;
  ARR_ELEMTYPE(result) = INT8OID;
  ARR_DIMS(result)[0] = nelems;
  ARR_LBOUND(result)[0] = 1;
  return result;
}

PG_FUNCTION_INFO_V1( id_encode );
Datum
id_encode( PG_FUNCTION_ARGS )
{
  // Declaration
  unsigned long long number;
  text *hash_string;
  hashids_t *hashids;

  unsigned int bytes_encoded;
  char* hash;

  // Arguments
  number = PG_GETARG_INT32(0);

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), to_char(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  hash = calloc(hashids_estimate_encoded_size(hashids, 1, &number), 1);

  bytes_encoded = hashids_encode_one(hashids, hash, number);
  hash_string = (text *)palloc( bytes_encoded );

  SET_VARSIZE(hash_string, bytes_encoded + VARHDRSZ);
  strncpy( VARDATA(hash_string), hash, bytes_encoded );

  hashids_free(hashids);
  PG_RETURN_TEXT_P( hash_string );

  free(hash);
}

PG_FUNCTION_INFO_V1( id_encode_array );
Datum
id_encode_array( PG_FUNCTION_ARGS )
{
  ArrayType *numbers;
  int numbers_count;

  // Declaration
  text *hash_string;
  hashids_t *hashids;

  unsigned int bytes_encoded;
  char* hash;

  // Arguments
  numbers = PG_GETARG_ARRAYTYPE_P(0);
  numbers_count = ARR_DIMS(numbers)[0];

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), to_char(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  hash = calloc(hashids_estimate_encoded_size(hashids, numbers_count, (unsigned long long*) ARR_DATA_PTR(numbers)), 1);

  bytes_encoded = hashids_encode(hashids, hash, numbers_count, (unsigned long long*) ARR_DATA_PTR(numbers));
  hash_string = (text *)palloc( bytes_encoded );

  SET_VARSIZE(hash_string, bytes_encoded + VARHDRSZ);
  strncpy( VARDATA(hash_string), hash, bytes_encoded );

  hashids_free(hashids);
  PG_RETURN_TEXT_P( hash_string );

  free(hash);
}


PG_FUNCTION_INFO_V1( id_decode );
Datum
id_decode( PG_FUNCTION_ARGS )
{
  // Declaration
  hashids_t *hashids;
  unsigned long long *numbers;
  unsigned long long *resultValues;
  int numbers_count;

  ArrayType* resultArray;

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), to_char(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  numbers_count = hashids_numbers_count(hashids, to_char(PG_GETARG_TEXT_P(0)));
  numbers = calloc(numbers_count, sizeof(unsigned long long));

  hashids_decode(hashids, to_char(PG_GETARG_TEXT_P(0)), numbers);
  hashids_free(hashids);

  resultArray = alloc_array(numbers_count);
  resultValues = (unsigned long long *)ARR_DATA_PTR(resultArray);

  memcpy(resultValues, numbers, numbers_count * sizeof(unsigned long long));

  PG_RETURN_ARRAYTYPE_P(resultArray);

  free(numbers);
}

PG_FUNCTION_INFO_V1( id_decode_once );
Datum
id_decode_once( PG_FUNCTION_ARGS )
{
  // Declaration
  hashids_t *hashids;
  unsigned long long *numbers;
  int numbers_count;

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(to_char(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), to_char(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  numbers_count = hashids_numbers_count(hashids, to_char(PG_GETARG_TEXT_P(0)));
  numbers = calloc(numbers_count, sizeof(unsigned long long));

  hashids_decode(hashids, to_char(PG_GETARG_TEXT_P(0)), numbers);
  hashids_free(hashids);

  PG_RETURN_INT64( numbers[0] );

  free(numbers);
}