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

PG_FUNCTION_INFO_V1(id_encode);
PG_FUNCTION_INFO_V1(id_encode_array);
PG_FUNCTION_INFO_V1(id_decode);
PG_FUNCTION_INFO_V1(id_decode_once);

// based on new_intArrayType contrib/intarray/_int_tool.c
static ArrayType *new_intArrayType(int num)
{
  ArrayType *r;
  int nbytes = ARR_OVERHEAD_NONULLS(1) + sizeof(unsigned long long) * num;

  r = (ArrayType *) palloc0(nbytes);

  SET_VARSIZE(r, nbytes);
  ARR_NDIM(r) = 1;
  r->dataoffset = 0;			/* marker for no null bitmap */
  ARR_ELEMTYPE(r) = INT8OID;
  ARR_DIMS(r)[0] = num;
  ARR_LBOUND(r)[0] = 1;

  return r;
}

Datum
id_encode(PG_FUNCTION_ARGS)
{
  // Declaration
  unsigned long long number;
  text *hash_string;
  hashids_t *hashids;

  unsigned int bytes_encoded;
  char *hash;

  // Arguments
  number = PG_GETARG_INT64(0);

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), text_to_cstring(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  hash = palloc0(hashids_estimate_encoded_size(hashids, 1, &number));
  bytes_encoded = hashids_encode_one(hashids, hash, number);
  hash_string = cstring_to_text_with_len(hash, bytes_encoded);

  PG_RETURN_TEXT_P(hash_string);
}

Datum
id_encode_array(PG_FUNCTION_ARGS)
{
  ArrayType *numbers;
  int numbers_count;

  // Declaration
  text *hash_string;
  hashids_t *hashids;

  unsigned int bytes_encoded;
  char *hash;

  // Arguments
  numbers = PG_GETARG_ARRAYTYPE_P(0);
  numbers_count = ARR_DIMS(numbers)[0];

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), text_to_cstring(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  hash = palloc0(hashids_estimate_encoded_size(hashids, numbers_count, (unsigned long long *) ARR_DATA_PTR(numbers)));
  bytes_encoded = hashids_encode(hashids, hash, numbers_count, (unsigned long long *) ARR_DATA_PTR(numbers));
  hash_string = cstring_to_text_with_len(hash, bytes_encoded);

  PG_RETURN_TEXT_P(hash_string);
}

Datum
id_decode(PG_FUNCTION_ARGS)
{
  // Declaration
  hashids_t *hashids;
  unsigned long long *numbers, *resultValues;
  char *hash;
  int numbers_count;
  ArrayType *resultArray;

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), text_to_cstring(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  hash = text_to_cstring(PG_GETARG_TEXT_P(0));
  numbers_count = hashids_numbers_count(hashids, hash);
  numbers = palloc0(numbers_count * sizeof(unsigned long long));
  hashids_decode(hashids, hash, numbers);

  resultArray = new_intArrayType(numbers_count);
  resultValues = (unsigned long long *) ARR_DATA_PTR(resultArray);
  memcpy(resultValues, numbers, numbers_count * sizeof(int64));

  PG_RETURN_ARRAYTYPE_P(resultArray);
}

Datum
id_decode_once(PG_FUNCTION_ARGS)
{
  // Declaration
  hashids_t *hashids;
  unsigned long long *numbers;
  char *hash;
  int numbers_count;

  if (PG_NARGS() == 2) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), 0);
  } else if (PG_NARGS() == 3) {
    hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
  } else if (PG_NARGS() == 4) {
    hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2), text_to_cstring(PG_GETARG_TEXT_P(3)));
  } else {
    hashids = hashids_init(NULL);
  }

  hash = text_to_cstring(PG_GETARG_TEXT_P(0));
  numbers_count = hashids_numbers_count(hashids, hash);
  numbers = palloc0(numbers_count * sizeof(unsigned long long));
  hashids_decode(hashids, hash, numbers);

  PG_RETURN_INT64(numbers[0]);
}
