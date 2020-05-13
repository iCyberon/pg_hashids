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

static void hashids_error()
{
	switch (hashids_errno) {
	case HASHIDS_ERROR_ALLOC:
		ereport(ERROR, (errcode(ERRCODE_OUT_OF_MEMORY), errmsg("allocation failed")));
		break;
	case HASHIDS_ERROR_ALPHABET_LENGTH:
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("alphabet is too short")));
		break;
	case HASHIDS_ERROR_ALPHABET_SPACE:
		ereport(ERROR,
		        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
		            errmsg("alphabet contains whitespace characters")));
		break;
	case HASHIDS_ERROR_INVALID_HASH:
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("invalid hash")));
		break;
	default:
		ereport(ERROR, (errcode(ERRCODE_EXTERNAL_ROUTINE_EXCEPTION), errmsg("unknown error")));
		break;
	}
}

// based on new_intArrayType contrib/intarray/_int_tool.c
static ArrayType *new_intArrayType(int num)
{
	ArrayType *r;
	int nbytes = ARR_OVERHEAD_NONULLS(1) + sizeof(int64) * num;

	r = (ArrayType *) palloc0(nbytes);

	SET_VARSIZE(r, nbytes);
	ARR_NDIM(r) = 1;
	r->dataoffset = 0;  /* marker for no null bitmap */
	ARR_ELEMTYPE(r) = INT8OID;
	ARR_DIMS(r)[0] = num;
	ARR_LBOUND(r)[0] = 1;

	return r;
}

Datum
id_encode(PG_FUNCTION_ARGS)
{
	// Declaration
	int64 number;
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
		hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2),
		                        text_to_cstring(PG_GETARG_TEXT_P(3)));
	} else {
		hashids = hashids_init(NULL);
	}
	if (!hashids) {
		hashids_error();
	}

	hash = palloc0(hashids_estimate_encoded_size(hashids, 1, (unsigned long long *) &number));
	bytes_encoded = hashids_encode_one(hashids, hash, (unsigned long long) number);
	hash_string = cstring_to_text_with_len(hash, bytes_encoded);

	hashids_free(hashids);
	pfree(hash);

	PG_RETURN_TEXT_P(hash_string);
}

Datum
id_encode_array(PG_FUNCTION_ARGS)
{
	ArrayType *numbers;
	size_t numbers_count;

	// Declaration
	text *hash_string;
	hashids_t *hashids;

	unsigned int bytes_encoded;
	char *hash;

	// Arguments
	numbers = PG_GETARG_ARRAYTYPE_P(0);
	numbers_count = ARR_DIMS(numbers)[0];

	if (array_contains_nulls(numbers)) {
		ereport(ERROR,
		        (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
		            errmsg("null value not allowed for array element")));

	}
	if (PG_NARGS() == 2) {
		hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), 0);
	} else if (PG_NARGS() == 3) {
		hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
	} else if (PG_NARGS() == 4) {
		hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2),
		                        text_to_cstring(PG_GETARG_TEXT_P(3)));
	} else {
		hashids = hashids_init(NULL);
	}
	if (!hashids) {
		hashids_error();
	}

	hash = palloc0(
	    hashids_estimate_encoded_size(hashids, numbers_count, (unsigned long long *) ARR_DATA_PTR(numbers)));
	bytes_encoded = hashids_encode(hashids, hash, numbers_count, (unsigned long long *) ARR_DATA_PTR(numbers));
	hash_string = cstring_to_text_with_len(hash, bytes_encoded);

	hashids_free(hashids);
	pfree(hash);

	PG_RETURN_TEXT_P(hash_string);
}

Datum
id_decode(PG_FUNCTION_ARGS)
{
	// Declaration
	hashids_t *hashids;
	int64 *numbers, *resultValues;
	char *hash;
	size_t numbers_count;
	ArrayType *resultArray;

	if (PG_NARGS() == 2) {
		hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), 0);
	} else if (PG_NARGS() == 3) {
		hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
	} else if (PG_NARGS() == 4) {
		hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2),
		                        text_to_cstring(PG_GETARG_TEXT_P(3)));
	} else {
		hashids = hashids_init(NULL);
	}
	if (!hashids) {
		hashids_error();
	}

	hash = text_to_cstring(PG_GETARG_TEXT_P(0));
	numbers_count = hashids_numbers_count(hashids, hash);
	if (!numbers_count) {
		hashids_error();
	}

	numbers = palloc0(numbers_count * sizeof(int64));
	hashids_decode(hashids, hash, (unsigned long long *) numbers, numbers_count);

	hashids_free(hashids);
	pfree(hash);

	resultArray = new_intArrayType(numbers_count);
	resultValues = (int64 *) ARR_DATA_PTR(resultArray);
	memcpy(resultValues, numbers, numbers_count * sizeof(int64));

	pfree(numbers);

	PG_RETURN_ARRAYTYPE_P(resultArray);
}

Datum
id_decode_once(PG_FUNCTION_ARGS)
{
	// Declaration
	hashids_t *hashids;
	int64 number;
	char *hash;
	size_t numbers_count;

	if (PG_NARGS() == 2) {
		hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), 0);
	} else if (PG_NARGS() == 3) {
		hashids = hashids_init2(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2));
	} else if (PG_NARGS() == 4) {
		hashids = hashids_init3(text_to_cstring(PG_GETARG_TEXT_P(1)), PG_GETARG_INT32(2),
		                        text_to_cstring(PG_GETARG_TEXT_P(3)));
	} else {
		hashids = hashids_init(NULL);
	}
	if (!hashids) {
		hashids_error();
	}

	hash = text_to_cstring(PG_GETARG_TEXT_P(0));
	numbers_count = hashids_numbers_count(hashids, hash);
	if (!numbers_count) {
		hashids_error();
	}

	hashids_decode(hashids, hash, (unsigned long long *) &number, 1);

	hashids_free(hashids);
	pfree(hash);

	PG_RETURN_INT64(number);
}
